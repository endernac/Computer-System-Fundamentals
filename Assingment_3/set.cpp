#include "set.h"
        
// adds <tag, dirty> pair tofront of list, updates map, and evicts last block (if size goes over max)
// returns 1 if evicted block is dirty
int Set::add(uint32_t tag, int dirty) {
    if(this->in_set(tag)) {                     // (basic hygeine) check element not already in set
        return 0;
    }
        
    blocks.push_front(pair_t(tag, dirty));      // add new block to front
    positions[tag] = blocks.begin();            // store iterator in quick look up
    if(blocks.size() > block_num) {             // if too many blocks
        list_t::iterator pos = blocks.end();    // get end of list
        return this->erase((--pos)->first);     // erase last element and return dirty bit
    }
    
    return 0;
}


// returns 1 if tag in the list 0 otherwise
int Set::in_set(uint32_t tag) {
    return positions.find(tag) != positions.end();
}


// erases <tag, dirty> pair from list and map
// returns 1 if erased block was dirty
int Set::erase(uint32_t tag) {
    if(!this->in_set(tag)) {                // make sure tag is in set
        return 0;
    }

    list_t::iterator pos = positions[tag];  // get the position
    int dirty = pos->second;                // get dirty bit
    blocks.erase(pos);                      // finds tag in list
    positions.erase(tag);                   // erases tag from positions
    return dirty;                           // returns dirty bit
}


// changes dirty bit to 1 if tag in list
void Set::smear(uint32_t tag) {
    if(this->in_set(tag)) {                     // make sure tag is in set
        list_t::iterator pos = positions[tag];  // get the position
        pos->second = 1;                        // set dirty bit to 1
    }
}

std::string Set::toString() {
    std::stringstream s;
    for(list_t::iterator pos = blocks.begin(); pos != blocks.end(); pos++) {
        s << pos->first << ": " << pos->second << "\t";
    }
    return s.str();
}

// Creates a set with a fixed set size, block size, and evict type
Set::Set(uint32_t block_num) {
    this->block_num = block_num;
    positions = map_t();
    blocks = list_t();
}

// Noting allocated and no new objects created
Set::~Set() {}
