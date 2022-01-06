#ifndef SET_H
#define SET_H

#include <stdint.h>
#include <unordered_map>
#include <list>
#include <sstream>

// define standard data structures we will be using
using pair_t = std::pair<uint32_t, int>;
using list_t = std::list<pair_t>;
using map_t = std::unordered_map<uint32_t, list_t::iterator>;

class Set {
    
private:
    // number of blocks    
    uint32_t block_num;
        
    // hash table for holding positions of blocks    
    map_t positions;

    // list for holding blocks    
    list_t blocks;

protected:
    // adds <tag, dirty> pair tofront of list, updates map, and evicts last block (if size goes over max)    
    // returns 1 if evicted block is dirty
    int add(uint32_t tag, int dirty);

    // returns 1 if tag in the list 0 otherwise
    int in_set(uint32_t tag);

    // erases <tag, dirty> pair from list and map
    int erase(uint32_t tag);

    // changes dirst bit to 1 if tag in list
    void smear(uint32_t tag);

public:
    // Creates a set with a fixed set size, block size, and evict type
    Set(uint32_t block_num);

    // virtual destructor
    virtual ~Set();

    // takes an address and returns 1 if a memory in cache, 0 otherwise
    virtual int get(uint32_t tag) = 0;

    // takes an address and stores it into set
    // returns 1 if dirty block gets overwritten
    // returns 0 otherwise (block already in set, block clean, block empty)
    virtual int put(uint32_t tag, int dirty) = 0;

    // to string method fro testing
    std::string toString();

};

#endif
