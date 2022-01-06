#include <iostream>
#include <string>
#include "adtls.h"
#include "simulation.h"

int main(int argc, char** argv) 
{
	if(argc != 7)
	{
        std::cout << "Error: Incorrect number of arguments" << std::endl;
        return 1;
	}

    // we need int32_t in case we want to simulate a large direct/associative cache with small
    // block size. It need to be signed because the user might input a negative number
    int32_t set_num = std::stoi(argv[1]);
    int32_t block_num = std::stoi(argv[2]);
    int32_t block_size = std::stoi(argv[3]);
    int write_alloc = adtls::string_check(std::string (argv[4]), std::string ("write-allocate"), std::string ("no-write-allocate"));
    int write_thru = adtls::string_check(std::string (argv[5]), std::string ("write-through"), std::string ("write-back"));
    int evict = adtls::string_check(std::string (argv[6]), std::string ("lru"), std::string ("fifo"));

    // error checking the numerical inputs
    if(!adtls::mult_two(set_num)) {
        std::cout << "Error: Invalid number of sets" << std::endl;
        return 2;
    }

    if(!adtls::mult_two(block_num)) {
        std::cout << "Error: Invalid number of blocks per set" << std::endl;
        return 2;
    }

    if(!adtls::mult_two(block_size) || block_size < 4) {
        std::cout << "Error: Invalid block size" << std::endl;
        return 2;
    }
    
    // error checking string inputs
    if(write_alloc == -1 || write_thru == -1 || (!write_alloc && !write_thru)) {
        std::cout << "Error: Invalid write setting" << std::endl;
        return 2;
    }

    if(evict == -1) {
        std::cout << "Error: Invalid eviction setting" << std::endl;
        return 2;
    }

    Simulation csim (set_num, block_num, block_size, write_alloc, write_thru, evict);
    csim.run();
    std::cout << csim.get_results() << std::endl;

    return 0;

}
