
#ifndef GARBAGE_FILES_HPP
#define GARBAGE_FILES_HPP

#include <vector>
#include <random>
#include <cstdint>
//function to get random bytes with given size
std::vector<uint8_t> generateGarbageBytes(size_t size) {
    std::vector<uint8_t> garbage(size);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<unsigned int> dis(0, 255);

    for (auto& byte : garbage) {
        byte = static_cast<uint8_t>(dis(gen)); 
    }

    return garbage;
}


#endif


