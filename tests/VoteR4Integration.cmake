if(NOT DEFINED APP OR NOT DEFINED TEST_DIR)
    message(FATAL_ERROR "APP and TEST_DIR must be provided")
endif()

file(MAKE_DIRECTORY "${TEST_DIR}")

function(make_record id value result)
    set(fields)
    foreach(column RANGE 0 23)
        if(column EQUAL 0)
            set(field "${id}")
        elseif(column EQUAL 1)
            set(field "${value}")
        elseif(column EQUAL 3)
            set(field "Alpha")
        elseif(column EQUAL 11)
            set(field "Beta")
        elseif(column EQUAL 12)
            set(field "Gamma")
        else()
            set(field "0")
        endif()
        list(APPEND fields "${field}")
    endforeach()
    string(JOIN "," record ${fields})
    set(${result} "${record}" PARENT_SCOPE)
endfunction()

make_record(1 20 first_record)
make_record(3 10 second_record)
make_record(6 -7 negative_record)
make_record(5 0 malformed_record)
string(REPLACE "5,0," "5,not-a-number," malformed_row "${malformed_record}")
make_record(8 1 extra_column_record)
set(extra_column_row "${extra_column_record},extra")
file(WRITE "${TEST_DIR}/initial.csv"
    "header\n${first_record}\n${second_record}\n${second_record}\n${negative_record}\n${malformed_row}\n7\n${extra_column_row}\n")

make_record(2 15 first_merge_record)
make_record(4 10 second_merge_record)
file(WRITE "${TEST_DIR}/merge.csv"
    "header\n${first_merge_record}\n${second_merge_record}\n")

file(WRITE "${TEST_DIR}/purge.csv"
    "header\n${second_merge_record}\n${negative_record}\n${malformed_row}\n")

file(WRITE "${TEST_DIR}/input.txt"
    "${TEST_DIR}/missing.csv\n"
    "${TEST_DIR}/initial.csv\n"
    "m\n"
    "${TEST_DIR}/merge.csv\n"
    "s\n"
    "\n"
    "s\n"
    "24\n"
    "s\n"
    "-1\n"
    "s\n"
    "1\n"
    "f\n"
    "\n"
    "f\n"
    "24\n"
    "f\n"
    "-1\n"
    "f\n"
    "1\n"
    "not-a-number\n"
    "f\n"
    "1\n"
    "-10\n"
    "f\n"
    "1\n"
    "4294967296\n"
    "f\n"
    "1\n"
    "10\n"
    "f\n"
    "1\n"
    "11\n"
    "f\n"
    "1\n"
    "0\n"
    "f\n"
    "0\n"
    "-1\n"
    "f\n"
    "0\n"
    "not-a-number\n"
    "f\n"
    "0\n"
    "4294967296\n"
    "f\n"
    "0\n"
    "2\n"
    "f\n"
    "3\n"
    "Alpha\n"
    "f\n"
    "2\n"
    "0\n"
    "h\n"
    "\n"
    "p\n"
    "${TEST_DIR}/purge.csv\n"
    "in\n"
    "${TEST_DIR}/inorder.txt\n"
    "q\n")

execute_process(
    COMMAND "${APP}"
    INPUT_FILE "${TEST_DIR}/input.txt"
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error_output
    RESULT_VARIABLE result
    TIMEOUT 30
)

if(NOT result EQUAL 0)
    message(FATAL_ERROR
        "VoteR4 exited with ${result}.\nstdout:\n${output}\nstderr:\n${error_output}")
endif()

if(NOT output MATCHES "NVRA records found: 2\\.")
    message(FATAL_ERROR "Expected a search matching two records.\n${output}")
endif()

if(NOT output MATCHES "NVRA records found: 1\\.")
    message(FATAL_ERROR "Expected the record-ID hash search to find one record.\n${output}")
endif()

if(NOT output MATCHES "NVRA records found: 0")
    message(FATAL_ERROR "Expected the missing-value search to find no records.\n${output}")
endif()

string(REGEX MATCHALL "NVRA records found: 4\\." four_record_searches "${output}")
list(LENGTH four_record_searches four_record_search_count)
if(NOT four_record_search_count EQUAL 2)
    message(FATAL_ERROR "Expected string and unsorted numeric searches to find four records.\n${output}")
endif()

string(REGEX MATCHALL "NVRA records found: 0" empty_searches "${output}")
list(LENGTH empty_searches empty_search_count)
if(NOT empty_search_count EQUAL 2)
    message(FATAL_ERROR "Expected missing numeric searches to find no records.\n${output}")
endif()

foreach(invalid_message
        "Invalid data at line 4: invalid non-negative number in column 2\\."
        "Invalid data at line 5: invalid non-negative number in column 2\\."
        "Invalid data at line 6: too few fields\\."
        "Invalid data at line 7: too many fields\\.")
    if(NOT output MATCHES "${invalid_message}")
        message(FATAL_ERROR "Expected malformed input to report its parse error: ${invalid_message}\n${output}")
    endif()
endforeach()

string(REGEX MATCHALL "Invalid data at line 2: invalid non-negative number in column 2\\." invalid_purge_line_2 "${output}")
string(REGEX MATCHALL "Invalid data at line 3: invalid non-negative number in column 2\\." invalid_purge_line_3 "${output}")
list(LENGTH invalid_purge_line_2 invalid_purge_line_2_count)
list(LENGTH invalid_purge_line_3 invalid_purge_line_3_count)
if(NOT invalid_purge_line_2_count EQUAL 1 OR NOT invalid_purge_line_3_count EQUAL 1)
    message(FATAL_ERROR "Expected malformed purge records to be rejected by the shared parser.\n${output}")
endif()

if(NOT output MATCHES "File is not available\\.")
    message(FATAL_ERROR "Expected an unavailable input file to be rejected before retrying.\n${output}")
endif()

foreach(message "Invalid sort field\\." "Invalid search field\\." "Invalid non-negative field value\\.")
    if(NOT output MATCHES "${message}")
        message(FATAL_ERROR "Expected invalid interactive input to be rejected: ${message}\n${output}")
    endif()
endforeach()

if(NOT output MATCHES "Duplicate record ID 3")
    message(FATAL_ERROR "Expected duplicate record IDs in the initial file to be reported.\n${output}")
endif()

if(NOT output MATCHES "Base Capacity:")
    message(FATAL_ERROR "Expected the hash table display to include its capacity summary.\n${output}")
endif()

string(REPLACE "," ";" expected_record_1 "${first_record}")
string(REPLACE "," ";" expected_record_2 "${first_merge_record}")
string(REPLACE "," ";" expected_record_3 "${second_record}")
file(READ "${TEST_DIR}/inorder.txt" inorder_output)
set(expected_inorder
    "${expected_record_1}\n${expected_record_2}\n${expected_record_3}\nData lines read:")
string(FIND "${inorder_output}" "${expected_inorder}" inorder_position)
if(inorder_position EQUAL -1)
    message(FATAL_ERROR "Expected inorder traversal after purge to contain records 1, 2, 3 in order.\n${inorder_output}")
endif()

if(NOT inorder_output MATCHES "NVRA records in memory: 3")
    message(FATAL_ERROR "Expected purge to leave three records in memory.\n${inorder_output}")
endif()

string(REPLACE "," ";" expected_record_4 "${second_merge_record}")
string(FIND "${inorder_output}" "${expected_record_4}\n" purged_record_position)
if(NOT purged_record_position EQUAL -1)
    message(FATAL_ERROR "Expected purged record 4 to be absent from inorder output.\n${inorder_output}")
endif()
