#pragma once

#include "global.hpp"


#include "../xor.hpp"

class mapper_t
{
private:

    std::string name;
    pdb_t pdb;
    std::string path;

private:

    pe_t pe;


    //pdb_t pdb1;

	core_t core;

	utilities_t utils;

private:

    uint64_t find_pattern_in_kernel(uint64_t address, uint64_t len, byte* mask, char* sz_mask);

    uint64_t find_section_in_kernel(char* name, uint64_t module_base, uint32_t* size);

    uint64_t find_pattern_in_section_in_kernel(char* name, uint64_t module_base, byte* mask, char* sz_mask);

private:

    bool free_pool(uint64_t address);

    void* resolve_relative_address(void* instruction, unsigned long offset, unsigned long instruction_size);

    bool ex_acquire_resource_exclusive_lite(void* resource, bool wait);

    bool ex_release_resource_lite(void* resource);

    bool rtl_delete_element_generic_table_avl(void* table, void* buffer);

    void* rtl_lookup_element_generic_table_avl(PRTL_AVL_TABLE table, void* Buffer);

    PiDDBCacheEntry* lookup_entry(PRTL_AVL_TABLE table, uint32_t timestamp, const wchar_t* name);

private:

    bool clear_wdfilter_driver_list();

    bool clear_mm_unloaded_drivers();

    bool clear_piddb_cache_table();

    bool clear_kernel_hash_bucket_list();

private:

    uint64_t allocate_independent_pages(uint32_t size);

    bool free_independent_pages(uint64_t address, uint32_t size);

    uint64_t allocate_big_pool(uint32_t size);
    
   // bool free_pool(uint64_t pool_address);
    //bool free_pool(uint64_t address, )

private:

	uint64_t get_kernel_export(uint64_t kernel_module_base, std::string function_name);

    template<typename T, typename ...A>
    bool call_kernel_function(T* result, uint64_t address, A ...arguments)
    {
        constexpr auto call_void = std::is_same_v<T, void>;

        if constexpr (!call_void)
        {
            if (!result)
            {
                return false;
            }
        }
        else
        {
            UNREFERENCED_PARAMETER(result);
        }

        HMODULE ntdll = GetModuleHandleA(xor ("ntdll.dll"));

        if (ntdll == 0)
        {
            return false;
        }

        void* nt_add_atom = reinterpret_cast<void*>(GetProcAddress(ntdll, xor ("NtAddAtom")));

        if (!nt_add_atom)
        {
            return false;
        }

        uint8_t jmp[] = { 0x48, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xe0 };

        uint8_t original[sizeof(jmp)];

        *(uint64_t*)&jmp[2] = address;

        uint64_t kernel_nt_add_atom = this->get_kernel_export(this->utils.get_kernel_module_base(xor("ntoskrnl.exe")), xor ("NtAddAtom"));

        if (!kernel_nt_add_atom)
        {
            return false;
        }

        if (!core.read_virtual(kernel_nt_add_atom, &original, sizeof(jmp)))
        {
            return false;
        }

        if (original[0] == jmp[0] && original[1] == jmp[1] && original[sizeof(jmp) - 2] == jmp[sizeof(jmp) - 2] && original[sizeof(jmp) - 1] == jmp[sizeof(jmp) - 1])
        {
            return false;
        }

        if (!core.write_virtual(kernel_nt_add_atom, &jmp, sizeof(jmp)))
        {
            return false;
        }
    
        if constexpr (!call_void)
        {
            using function = T(__stdcall*)(A...);

            function run = reinterpret_cast<function>(nt_add_atom);

            *result = run(arguments...);
        }
        else
        {
            using function = void(__stdcall*)(A...);

            function run = reinterpret_cast<function>(nt_add_atom);

            run(arguments...);
        }
    
        core.write_virtual(kernel_nt_add_atom, original, sizeof(jmp));

        return true;
    }

private:

    bool map_bytes(uint8_t* bytes, uint64_t arg1, uint64_t arg2, uint64_t arg3, bool free = false);

public:

    bool map(std::vector<uint8_t>& encrypted_image, uint64_t arg1, uint64_t arg2, uint64_t arg3, bool destroy_header, bool free = false);
};