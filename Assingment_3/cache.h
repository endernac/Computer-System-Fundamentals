#ifndef CACHE_H
#define CACHE_H

#include <stdint.h>
#include <stdlib.h>
#include "set.h"
#include "setLRU.h"
#include "setFIFO.h"
#include "adtls.h"

class Cache {
private:
        
    // store the log of the number of sets and block size   
    uint32_t set_sft;
    uint32_t block_sft;

    // create an array of set pointers
    Set **sets;

public:   

    // Creates a cache with a fixed number of sets, set size, block size, write alloc, and evict type
    Cache(uint32_t set_num, uint32_t block_num, uint32_t block_size, int evict);   
    
    // destructor
    ~Cache();       

    // takes an address and returns 1 if a memory in cache, 0 otherwise
    int get(uint32_t addr);

    // takes an address and stores it into cache, returning address that was overwritten (if any), 
    // or 0 if memory was previously free
    int put(uint32_t addr, int dirty);
};

#endif
