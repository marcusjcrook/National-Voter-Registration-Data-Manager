#include <iostream>
#include <string>
#include <sstream>
#include <array>
#include <charconv>
#include <expected>
#include <system_error>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <memory>
#include <string_view>

bool parseUnsignedInput(const std::string& input, unsigned int& value) {
    if (input.empty()) {
        return false;
    }

    const auto [end, error] = std::from_chars(input.data(), input.data() + input.size(), value);
    return error == std::errc{} && end == input.data() + input.size();
}
#include "NvraRecord.h"
#include "Sorter.h"
#include "Search.h"
#include "TemplatedArray.h"
#include "Exceptions.h"
#include "NvraComparator.h"
#include "OULinkedList.h"
#include "OULinkedListEnumerator.h"
#include "OULink.h"
#include "HashTable.h"
#include "NvraHasher.h"
#include "HashTableEnumerator.h"
#include "AVLTreeEnumerator.h"
#include "AVLTree.h"
#include "AVLTreeOrder.h"

// method to print search results for binary search
void printSearchResults(long long index, const TemplatedArray<NvraRecord>& array, NvraComparator& comparator) {

    // method to print the records
    long long recordsFound = 0;
    for (int i = 0; i < static_cast<int>(array.getSize()); ++i) {
        if (((index + i) < static_cast<int>(array.getSize())) && comparator.compare(array[index], array[index + i]) == 0) {
            std::cout << array[index + i] << std::endl;
            recordsFound++;
        }
    }
    std::cout << "NVRA records found: " << recordsFound << "." << std::endl;
}

// linear search method if file is not sorted by selected column
void linearSearch(TemplatedArray<NvraRecord>& array, const NvraRecord& record, const NvraComparator& comparator) {

    // search the array for item
    int recordsFound = 0;
    for (unsigned int index = 0; index < array.getSize(); ++index) {
        if (comparator.compare(array[index], record) == 0) {
            recordsFound++;
        }
    }

    // print linear search results
    Sorter<NvraRecord>::sort(array, comparator); // output on ZyBooks had to be printed in order
    for (unsigned int index = 0; index < array.getSize(); ++index) {
        if (comparator.compare(array[index], record) == 0) {
            std::cout << array[index] << std::endl;
        }
    }
    std::cout << "NVRA records found: " << recordsFound << "." << std::endl;
}

// method to print AVLTree by specified order
void printAVL(AVLTreeEnumerator<NvraRecord> treeEnum,
        const long& records, const long& attemptedRecords, const long& validRecords) {

    std::string outputFile;
    std::cout << "Enter output file name: "; // prompt the user for a file name
    std::getline(std::cin, outputFile);


    if (outputFile.empty()) { // no file entered, send to cout
        while (treeEnum.hasNext()) {
            std::cout << treeEnum.next() << std::endl;
        }

        std::cout << "Data lines read: " << attemptedRecords << "; Valid NVRA records read: " << validRecords <<
             "; NVRA records in memory: " << records << std::endl;
    } else { // valid file chosen, so write data to it
        std::ofstream fileToWrite;
        fileToWrite.open(outputFile);

        if (fileToWrite.is_open()) {

            while (treeEnum.hasNext()) {
                fileToWrite << treeEnum.next() << std::endl;
            }

            fileToWrite << "Data lines read: " << attemptedRecords <<
                        "; Valid NVRA records read: " << validRecords << "; NVRA records in memory: " <<
                        records << std::endl;
            fileToWrite.close();
        }
    }

}

// print the data from the array
void outputLoop(const TemplatedArray<NvraRecord>& array, const long& records, const long& attemptedRecords, const long& validRecords) {

    std::string outputFile;
    std::cout << "Enter output file name: ";
    std::getline(std::cin, outputFile);

    // no file name entered, send to cout
    if (outputFile.empty()) {
        for (unsigned int recordIndex = 0; recordIndex < array.getSize(); ++recordIndex) {
            std::cout << array.get(recordIndex) << std::endl;
        }
    } else {
        std::ofstream fileToWrite;
        fileToWrite.open(outputFile);

        // wasn't sure, based on instructions, whether to print to console or only print to file so I did both the be safe
        if (fileToWrite.is_open()) {
            for (unsigned long index = 0; index < array.getSize(); ++index) {
                fileToWrite << array.get(index) << std::endl; // write the data line to the file
                std::cout << array.get(index) << std::endl; // print the line to the console
            }

            // print out information about data
            fileToWrite << "Data lines read: " << attemptedRecords <<
                        "; Valid NVRA records read: " << validRecords << "; NVRA records in memory: " <<
                        records << std::endl;
            fileToWrite.close();
        } else {
            std::cout << "File is not available." << std::endl;
        }
    }

    // information about the records read in
    std::cout << "Data lines read: " << attemptedRecords <<
         "; Valid NVRA records read: " << validRecords << "; NVRA records in memory: " <<
         records << std::endl;
}

