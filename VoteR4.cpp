#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <stdio.h>
#include <cstdio>
#include <cctype>
#include <regex>
#include <cmath>
#include <fstream>
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

using namespace std;

// method to print search results for binary search
void printSearchResults(long long index, TemplatedArray<NvraRecord>& array, NvraComparator& comparator) {

    // method to print the records
    long long recordsFound = 0;
    for (int i = 0; i < (signed) array.getSize(); ++i) {
        if (((index + i) < (signed) array.getSize()) && comparator.compare(array[index], array[index + i]) == 0) {
            cout << array[index + i] << endl;
            recordsFound++;
        }
    }
    cout << "NVRA records found: " << recordsFound << "." << endl;
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
            cout << array[index] << endl;
        }
    }
    cout << "NVRA records found: " << recordsFound << "." << endl;
}

// method to print AVLTree by specified order
void printAVL(AVLTreeEnumerator<NvraRecord> treeEnum,
        const long& records, const long& attemptedRecords, const long& validRecords) {

    std::string outputFile;
    std::cout << "Enter output file name: "; // prompt the user for a file name
    getline(std::cin, outputFile);


    if (outputFile.empty()) { // no file entered, send to cout
        while (treeEnum.hasNext()) {
            cout << treeEnum.next() << endl;
        }

        cout << "Data lines read: " << attemptedRecords << "; Valid NVRA records read: " << validRecords <<
             "; NVRA records in memory: " << records << std::endl;
    } else { // valid file chosen, so write data to it
        std::ofstream fileToWrite;
        fileToWrite.open(outputFile);

        if (fileToWrite.is_open()) {

            while (treeEnum.hasNext()) {
                fileToWrite << treeEnum.next() << endl;
            }

            fileToWrite << "Data lines read: " << attemptedRecords <<
                        "; Valid NVRA records read: " << validRecords << "; NVRA records in memory: " <<
                        records << std::endl;
            fileToWrite.close();
        }
    }

}

// print the data from the array
void outputLoop(TemplatedArray<NvraRecord>& array, const long& records, const long& attemptedRecords, const long& validRecords) {

    string outputFile;
    cout << "Enter output file name: ";
    getline(cin, outputFile);

    // no file name entered, send to cout
    if (outputFile.empty()) {
        for (unsigned int recordIndex = 0; recordIndex < array.getSize(); ++recordIndex) {
            cout << array.get(recordIndex) << endl;
        }
    } else {
        ofstream fileToWrite;
        fileToWrite.open(outputFile);

        // wasn't sure, based on instructions, whether to print to console or only print to file so I did both the be safe
        if (fileToWrite.is_open()) {
            for (unsigned long index = 0; index < array.getSize(); ++index) {
                fileToWrite << array.get(index) << endl; // write the data line to the file
                cout << array.get(index) << endl; // print the line to the console
            }

            // print out information about data
            fileToWrite << "Data lines read: " << attemptedRecords <<
                        "; Valid NVRA records read: " << validRecords << "; NVRA records in memory: " <<
                        records << endl;
            fileToWrite.close();
        } else {
            cout << "File is not available." << endl;
        }
    }

    // information about the records read in
    cout << "Data lines read: " << attemptedRecords <<
         "; Valid NVRA records read: " << validRecords << "; NVRA records in memory: " <<
         records << endl;
}

// method for sorting portion of data manipulation loop
unsigned int sortLoop(TemplatedArray<NvraRecord>& oldArray) {

    unsigned int result = 0; // returns column by which the data is sorted
    unsigned int sortField = 0;
    string sortFieldString; // field by which to search
    cout << "Enter sort field (0-23): " << endl;
    getline(cin, sortFieldString);

    // check whether the input is valid
    regex stringCheck ("^-?\\d+");
    if (regex_match(sortFieldString,stringCheck)) {

        sortField = (unsigned) stoi(sortFieldString); // convert to unsigned int
        if (sortField <= 23) {
            NvraComparator n(sortField);
            Sorter<NvraRecord>::sort(oldArray, n); // sort the data by column number
        }
        result = sortField;
    }
    return result;
}

