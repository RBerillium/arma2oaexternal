#pragma once

#include "global.hpp"

class pe_t
{
public:

	typedef struct reloc_info
	{
		uint64_t address;
		uint16_t* item;
		uint32_t count;
	};

	typedef struct import_function_info
	{
		std::string name;
		uint64_t* address;
	};

	typedef struct import_info
	{
		std::string module_name;
		std::vector<import_function_info> function_datas;
	};

	using vec_sections = std::vector<IMAGE_SECTION_HEADER>;
	using vec_relocs = std::vector<reloc_info>;
	using vec_imports = std::vector<import_info>;

public:

	PIMAGE_NT_HEADERS64 get_nt_headers(void* image_base);

	vec_relocs get_relocs(void* image_base);

	vec_imports get_imports(void* image_base);
};