// method for sorting portion of data manipulation loop
unsigned int sortLoop(TemplatedArray<NvraRecord>& oldArray) {

    unsigned int result = 0; // returns column by which the data is sorted
    unsigned int sortField = 0;
    std::string sortFieldString; // field by which to search
    std::cout << "Enter sort field (0-23): " << std::endl;
    std::getline(std::cin, sortFieldString);

    if (parseUnsignedInput(sortFieldString, sortField) && sortField <= 23) {
        NvraComparator n(sortField);
        Sorter<NvraRecord>::sort(oldArray, n);
        result = sortField;
    } else {
        std::cout << "Invalid sort field." << std::endl;
    }
    return result;
}

// method for finding portion of data manipulation loop
void findLoop(TemplatedArray<NvraRecord>& oldArray, HashTable<NvraRecord>& hashTable, unsigned int sortedColumn) {
        unsigned int findField = 0;
        std::string findFieldString;
        std::string numberToSearch;
        unsigned int unsignedSearchNumber = 0;
        long long result = 0;
        std::string stringToSearch;
        NvraRecord searchRecord;
        std::cout << "Enter search field (0-23): " << std::endl;
        std::getline(std::cin, findFieldString);

        if (!parseUnsignedInput(findFieldString, findField) || findField > 23) {
            std::cout << "Invalid search field." << std::endl;
            return;
        }

        NvraComparator fieldCompare(findField);
        Sorter<NvraRecord>::sort(oldArray, fieldCompare);

        unsigned int searchIndex = findField;
        if (findField > 3 && findField < 11) {
            searchIndex = findField - 1;
        } else if (findField > 12) {
            searchIndex = findField - 3;
        }

        if (findField == 3 || findField == 11 || findField == 12) {
            std::cout << "Enter exact text on which to search: " << std::endl;
            std::getline(std::cin, stringToSearch);

            const unsigned int stringIndex = findField == 3 ? 0 : (findField == 11 ? 1 : 2);
            searchRecord.setString(stringToSearch, stringIndex);
            NvraComparator stringFieldCompare(findField);
            result = binarySearch(searchRecord, oldArray, stringFieldCompare);

            if (result >= 0) {
                printSearchResults(result, oldArray, stringFieldCompare);
            } else {
                std::cout << "NVRA records found: 0" << std::endl;
            }
            return;
        }

        std::cout << "Enter non-negative field value: " << std::endl;
        std::getline(std::cin, numberToSearch);
        if (!parseUnsignedInput(numberToSearch, unsignedSearchNumber)) {
            std::cout << "Invalid non-negative field value." << std::endl;
            return;
        }

        searchRecord.setNum(unsignedSearchNumber, searchIndex);
        if (findField == 0) {
            try {
                const NvraRecord hashResult = hashTable.find(&searchRecord);
                std::cout << hashResult << std::endl;
                std::cout << "NVRA records found: 1." << std::endl;
            } catch (const ExceptionHashTableAccess&) {
                std::cout << "NVRA records found: 0" << std::endl;
            }
            return;
        }

        NvraComparator numsFieldCompare(findField);
        if (sortedColumn == findField) {
            result = binarySearch(searchRecord, oldArray, numsFieldCompare);
            if (result >= 0) {
                printSearchResults(result, oldArray, numsFieldCompare);
            } else {
                std::cout << "NVRA records found: 0" << std::endl;
            }
        } else {
            linearSearch(oldArray, searchRecord, numsFieldCompare);
        }
    }

// function to create array from AVLTree
void createArrayFromTree(AVLTree<NvraRecord>& oldTree, TemplatedArray<NvraRecord>& array) {

    array.remove(); // delete the contents of the array

    if (oldTree.getSize() == 0) { // AVLTree is empty so simply return
        return;
    }

    NvraRecord recordToAdd;
    NvraRecord *recordPointer;

    AVLTreeEnumerator<NvraRecord> treeEnum(&oldTree, AVLTreeOrder::inorder);

    // conduct an in order traversal of the tree and at elements to array
    while (treeEnum.hasNext()) {
        recordToAdd = treeEnum.next();
        recordPointer = &recordToAdd;
        array.add(recordPointer);
    }

}

