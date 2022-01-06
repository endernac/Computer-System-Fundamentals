#include "cache.h"


// Creates a cache with a fixed number of sets, set size, block size, write alloc, and evict type
Cache::Cache(uint32_t set_num, uint32_t block_num, uint32_t block_size, int evict) {
    this->set_sft = adtls::get_sft(set_num);
    this->block_sft = adtls::get_sft(block_size);
    this->sets = (Set **) malloc(set_num * sizeof(Set *));

    if(evict) {
        for(uint32_t s = 0; s < set_num; s++) {
            sets[s] = new SetLRU(block_num);
        }
    } else {
        for(uint32_t s = 0; s < set_num; s++) {
            sets[s] = new SetFIFO(block_num);
        }
    }

    // block_num and evict will be used in Set.cpp
}

Cache::~Cache() {
    for(uint32_t s = 0; s < (uint32_t) (1 << set_sft); s++) {
        delete sets[s];
    }

    free(sets);
}


// takes an address and returns 1 if a memory in cache, 0 otherwise
int Cache::get(uint32_t addr) {
    uint32_t tag = adtls::get_tag(addr, block_sft, set_sft);
    uint32_t set = adtls::get_set(addr, block_sft, set_sft);

    return sets[set]->get(tag) == 1;
}


// takes an address and stores it into cache, returning address that was overwritten (if any), 
// or 0 if memory was previously free
int Cache::put(uint32_t addr, int dirty) {
    uint32_t tag = adtls::get_tag(addr, block_sft, set_sft);
    uint32_t set = adtls::get_set(addr, block_sft, set_sft);

    return sets[set]->put(tag, dirty);
}
