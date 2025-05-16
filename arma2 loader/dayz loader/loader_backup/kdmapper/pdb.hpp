#pragma once

#include "global.hpp"

#include <fstream>
#include <filesystem>
#include <DbgHelp.h>

#pragma comment(lib, "DbgHelp.lib")
#pragma comment(lib, "Urlmon.lib")

class pdb_t
{
private:

	std::string path;

	HANDLE process;

	HANDLE pdb_file;

	typedef struct struct_entity
	{
		std::string name;
		uint32_t offset;
	};

	typedef struct struct_info
	{
		std::vector<struct_entity> data;
		uint32_t size;

		uint32_t offset_of(std::string name)
		{
			for (struct_entity entity : data)
			{
				if (name == entity.name)
				{
					return entity.offset;
				}
			}
		}
	};

public:

	bool load(std::string name);

	void unload();

	uint32_t get_rva(std::string name);

	struct_info get_struct(std::string name);
}; 