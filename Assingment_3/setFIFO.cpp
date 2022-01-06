#include "setFIFO.h"

// Creates a set with a fixed set size, block size, and evict type
SetFIFO::SetFIFO(uint32_t block_num) : Set(block_num) {}

// takes an address and returns 1 if a memory in cache, 0 otherwise
int SetFIFO::get(uint32_t tag) {
    return this->in_set(tag);
}

// takes an address and stores it into set
// returns 1 if dirty block gets overwritten
// returns 0 otherwise (block already in set, block clean, block empty)
int SetFIFO::put(uint32_t tag, int dirty) {
    if(this->in_set(tag)) {
        this->smear(tag);               // storing block already in cache makes it dirty
    }

    return this->add(tag, dirty);       // add to set and return 1 if dirty block evicted 
}

