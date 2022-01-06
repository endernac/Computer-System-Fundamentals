// Not really sure why I'm using a new namespace, but why not ¯\_(ツ)_/¯
// stands for address tools

#ifndef ADDRESS_TOOLS_H
#define ADDRESS_TOOLS_H

#include <stdint.h>
#include <string>

namespace adtls {
    
    // two following functions return the offset given a certain cache configuation
    // num_block is the number of blocks in a set
    // num_set is the number of sets in the cache
    uint32_t get_set(uint32_t addr, uint32_t block_sft, uint32_t set_sft);

    uint32_t get_tag(uint32_t addr, uint32_t block_sft, uint32_t set_sft);

    // get the log base 2 of the number (assume number is a power of 2)
    int get_sft(uint32_t num);

    // check number is greater than 0 and has binary property
    int mult_two(int32_t num);

    // string check (technically not an address tool but whatever)
    // returns 1 if first string matches second
    // returns 0 if first string matches third
    // returns -1 otherwise
    int string_check(std::string s1, std::string s2, std::string s3);

}

#endif
