#include "adtls.h"
    
// two following functions return the offset given a certain cache configuation
// block_size is the size in bytes of a block
// num_set is the number of sets in the cache
uint32_t adtls::get_set(uint32_t addr, uint32_t block_sft, uint32_t set_sft) {
    addr = addr >> (block_sft);
    return addr & ((1 << set_sft) - 1);
}

uint32_t adtls::get_tag(uint32_t addr, uint32_t block_sft, uint32_t set_sft) {
    return addr >> (block_sft + set_sft);
}

// get the log base 2 of a number (assume that the number is a power of 2)
int adtls::get_sft(uint32_t num) {
    int sft = 0;

    while(num != 1) {
        num = num >> 1;
        sft += 1;
    }

    return sft;
}

// check number is greater than 0 and has binary property
int adtls::mult_two(int32_t num) {
    return (num > 0) && ((num & (num - 1)) == 0);
}

int adtls::string_check(std::string s1, std::string s2, std::string s3) {
    if(s1.compare(s2) == 0) {
        return 1;
    } else if(s1.compare(s3) == 0) {
        return 0;
    }

    return -1;
}
