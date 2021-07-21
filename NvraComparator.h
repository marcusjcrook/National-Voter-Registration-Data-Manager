#ifndef NVRA_COMPARATOR_H
#define NVRA_COMPARATOR_H

#include "Comparator.h"
#include "NvraRecord.h"

class NvraComparator : public Comparator<NvraRecord> {
private:
    unsigned int column = 0;
public:
    explicit NvraComparator(unsigned int column);
    int compare(const NvraRecord& item1, const NvraRecord& item2) const override;
};

#endif