// method for finding portion of data manipulation loop
void findLoop(TemplatedArray<NvraRecord>& oldArray, HashTable<NvraRecord>* hashTable, unsigned int sortedColumn) {

    unsigned int findField = 0; // column to search
    string findFieldString; // string to convert to column to search
    string numberToSearch; // the non-negative string value to search (after converted to number below)
    int searchNumber = 0; // int conversion of above string
    unsigned int unsignedSearchNumber = 0; // needed the number to be unsigned
    long long result = 0; // resulting index or index location from binary search
    string stringToSearch; // the string to search using binary search
    NvraRecord searchRecord; // record to search for
    cout << "Enter search field (0-23): " << endl;
    getline(cin, findFieldString);

    // used regex again to check if number
    regex stringCheck ("^-?\\d+");
    if (regex_match (findFieldString,stringCheck)) {

        findField = (unsigned) stoi(findFieldString);
        if (findField <= 23) {
            NvraComparator fieldCompare(findField);
            Sorter<NvraRecord>::sort(oldArray, fieldCompare);
        }

        if (findField <= 23) {
            unsigned int searchIndex = 0;

            // set search index to appropriate offset value
            if (findField > 3 && findField < 11) {
                searchIndex = findField - 1;
            } else if (findField > 12) {
                searchIndex = findField - 3;
            } else {
                searchIndex = findField;
            }

            // if the column is a string column
            if (findField == 3 || findField == 11 || findField == 12) {
                cout << "Enter exact text on which to search: " << endl;
                getline(cin, stringToSearch);

                if (findField == 3) {
                    searchRecord.setString(stringToSearch, 0);
                } else if (findField == 11) {
                    searchRecord.setString(stringToSearch, 1);
                } else if (findField == 12) {
                    searchRecord.setString(stringToSearch, 2);
                }

                NvraComparator stringFieldCompare(findField);
                result = binarySearch(searchRecord, oldArray, stringFieldCompare);

                if (result >= 0) { // item found, so print results
                    printSearchResults(result, oldArray, stringFieldCompare);
                }
                else {
                    cout << "NVRA records found: " << 0 << endl;
                }
            } else if (findField == 0) { // we are searching for record ID, so use hash
                cout << "Enter non-negative field value: " << endl;
                getline(cin, numberToSearch);
                regex numCheck ("^-?\\d+"); // check for number validity
                if (regex_match (numberToSearch,numCheck)) {
                    searchNumber = stoi(numberToSearch);
                    unsignedSearchNumber = (unsigned) searchNumber;
                    searchRecord.setNum(unsignedSearchNumber, searchIndex);
                    NvraRecord* searchRecordPointer = &searchRecord; // create record object with record ID

                    NvraRecord hashResult = hashTable->find(searchRecordPointer); // find the item in the hash table
                    cout << hashResult << endl;
                    cout << "NVRA records found: " << 1 << "." << endl; // only one record should be found
                }
            } else { // if the column us a number column
                cout << "Enter non-negative field value: " << endl;
                getline(cin, numberToSearch);
                regex numCheck ("^-?\\d+");
                if (regex_match (numberToSearch,numCheck)) {
                    searchNumber = stoi(numberToSearch);
                    unsignedSearchNumber = (unsigned) searchNumber;
                    if (searchNumber >= 0) {

                        // set the searchRecord value at the index
                        searchRecord.setNum(unsignedSearchNumber, searchIndex);

                        NvraComparator numsFieldCompare(findField);

                        // array is sorted by this particular column, so use binary search
                        if (sortedColumn == findField) {
                            result = binarySearch(searchRecord, oldArray, numsFieldCompare);
                            if (result >= 0) {
                                printSearchResults(result, oldArray, numsFieldCompare);
                            } else {
                                cout << "NVRA records found: " << 0 << endl;
                            }
                        }
                        else { // not sorted by the column, use linear search
                            linearSearch(oldArray, searchRecord, numsFieldCompare); // not sorted, call limear search
                        }
                    }
                }
            }
        }
    }
}

