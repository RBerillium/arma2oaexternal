#pragma once

#include "global.hpp"

class core_t
{
    friend class mapper_t;

private:

    uint32_t map_code = 0x80002000;

    uint32_t unmap_code = 0x80002004;

private:

    static HANDLE device;

private:

    bool virtual_to_physical(uint64_t virtual_address, uint64_t* physical_address);

    bool map_memory(uint64_t address, uint64_t size, void** section);

    bool unmap_memory(void* section);

public:

    bool connect();

    bool disconnect();

    bool read_virtual(uint64_t address, void* buffer, uint32_t size);

    bool write_virtual(uint64_t address, void* buffer, uint32_t size);

    bool read_physical(uint64_t address, void* buffer, uint32_t size);

    bool write_physical(uint64_t address, void* buffer, uint32_t size);
};