// function to create hashTable from AVLTree
void createHashTableFromTree(AVLTree<NvraRecord>& oldTree, HashTable<NvraRecord>& hashTable) {
    hashTable.recreateTable(oldTree.getSize(), hashTable.getBaseCapacity()); // call method to clear hash table

    if (oldTree.getSize() == 0) { // tree is empty, so return
        return;
    }

    NvraRecord recordToAdd;
    NvraRecord *recordPointer;

    AVLTreeEnumerator<NvraRecord> treeEnum(&oldTree, AVLTreeOrder::inorder);

    // conduct an in order traversal of the tree and at elements to hashtable
    while (treeEnum.hasNext()) {
        recordToAdd = treeEnum.next();
        recordPointer = &recordToAdd;
        hashTable.insert(recordPointer);
    }
}

// function to combine old tree and new tree
void mergeOldTreeNewTree(
        std::unique_ptr<AVLTree<NvraRecord>>& oldTree,
        std::unique_ptr<AVLTree<NvraRecord>> newTree,
        long& records,
        NvraComparator& comparator) {

    records = 0;
    if (newTree->getSize() == 0) { // no elements in tree to be added, so return
        return;
    }

    auto newNewTree = std::make_unique<AVLTree<NvraRecord>>(comparator);

    NvraRecord oldRecord;
    NvraRecord newRecord;
    NvraRecord *oldRecordPointer;
    NvraRecord *newRecordPointer;

    AVLTreeEnumerator<NvraRecord> oldTreeEnum(oldTree.get(), AVLTreeOrder::inorder);
    AVLTreeEnumerator<NvraRecord> newTreeEnum(newTree.get(), AVLTreeOrder::inorder);


    while (oldTreeEnum.hasNext() && newTreeEnum.hasNext()) { // iterate until one (or both) of the trees is empty

        oldRecord = oldTreeEnum.peek(); // need the first object without deleting
        newRecord = newTreeEnum.peek();

        if (comparator.compare(oldRecord, newRecord) == -1) { // oldRecord has the smaller of the two values
            oldRecord = oldTreeEnum.next();
            oldRecordPointer = &oldRecord;
            if (newNewTree->insert(oldRecordPointer)) { // insert it in the tree
                records++; // increment records
            }
        } else { // the old record is greater than or equal to the new record
            newRecord = newTreeEnum.next();
            newRecordPointer = &newRecord;

            if (newNewTree->insert(newRecordPointer)) {
                records++;
            }
        }
    }

    while (oldTreeEnum.hasNext()) { // old tree was longer and still has some elements
        oldRecord = oldTreeEnum.next();
        oldRecordPointer = &oldRecord;
        if (newNewTree->insert(oldRecordPointer)) {
            records++;
        }
    }

    while (newTreeEnum.hasNext()) { // new tree still has some elements, insert them
        newRecord = newTreeEnum.next();
        newRecordPointer = &newRecord;

        if (!(newNewTree->replace(newRecordPointer))) { // attempt to insert in tree
            newNewTree->insert(newRecordPointer);
            records++;
        }
    }

    oldTree = std::move(newNewTree);
}

struct NvraParseError {
    enum class Kind {
        tooFewFields,
        tooManyFields,
        invalidNumber
    };

    Kind kind;
    std::size_t column = 0;
};

std::expected<NvraRecord, NvraParseError> parseNvraRecord(std::string_view line) {
    constexpr std::size_t fieldCount = MAX_NUMS + MAX_STRINGS;
    std::array<std::string_view, fieldCount> fields;
    std::size_t fieldStart = 0;

    for (std::size_t column = 0; column < fieldCount; ++column) {
        const auto delimiter = line.find(',', fieldStart);
        if (column + 1 < fieldCount) {
            if (delimiter == std::string_view::npos) {
                return std::unexpected(NvraParseError{NvraParseError::Kind::tooFewFields});
            }
            fields[column] = line.substr(fieldStart, delimiter - fieldStart);
            fieldStart = delimiter + 1;
        } else {
            if (delimiter != std::string_view::npos) {
                return std::unexpected(NvraParseError{NvraParseError::Kind::tooManyFields});
            }
            fields[column] = line.substr(fieldStart);
        }
    }

    NvraRecord record;
    for (std::size_t column = 0; column < fieldCount; ++column) {
        if (column == 3 || column == 11 || column == 12) {
            record.addString(std::string(fields[column]));
        } else {
            unsigned int value = 0;
            const auto [end, error] = std::from_chars(
                fields[column].data(),
                fields[column].data() + fields[column].size(),
                value);
            if (error != std::errc{} || end != fields[column].data() + fields[column].size()) {
                return std::unexpected(
                    NvraParseError{NvraParseError::Kind::invalidNumber, column + 1});
            }
            record.addNum(value);
        }
    }

    return record;
}

