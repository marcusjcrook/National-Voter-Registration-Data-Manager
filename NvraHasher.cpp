#include "NvraRecord.h"
#include "NvraHasher.h"

// implement a hash function appropriate for NvraRecords
unsigned long NvraHasher::hash(const NvraRecord& item) const {
    return item.getNum(0); // return the record ID
}

