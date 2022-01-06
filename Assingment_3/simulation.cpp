#include "simulation.h"

void Simulation::store(uint32_t addr) {
    int in_cache = cache->get(addr);
    // even if the addresss is in cache, we need to update position and dirtiness
    int dirty = 1; 
    if(write_alloc) {
        dirty = cache->put(addr, 1);
    }

    total_stores++;

    if(in_cache) {                  // cache hit
        store_hits++;

        if(write_thru) {            // addr-->memory (+ addr-->cache)
            total_cycles += 100;
        } else {                    // addr-->cache
            total_cycles += 1;
        }

    } else {                        // cache miss
        store_misses++;

        if(write_alloc) {           // memory-->cache
            total_cycles += 100 * block_words;               

            if(write_thru) {        // addr-->memory (+ addr-->cache)
                total_cycles += 100;
            } else {                // addr-->cache + (if dirty block evicted) cache-->memory
                total_cycles += 100 * block_words * dirty + 1;
            }

        } else {                    // addr-->memory
            total_cycles += 100;
        }
    }
}


// write setting do not affect loads
void Simulation::load(uint32_t addr) {
    int in_cache = cache->get(addr);
    total_loads++;

    if(in_cache) {                  // cache hit (-->cache)
        load_hits++;
        total_cycles += 1;
    } else {                        // cache miss (-->cache + memory-->cache)
        cache->put(addr, 0);
        load_misses++;
        total_cycles += 100 * block_words + 1;
    }
}



// creates a simulation based on given parameters
Simulation::Simulation(uint32_t sn, uint32_t bn, uint32_t bs, int wa, int wt, int ev) 
    : write_alloc(wa), write_thru(wt), block_words(bs / 4) {
        
    total_loads = 0;           
    load_hits = 0;
    load_misses = 0;

    total_stores = 0;
    store_hits = 0;
    store_misses = 0;

    total_cycles = 0;
    cache = new Cache(sn, bn, bs, ev);    
}

// delete cache object
Simulation::~Simulation() {
    delete cache;
}


// runs simulation on input
void Simulation::run() {
    char instruct;
    uint32_t addr;
    int x;

    while(std::cin >> instruct) {
        std::cin >> std::hex >> addr;
        std::cin >> x;
        
        if(instruct == 'l') {
            this->load(addr);
        } else if(instruct == 's') {
            this->store(addr);
        }
    }
}

    
// return results as a string
std::string Simulation::get_results() {
    std::stringstream res;

    res << "Total loads: " << total_loads << "\n";
    res << "Total stores: " << total_stores << "\n";
    res << "Load hits: " << load_hits << "\n";
    res << "Load misses: " << load_misses << "\n";
    res << "Store hits: " << store_hits << "\n";
    res << "Store misses: " << store_misses << "\n";
    res << "Total cycles: " << total_cycles;
        
    return res.str(); 
}
