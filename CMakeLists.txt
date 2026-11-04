cmake_minimum_required(VERSION 3.20)
project(ProjectOne LANGUAGES CXX)

add_executable(ProjectOne VoteR4.cpp Comparator.h Exceptions.h NvraComparator.cpp Search.h Sorter.h NvraComparator.h TemplatedArray.h NvraRecord.cpp NvraRecord.h Enumerator.h OULink.h OULinkedList.h OULinkedListEnumerator.h NvraHasher.h HashTableEnumerator.h Hasher.h HashTable.h NvraHasher.cpp AVLTree.h AVLTreeEnumerator.h AVLTreeOrder.h)
target_compile_features(ProjectOne PRIVATE cxx_std_23)
set_target_properties(ProjectOne PROPERTIES CXX_EXTENSIONS OFF)