template <typename Handler>
void readNvraRecords(
        std::istream& input,
        long& attemptedRecords,
        long& validRecords,
        Handler handler) {
    std::string line;
    std::getline(input, line);

    long lineNumber = 0;
    while (std::getline(input, line)) {
        ++lineNumber;
        ++attemptedRecords;

        auto parsedRecord = parseNvraRecord(line);
        if (!parsedRecord) {
            std::cout << "Invalid data at line " << lineNumber << ": ";
            switch (parsedRecord.error().kind) {
            case NvraParseError::Kind::tooFewFields:
                std::cout << "too few fields.";
                break;
            case NvraParseError::Kind::tooManyFields:
                std::cout << "too many fields.";
                break;
            case NvraParseError::Kind::invalidNumber:
                std::cout << "invalid non-negative number in column "
                          << parsedRecord.error().column << ".";
                break;
            }
            std::cout << std::endl;
            continue;
        }

        ++validRecords;
        handler(*parsedRecord, lineNumber);
    }
}

// function to merge file elements
bool mergeLoop(
        std::unique_ptr<AVLTree<NvraRecord>>& oldTree,
        long& records,
        long& attemptedRecords,
        long& validRecords,
        bool& first,
        NvraComparator& comparator) {

    bool firstFile = first;
    bool dataRead = false; // has data been read in?
    std::string requestedFile;
    auto newTree = std::make_unique<AVLTree<NvraRecord>>(comparator);

    std::cout << "Enter data file name: ";

    while (std::getline(std::cin, requestedFile)) {

        if (requestedFile.empty()) { // file is empty, so exit loop
            break;
        }

        std::ifstream fileToRead(requestedFile);

        if (fileToRead.is_open()) {

            dataRead = true; // we have read some data, so set to true
            readNvraRecords(fileToRead, attemptedRecords, validRecords,
                [&](const NvraRecord& record, long lineNumber) {
                    bool isInserted;

                    if (firstFile) {
                        isInserted = oldTree->insert(&record);
                        if (isInserted) {
                            records++;
                        }
                    } else {
                        isInserted = newTree->insert(&record);
                    }

                    if (!isInserted) {
                        std::cout << "Duplicate record ID " << record.getNum(0)
                                  << " at line " << lineNumber << "." << std::endl;
                    }
                });
        } else { // file is not available
            std::cout << "File is not available." << std::endl;
            std::cout << "Enter data file name: ";
            continue; // to allow for a new file to be read if file not available
        }
        fileToRead.close();

        if (!firstFile) {
            mergeOldTreeNewTree(oldTree, std::move(newTree), records, comparator); // new data, so call merge trees method
        }

        return dataRead; // return whether data has been read
    }
    return dataRead; // return whether data has been read
}

void purgeOldTree(
        AVLTree<NvraRecord>& oldTree,
        std::unique_ptr<AVLTree<NvraRecord>> newTree,
        long& records) {

    if (oldTree.getSize() == 0 || newTree->getSize() == 0) { // if new tree is empty, nothing to purge
        return;
    }

    bool itemRemoved;
    NvraRecord recordToRemove;
    NvraRecord *recordPointer;
    AVLTreeEnumerator<NvraRecord> newTreeEnum(newTree.get(), AVLTreeOrder::inorder);

    // while the new tre has items, attempt to remove them from the old AVLTree
    while (newTreeEnum.hasNext()) {
        recordToRemove = newTreeEnum.next();
        recordPointer = &recordToRemove;

        itemRemoved = oldTree.remove(recordPointer); // check whether item was removed

        if (itemRemoved) { // if it was removed, decrement records variable
            records--;
        }
    }
}