// function to create array from AVLTree
void createArrayFromTree(AVLTree<NvraRecord>* oldTree, TemplatedArray<NvraRecord>& array) {

    array.remove(); // delete the contents of the array

    if (oldTree->getSize() == 0) { // AVLTree is empty so simply return
        return;
    }

    NvraRecord recordToAdd;
    NvraRecord *recordPointer;

    AVLTreeEnumerator<NvraRecord> treeEnum(oldTree, AVLTreeOrder::inorder);

    // conduct an in order traversal of the tree and at elements to array
    while (treeEnum.hasNext()) {
        recordToAdd = treeEnum.next();
        recordPointer = &recordToAdd;
        array.add(recordPointer);
    }

}

// function to create hashTable from AVLTree
void createHashTableFromTree(AVLTree<NvraRecord>* oldTree, HashTable<NvraRecord>* hashTable) {
    hashTable->recreateTable(oldTree->getSize(), hashTable->getBaseCapacity()); // call method to clear hash table

    if (oldTree->getSize() == 0) { // tree is empty, so return
        return;
    }

    NvraRecord recordToAdd;
    NvraRecord *recordPointer;

    AVLTreeEnumerator<NvraRecord> treeEnum(oldTree, AVLTreeOrder::inorder);

    // conduct an in order traversal of the tree and at elements to hashtable
    while (treeEnum.hasNext()) {
        recordToAdd = treeEnum.next();
        recordPointer = &recordToAdd;
        hashTable->insert(recordPointer);
    }
}

