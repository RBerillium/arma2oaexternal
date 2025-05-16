#pragma once

#include "global.hpp"

class utilities_t
{
public:

	uint32_t start_service(std::string name);

	uint32_t stop_service(std::string name);

	bool create_service(std::string name, std::string path);

	bool delete_service(std::string name);

	bool create_file_from_memory(std::string path, uint8_t* bytes, size_t size);

	bool delete_file(std::string path);

	std::string random_string(size_t size);

	uint64_t get_kernel_module_base(std::string module_name);

	bool data_compare(byte* data, byte* mask, const char* sz_mask);

	uint64_t find_pattern(uint64_t address, uint64_t len, byte* mask, char* sz_mask);

	uint64_t find_section(char* name, uint64_t module_base, uint32_t* size);
};