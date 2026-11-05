cmake_minimum_required(VERSION 3.25)
project(ProjectOne LANGUAGES CXX)

include(CTest)

add_executable(ProjectOne VoteR4.cpp Comparator.h Exceptions.h NvraComparator.cpp Search.h Sorter.h NvraComparator.h TemplatedArray.h NvraRecord.cpp NvraRecord.h Enumerator.h OULink.h OULinkedList.h OULinkedListEnumerator.h NvraHasher.h HashTableEnumerator.h Hasher.h HashTable.h NvraHasher.cpp AVLTree.h AVLTreeEnumerator.h AVLTreeOrder.h)
set_target_properties(ProjectOne PROPERTIES CXX_EXTENSIONS OFF)

if(MSVC)
    target_compile_options(ProjectOne PRIVATE /std:c++latest /Zc:__cplusplus)
else()
    target_compile_features(ProjectOne PRIVATE cxx_std_26)
endif()

if(BUILD_TESTING)
    add_test(
        NAME VoteR4Integration
        COMMAND ${CMAKE_COMMAND}
            "-DAPP=$<TARGET_FILE:ProjectOne>"
            "-DTEST_DIR=${CMAKE_CURRENT_BINARY_DIR}/integration-test-data"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/VoteR4Integration.cmake"
    )
endif()