#if 0
// Original purge parser retained for comparison with the shared parser.
//function purge data from tree
void purgeLoop(
        AVLTree<NvraRecord>* oldTree,
        long& records,
        long& attemptedRecords,
        long& validRecords) {

    NvraComparator compare(0);
    auto newTree = std::make_unique<AVLTree<NvraRecord>>(compare);
    std::string requestedFile;
    std::cout << "Enter data file name: ";

    while (std::getline(std::cin, requestedFile)) {

        if (requestedFile.empty()) {
            break; // file is empty, so exit loop
        }

        std::ifstream fileToRead(requestedFile);

        if (fileToRead.is_open()) {

            std::string textToRead; // the text to read
            std::getline(fileToRead, textToRead); // ignore first line

            int errorLine = 0; // line number for error printing

            firstLoop:while(std::getline(fileToRead, textToRead)) {
            int columnNumber = 0;
            errorLine++; // keeps track of error line
            std::stringstream lineStream(textToRead);
            NvraRecord nvraRecordKeeper; // stores the present nvraRecord
            int nvraRecordIntCount = 1; // counts number of ints read into an nvraRecord
            int nvraRecordStringCounter = 0; // counts number of strings read into an nvraRecord
            int stringToIntValue = 0; // int variable that holds number value of string variable
            unsigned int unsignedStringToInt = 0; // unsigned of above
            unsigned int recordIdHolder = 0; // variable to hold record ID value
            int recordIdSigned = 0;

            // separate the first number for duplicate checking
            std::getline(lineStream, textToRead, ',');
            recordIdSigned = std::stoi(textToRead); // save record ID for duplicate checking
            recordIdHolder = static_cast<unsigned int>(recordIdSigned);
            nvraRecordKeeper.addNum(recordIdHolder); // add the record ID to nums array
            attemptedRecords++;

            // split the file string by comma
            while (std::getline(lineStream, textToRead, ',')) {

                std::stringstream tokenStream(textToRead);

                // string column is chosen
                if (columnNumber == 2 || columnNumber == 10 || columnNumber == 11) {
                    nvraRecordKeeper.addString(textToRead);
                    nvraRecordStringCounter++; // increase string counter
                    columnNumber++; // increase column number
                } else { // number column chosen
                    tokenStream >> stringToIntValue;

                    if (stringToIntValue < 0) {
                        // negative number found, print error message
                        std::cout << "Invalid data at line " << errorLine << "." << std::endl;
                        goto firstLoop; // go back to loop condition
                    } else {
                        // add the number to nums[] array
                        unsignedStringToInt = static_cast<unsigned int>(stringToIntValue);
                        nvraRecordKeeper.addNum(unsignedStringToInt); // add the number to the nums[]
                        nvraRecordIntCount++; // increase nums counter
                        columnNumber++; // increase the column number
                    }
                }
            }
                validRecords++;
                NvraRecord *temp = &nvraRecordKeeper;
                newTree->insert(temp); // insert the item into a new tree
            }

        } else { // file is not available, print message
            std::cout << "File is not available." << std::endl;
            std::cout << "Enter data file name: ";
            continue;
        }

        fileToRead.close();
        purgeOldTree(oldTree, std::move(newTree), records); // purge the new records from the old tree
        return;
    }
}
#endif

void purgeLoop(
        AVLTree<NvraRecord>& oldTree,
        long& records,
        long& attemptedRecords,
        long& validRecords) {

    NvraComparator compare(0);
    auto newTree = std::make_unique<AVLTree<NvraRecord>>(compare);
    std::string requestedFile;
    std::cout << "Enter data file name: ";

    while (std::getline(std::cin, requestedFile)) {
        if (requestedFile.empty()) {
            break;
        }

        std::ifstream fileToRead(requestedFile);
        if (!fileToRead.is_open()) {
            std::cout << "File is not available." << std::endl;
            std::cout << "Enter data file name: ";
            continue;
        }

        readNvraRecords(fileToRead, attemptedRecords, validRecords,
            [&](const NvraRecord& record, long) {
                newTree->insert(&record);
            });

        purgeOldTree(oldTree, std::move(newTree), records);
        return;
    }
}

