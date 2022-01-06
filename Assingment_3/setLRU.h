#ifndef SET_LRU_H
#define SET_LRU_H

#include "set.h"

class SetLRU : public Set {
    
public:
    // Creates a set with a fixed set size, block size, and evict type
    SetLRU(uint32_t block_num);

    // takes an address and returns 1 if a memory in cache, 0 otherwise
    int get(uint32_t tag) override;

    // takes an address and stores it into set
    // returns 1 if dirty block gets overwritten
    // returns 0 otherwise (block already in set, block clean, block empty)
    int put(uint32_t tag, int dirty) override;

};

#endif
