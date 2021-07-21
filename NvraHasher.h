#ifndef NVRA_HASHER_H
#define NVRA_HASHER_H

#include "Hasher.h"
#include "NvraRecord.h"

class NvraHasher : public Hasher<NvraRecord> {
public:
    unsigned long hash(const NvraRecord& item) const;
};

#endif // !NVRA_HASHER_H