int main() {

    NvraComparator compare(0); // compare according to the record ID
    TemplatedArray<NvraRecord> oldNvraArray; // array to store data for searching and sorting
    auto avl = std::make_unique<AVLTree<NvraRecord>>(compare); // tree to hold the record IDs
    NvraHasher nhash; // hash

    long nvraRecordCounter = 0; // keeps track of the nvra records on memory
    long attemptedNvraRecords = 0; // records that the program tried to read
    long validNvraRecords = 0; // nvra records without negative values

    try {

        bool isFirst = true; // is this the first file read in?
        bool dataRead = mergeLoop(avl, nvraRecordCounter, attemptedNvraRecords, validNvraRecords, isFirst, compare);
        createArrayFromTree(*avl, oldNvraArray); // create an array from the tree

        auto tableHash = std::make_unique<HashTable<NvraRecord>>(compare, nhash, avl->getSize()); // hash table to hold IDs for searching records
        createHashTableFromTree(*avl, *tableHash); // create a hash table from the tree

        if (!dataRead) { // data has not been read in, so exit
            return 0;
        } else { // data has been read in, so enter option loop
            std::string answer;
            std::cout << "Enter (o)utput, (s)ort, (f)ind, (m)erge, (p)urge, (h)ash table, (pre)order, (in)order, (post)order, or (q)uit: ";
            std::getline(std::cin, answer);
            unsigned int columnSorted = 0; // keeps track of the column by which data is sorted

            while (answer != "q") {

                if (answer == "o") { // print array

                    outputLoop(oldNvraArray, nvraRecordCounter, attemptedNvraRecords, validNvraRecords);

                } else if (answer == "s") { // sort by a column value

                    columnSorted = sortLoop(oldNvraArray); // call sort method

                } else if (answer == "f") { // find by a column value

                    findLoop(oldNvraArray, *tableHash, columnSorted); // call find method

                } else if (answer == "m") { // merge old file with new file

                    bool isSecond = false; // first file has already been read
                    mergeLoop(avl, nvraRecordCounter, attemptedNvraRecords, validNvraRecords, isSecond, compare); // call merge method
                    createArrayFromTree(*avl, oldNvraArray);
                    createHashTableFromTree(*avl, *tableHash);

                } else if (answer == "p") { // purge a file from the data

                    purgeLoop(*avl, nvraRecordCounter, attemptedNvraRecords, validNvraRecords); // call purge method
                    createArrayFromTree(*avl, oldNvraArray);
                    createHashTableFromTree(*avl, *tableHash);

                } else if (answer == "h") { // print hash table

                    tableHash->displayHashTable(nvraRecordCounter, attemptedNvraRecords, validNvraRecords); // call hash table print method

                } else if(answer == "pre") { // conduct preorder traversal of tree and print it

                    AVLTreeEnumerator<NvraRecord> treeEnum (avl.get(), AVLTreeOrder::preorder);
                    printAVL(treeEnum, nvraRecordCounter, attemptedNvraRecords, validNvraRecords);

                } else if (answer == "in") { // conduct inorder traversal of the tree and print it

                    AVLTreeEnumerator<NvraRecord> treeEnum (avl.get(), AVLTreeOrder::inorder);
                    printAVL(treeEnum, nvraRecordCounter, attemptedNvraRecords, validNvraRecords);

                } else if (answer == "post") { // conduct a postorder traversal of the tree

                    AVLTreeEnumerator<NvraRecord> treeEnum (avl.get(), AVLTreeOrder::postorder);
                    printAVL(treeEnum, nvraRecordCounter, attemptedNvraRecords, validNvraRecords);

                } else {

                    // option not valid, repeat prompt
                    std::cout << "Please enter a valid option or type 'q' to quit " << std::endl;

                }
                // repeat prompt
                std::cout << "Enter (o)utput, (s)ort, (f)ind, (m)erge, (p)urge, (h)ash table, (pre)order, (in)order, (post)order, or (q)uit: ";
                std::cin.clear(); // clear cin
                std::getline(std::cin, answer); // get the next file
            }

            // print friendly message
            std::cout << "Thanks for using VoteR." << std::endl;
        }
    } catch (const ExceptionIndexOutOfRange&) { // index is out of range, throw an exception
        std::cout << "Index out of bounds! Enter a valid index" << std::endl;
        return EXIT_FAILURE;
    } catch (const ExceptionMemoryNotAvailable&) { // memory unavailable
        std::cout << "Memory not available!" << std::endl;
        return EXIT_FAILURE;
    } catch (const ExceptionEnumerationBeyondEnd&) { // enumeration extended beyond the end of the LinkedList
        std::cout << "Enumeration beyond end!" << std::endl;
        return EXIT_FAILURE;
    } catch (const ExceptionLinkedListAccess&) { // accessing element beyond end of LinkedList
        std::cout << "Exception linkedlist access!" << std::endl;
        return EXIT_FAILURE;
    } catch (const ExceptionHashTableAccess&) { // accessing element not in hash table
        std::cout << "Exception Hash Table access!" << std::endl;
        return EXIT_FAILURE;
    } catch (const ExceptionAVLTreeAccess&) { // attempting to access element beyond tree size
        std::cout << "Exception AVLTree access!" << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}