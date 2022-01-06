#include "setLRU.h"

// Creates a set with a fixed set size, block size, and evict type
SetLRU::SetLRU(uint32_t block_num) : Set(block_num) {}

// takes an address and returns 1 if a memory in cache, 0 otherwise
int SetLRU::get(uint32_t tag) {
    if(this->in_set(tag)) {
        int dirty = this->erase(tag);   // take it out from where ever it is
        this->add(tag, dirty);          // put it at the front of the list
        return 1;
    }

    return 0;
}


// takes an address and stores it into set
// returns 1 if dirty block gets overwritten
// returns 0 otherwise (block already in set, block clean, block empty)
// ---------------------------------------------------------------------------------------
// NOTE: The only time when a user would try to put() something into the cache that's already
// in cache is when they are storing (if they were just trying to load it, then either they
// would get() that element if it already was in cache, or if it wasn't, then they could retrieve
// if from memory and then put() it in the cache, in which case it wouldn't be in cache). Hence,
// when we re add the element, it's safe to assume that element is now dirty. Of course, this isn't
// true is we are using write-through. However, it is easier to just store this information and
// ignore it if we don't need it then it is to remember a special case and potentially save memory.
int SetLRU::put(uint32_t tag, int dirty) {
    if(this->in_set(tag)) {
        this->erase(tag);               // take it out from where ever it is
        this->add(tag, 1);              // put it at the front of the list, block has to be dirty
        return 0;                       // function essentially did nothing, so return 0
    }
    
    return this->add(tag, dirty);       // add to set and return 1 if dirty block evicted 
}