// function to combine old tree and new tree
void mergeOldTreeNewTree(AVLTree<NvraRecord>*& oldTree, AVLTree<NvraRecord>* newTree, long& records) {

    records = 0;
    if (newTree->getSize() == 0) { // no elements in tree to be added, so return
        return;
    }

    NvraComparator compare(0);
    NvraComparator* comparePointer = &compare;
    auto* newNewTree = new AVLTree<NvraRecord>(comparePointer);

    NvraRecord oldRecord;
    NvraRecord newRecord;
    NvraRecord *oldRecordPointer;
    NvraRecord *newRecordPointer;

    AVLTreeEnumerator<NvraRecord> oldTreeEnum(oldTree, AVLTreeOrder::inorder);
    AVLTreeEnumerator<NvraRecord> newTreeEnum(newTree, AVLTreeOrder::inorder);


    while (oldTreeEnum.hasNext() && newTreeEnum.hasNext()) { // iterate until one (or both) of the trees is empty

        oldRecord = oldTreeEnum.peek(); // need the first object without deleting
        newRecord = newTreeEnum.peek();

        if (compare.compare(oldRecord, newRecord) == -1) { // oldRecord has the smaller of the two values
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

    delete newTree; // delete the new tree
    delete oldTree; // delete the old tree
    oldTree = new AVLTree<NvraRecord>(comparePointer);
    *oldTree = *newNewTree; // redirect data
}

// function to merge file elements
bool mergeLoop(AVLTree<NvraRecord>*& oldTree, long& records, long& attemptedRecords, long& validRecords, bool& first) {

    bool firstFile = first;
    bool dataRead = false; // has data been read in?
    string requestedFile;
    NvraComparator compare(0);
    NvraComparator* comparePointer = &compare;
    auto* newTree = new AVLTree<NvraRecord>(comparePointer);

    cout << "Enter data file name: ";

    while (getline(cin, requestedFile)) {

        if (requestedFile.empty()) { // file is empty, so exit loop
            break;
        }

        ifstream fileToRead(requestedFile);

        if (fileToRead.is_open()) {

            dataRead = true; // we have read some data, so set to true
            //bool duplicateFound = false;

            string textToRead; // the text to read
            getline(fileToRead, textToRead); // ignore first line

            int errorLine = 0; // line number for error printing

            firstLoop:while(getline(fileToRead, textToRead)) {
                int columnNumber = 0;
                errorLine++; // keeps track of error line
                stringstream lineStream(textToRead);
                NvraRecord nvraRecordKeeper; // stores the present nvraRecord
                int nvraRecordIntCount = 1; // counts number of ints read into an nvraRecord
                int nvraRecordStringCounter = 0; // counts number of strings read into an nvraRecord
                int stringToIntValue = 0; // int variable that holds number value of string variable
                unsigned int unsignedStringToInt = 0; // unsigned of above
                unsigned int recordIdHolder = 0; // variable to hold record ID value
                int recordIdSigned = 0;

                getline(lineStream, textToRead, ','); // split by comma
                recordIdSigned = stoi(textToRead); // save record ID for duplicate checking
                recordIdHolder = (unsigned) recordIdSigned;
                nvraRecordKeeper.addNum(recordIdHolder); // add the record ID to nums array
                attemptedRecords++;

                // split the file string
                while (getline(lineStream, textToRead, ',')) {

                    stringstream tokenStream(textToRead);

                    // originally used regex here, but changed to work with zyLabs. Just used column number instead
                    if (columnNumber == 2 || columnNumber == 10 || columnNumber == 11) {
                        nvraRecordKeeper.addString(textToRead);
                        nvraRecordStringCounter++; // increase string counter
                        columnNumber++; // increase column number
                    } else {
                        tokenStream >> stringToIntValue;

                        if (stringToIntValue < 0) {
                            // negative number found, print error message!
                            cout << "Invalid data at line " << errorLine << "." << endl;
                            goto firstLoop; // the dreaded goto statement to go back to beginning of loop
                        } else {
                            // add the number to nums[] array
                            unsignedStringToInt = (unsigned) stringToIntValue;
                            nvraRecordKeeper.addNum(unsignedStringToInt); // add the number to the nums[]
                            nvraRecordIntCount++; // increase nums counter
                            columnNumber++; // increase the column number
                        }
                    }
                }
                validRecords++;

                NvraRecord *temp = &nvraRecordKeeper;

                bool isInserted;

                // if this is the first file, simply add the elements to the old AVLTree
                if (firstFile) {
                    isInserted = oldTree->insert(temp);

                    if (isInserted) {
                        records++;
                    }
                } else { // if we are merging new data, store in new AVLTree
                    isInserted = newTree->insert(temp);
                }

                if (!isInserted) {
                    // duplicate found, print error message
                    cout << "Duplicate record ID " << recordIdHolder << " at line " << errorLine << "." << endl;
                    //records--; // reduce record by one since a duplicate was found
                }
            }
        } else { // file is not available
            cout << "File is not available." << endl;
            cout << "Enter data file name: ";
            continue; // to allow for a new file to be read if file not available
        }
        fileToRead.close();

        if (!firstFile) {
            mergeOldTreeNewTree(oldTree, newTree, records); // new data, so call merge trees method
        }

        return dataRead; // return whether data has been read
    }
    return dataRead; // return whether data has been read
}

void purgeOldTree(AVLTree<NvraRecord>*& oldTree, AVLTree<NvraRecord>* newTree, long& records) {

    if (oldTree->getSize() == 0 || newTree->getSize() == 0) { // if new tree is empty, nothing to purge
        return;
    }

    bool itemRemoved;
    NvraRecord recordToRemove;
    NvraRecord *recordPointer;
    AVLTreeEnumerator<NvraRecord> newTreeEnum(newTree, AVLTreeOrder::inorder);

    // while the new tre has items, attempt to remove them from the old AVLTree
    while (newTreeEnum.hasNext()) {
        recordToRemove = newTreeEnum.next();
        recordPointer = &recordToRemove;

        itemRemoved = oldTree->remove(recordPointer); // check whether item was removed

        if (itemRemoved) { // if it was removed, decrement records variable
            records--;
        }
    }
    delete newTree; // delete the new tree
}

//function purge data from tree
void purgeLoop(AVLTree<NvraRecord>* oldTree, long& records, long& attemptedRecords, long& validRecords) {

    NvraComparator compare(0);
    NvraComparator* comparePointer = &compare;
    auto* newTree = new AVLTree<NvraRecord>(comparePointer);
    string requestedFile;
    cout << "Enter data file name: ";

    while (getline(cin, requestedFile)) {

        if (requestedFile.empty()) {
            break; // file is empty, so exit loop
        }

        ifstream fileToRead(requestedFile);

        if (fileToRead.is_open()) {

            string textToRead; // the text to read
            getline(fileToRead, textToRead); // ignore first line

            int errorLine = 0; // line number for error printing

            firstLoop:while(getline(fileToRead, textToRead)) {
            int columnNumber = 0;
            errorLine++; // keeps track of error line
            stringstream lineStream(textToRead);
            NvraRecord nvraRecordKeeper; // stores the present nvraRecord
            int nvraRecordIntCount = 1; // counts number of ints read into an nvraRecord
            int nvraRecordStringCounter = 0; // counts number of strings read into an nvraRecord
            int stringToIntValue = 0; // int variable that holds number value of string variable
            unsigned int unsignedStringToInt = 0; // unsigned of above
            unsigned int recordIdHolder = 0; // variable to hold record ID value
            int recordIdSigned = 0;

            // separate the first number for duplicate checking
            getline(lineStream, textToRead, ',');
            recordIdSigned = stoi(textToRead); // save record ID for duplicate checking
            recordIdHolder = (unsigned) recordIdSigned;
            nvraRecordKeeper.addNum(recordIdHolder); // add the record ID to nums array
            attemptedRecords++;

            // split the file string by comma
            while (getline(lineStream, textToRead, ',')) {

                stringstream tokenStream(textToRead);

                // string column is chosen
                if (columnNumber == 2 || columnNumber == 10 || columnNumber == 11) {
                    nvraRecordKeeper.addString(textToRead);
                    nvraRecordStringCounter++; // increase string counter
                    columnNumber++; // increase column number
                } else { // number column chosen
                    tokenStream >> stringToIntValue;

                    if (stringToIntValue < 0) {
                        // negative number found, print error message
                        cout << "Invalid data at line " << errorLine << "." << endl;
                        goto firstLoop; // go back to loop condition
                    } else {
                        // add the number to nums[] array
                        unsignedStringToInt = (unsigned) stringToIntValue;
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
            cout << "File is not available." << endl;
            cout << "Enter data file name: ";
            continue;
        }

        fileToRead.close();
        purgeOldTree(oldTree, newTree, records); // purge the new records from the old tree
        return;
    }
}

int main() {

    NvraComparator compare(0); // compare according to the record ID
    NvraComparator* comparePointer = &compare; // pointer to comparator
    TemplatedArray<NvraRecord> oldNvraArray; // array to store data for searching and sorting
    auto* avl = new AVLTree<NvraRecord>(comparePointer); // tree to hold the record IDs
    NvraHasher nhash; // hash
    NvraHasher* hashPointer = &nhash;// pointer to hash

    long nvraRecordCounter = 0; // keeps track of the nvra records on memory
    long attemptedNvraRecords = 0; // records that the program tried to read
    long validNvraRecords = 0; // nvra records without negative values

    try {

        bool isFirst = true; // is this the first file read in?
        bool dataRead = mergeLoop(avl, nvraRecordCounter, attemptedNvraRecords, validNvraRecords, isFirst);
        createArrayFromTree(avl, oldNvraArray); // create an array from the tree

        auto* tableHash = new HashTable<NvraRecord>(comparePointer, hashPointer, avl->getSize()); // hash table to hold IDs for searching records
        createHashTableFromTree(avl, tableHash); // create a hash table from the tree

        if (!dataRead) { // data has not been read in, so exit
            return 0;
        } else { // data has been read in, so enter option loop
            string answer;
            cout << "Enter (o)utput, (s)ort, (f)ind, (m)erge, (p)urge, (h)ash table, (pre)order, (in)order, (post)order, or (q)uit: ";
            getline(cin, answer);
            unsigned int columnSorted = 0; // keeps track of the column by which data is sorted

            while (answer != "q") {

                if (answer == "o") { // print array

                    outputLoop(oldNvraArray, nvraRecordCounter, attemptedNvraRecords, validNvraRecords);

                } else if (answer == "s") { // sort by a column value

                    columnSorted = sortLoop(oldNvraArray); // call sort method

                } else if (answer == "f") { // find by a column value

                    findLoop(oldNvraArray, tableHash, columnSorted); // call find method

                } else if (answer == "m") { // merge old file with new file

                    bool isSecond = false; // first file has already been read
                    mergeLoop(avl, nvraRecordCounter, attemptedNvraRecords, validNvraRecords, isSecond); // call merge method
                    createArrayFromTree(avl, oldNvraArray);
                    createHashTableFromTree(avl, tableHash);

                } else if (answer == "p") { // purge a file from the data

                    purgeLoop(avl, nvraRecordCounter, attemptedNvraRecords, validNvraRecords); // call purge method
                    createArrayFromTree(avl, oldNvraArray);
                    createHashTableFromTree(avl, tableHash);

                } else if (answer == "h") { // print hash table

                    tableHash->displayHashTable(nvraRecordCounter, attemptedNvraRecords, validNvraRecords); // call hash table print method

                } else if(answer == "pre") { // conduct preorder traversal of tree and print it

                    AVLTreeEnumerator<NvraRecord> treeEnum (avl, AVLTreeOrder::preorder);
                    printAVL(treeEnum, nvraRecordCounter, attemptedNvraRecords, validNvraRecords);

                } else if (answer == "in") { // conduct inorder traversal of the tree and print it

                    AVLTreeEnumerator<NvraRecord> treeEnum (avl, AVLTreeOrder::inorder);
                    printAVL(treeEnum, nvraRecordCounter, attemptedNvraRecords, validNvraRecords);

                } else if (answer == "post") { // conduct a postorder traversal of the tree

                    AVLTreeEnumerator<NvraRecord> treeEnum (avl, AVLTreeOrder::postorder);
                    printAVL(treeEnum, nvraRecordCounter, attemptedNvraRecords, validNvraRecords);

                } else {

                    // option not valid, repeat prompt
                    cout << "Please enter a valid option or type 'q' to quit " << endl;

                }
                // repeat prompt
                cout << "Enter (o)utput, (s)ort, (f)ind, (m)erge, (p)urge, (h)ash table, (pre)order, (in)order, (post)order, or (q)uit: ";
                cin.clear(); // clear cin
                getline(cin, answer); // get the next file
            }

            // print friendly message
            cout << "Thanks for using VoteR." << endl;
        }
    } catch (const ExceptionIndexOutOfRange* e ) { // index is out of range,throw an exception
        cout << "Index out of bounds! Enter a valid index" << endl;
        delete e;
        return EXIT_FAILURE;
    } catch (const ExceptionMemoryNotAvailable* e) { // memory unavailable
        cout << "Memory not available!" << endl;
        delete e;
        return EXIT_FAILURE;
    } catch (const ExceptionEnumerationBeyondEnd* e) { // enumeration extended beyond the end of the LinkedList
        cout << "Enumeration beyond end!" << endl;
        delete e;
        return EXIT_FAILURE;
    } catch (const ExceptionLinkedListAccess* e) { // accessing element beyond end of LinkedList
        cout << "Exception linkedlist access!" << endl;
        delete e;
        return EXIT_FAILURE;
    } catch (const ExceptionHashTableAccess* e) { // accessing element not in hash table
        cout << "Exception Hash Table access!" << endl;
        delete e;
        return EXIT_FAILURE;
    } catch (const ExceptionAVLTreeAccess* e) { // attempting to access element beyond tree size
        cout << "Exception AVLTree access!" << endl;
        delete e;
        return EXIT_FAILURE;
    }
    return 0;
}