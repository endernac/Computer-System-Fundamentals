#ifndef SIMULATION_H
#define SIMULATION_H

#include <iostream>
#include <stdint.h>
#include <string>
#include <sstream>
#include "cache.h"

class Simulation {

private:
    int total_loads;    // technically redundant, but useful for error checking
    int load_hits;
    int load_misses;

    int total_stores;   // technically redundant
    int store_hits;
    int store_misses;

    int total_cycles;

    const int write_alloc;
    const int write_thru;
    const int block_words;

    Cache *cache;     

    void store(uint32_t addr);
    void load(uint32_t addr);

public:
    // creates a simulation based on given parameters
    Simulation(uint32_t sn, uint32_t bn, uint32_t bs, int wa, int wt, int ev);

    // destructor
    ~Simulation();

    // runs simulation on input
    void run();

    // return results as a string
    std::string get_results();

};

#endif
