cmake_minimum_required(VERSION 3.12)
project(ProjectOne)

set(CMAKE_CXX_STANDARD 14)

add_executable(ProjectOne VoteR4.cpp Comparator.h Exceptions.h NvraComparator.cpp Search.h Sorter.h NvraComparator.h TemplatedArray.h NvraRecord.cpp NvraRecord.h Enumerator.h OULink.h OULinkedList.h OULinkedListEnumerator.h NvraHasher.h HashTableEnumerator.h Hasher.h HashTable.h NvraHasher.cpp AVLTree.h AVLTreeEnumerator.h AVLTreeOrder.h)