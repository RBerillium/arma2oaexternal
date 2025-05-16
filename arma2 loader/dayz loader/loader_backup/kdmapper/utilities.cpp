#include "utilities.hpp"

#include "../xor.hpp"

uint32_t utilities_t::start_service(std::string name)
{
	HMODULE ntdll = GetModuleHandleA(xor ("ntdll.dll"));

	if (!ntdll)
	{
		return 0xdeadbeef;
	}

	BOOLEAN buffer;

	if (!NT_SUCCESS(RtlAdjustPrivilege(10UL, true, false, &buffer)))
	{
		return 0xdeadbeef;
	}

	std::string str = xor ("\\Registry\\Machine\\System\\CurrentControlSet\\Services\\") + name;

	ANSI_STRING ansi_service;

	RtlInitAnsiString(&ansi_service, str.c_str());

	UNICODE_STRING unicode_servide;

	RtlAnsiStringToUnicodeString(&unicode_servide, &ansi_service, true);

	NTSTATUS status = NtLoadDriver(&unicode_servide);

	return status;
}

uint32_t utilities_t::stop_service(std::string name)
{
	HMODULE ntdll = GetModuleHandleA(xor ("ntdll.dll"));

	if (!ntdll)
	{
		return false;
	}

	std::string str = xor ("\\Registry\\Machine\\System\\CurrentControlSet\\Services\\") + name;

	ANSI_STRING ansi_service;

	RtlInitAnsiString(&ansi_service, str.c_str());

	UNICODE_STRING unicode_servide;

	RtlAnsiStringToUnicodeString(&unicode_servide, &ansi_service, true);

	NTSTATUS status = NtUnloadDriver(&unicode_servide);

	return status;
}

bool utilities_t::create_service(std::string name, std::string path)
{
	HKEY service;

	if (ERROR_SUCCESS != RegCreateKeyA(HKEY_LOCAL_MACHINE, std::string(xor ("SYSTEM\\CurrentControlSet\\Services\\") + name).c_str(), &service))
	{
		return false;
	}

	std::string driver_path = std::string(xor ("\\??\\") + path);

	if (ERROR_SUCCESS != RegSetKeyValueA(service, 0, xor ("ImagePath"), REG_EXPAND_SZ, driver_path.c_str(), driver_path.size()))
	{
		RegCloseKey(service);
		return false;
	}

	uint32_t type = 1;

	if (ERROR_SUCCESS != RegSetKeyValueA(service, 0, xor ("Type"), REG_DWORD, &type, sizeof(DWORD)))
	{
		RegCloseKey(service);

		return false;
	}

	RegCloseKey(service);

	return true;
}

bool utilities_t::delete_service(std::string name)
{
	if (ERROR_SUCCESS != RegDeleteKeyA(HKEY_LOCAL_MACHINE, std::string(xor ("SYSTEM\\CurrentControlSet\\Services\\") + name).c_str()))
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool utilities_t::create_file_from_memory(std::string path, uint8_t* bytes, size_t size)
{
	std::ofstream file(path.c_str(), std::ios_base::out | std::ios_base::binary);

	if (file.write(reinterpret_cast<const char*>(bytes), size))
	{
		file.close();

		return true;
	}
	else
	{
		file.close();

		return false;
	}
}

bool utilities_t::delete_file(std::string path)
{
	std::ofstream file(path.c_str(), std::ios_base::out | std::ios_base::binary);

	if (!file)
	{
		file.close();
	}
	else
	{
		uint32_t new_size = sizeof(rtcore64) + ((uint64_t)rand() % 2348767 + 56725);

		byte* random_data = new byte[new_size];

		for (uint32_t i = 0; i < new_size; i++)
		{
			random_data[i] = (byte)(rand() % 255);
		}
		if (file.write((char*)random_data, new_size))
		{
			log(xor ("[+] driver data destroyed before remove. \n"));
		}

		file.close();

		delete[] random_data;
	}

	if (remove(path.c_str()) == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool utilities_t::data_compare(byte* data, byte* mask, const char* sz_mask)
{
	for (; *sz_mask; sz_mask++, data++, mask++)
	{
		if (*sz_mask == 'x' && *data != *mask)
		{
			return false;
		}
	}

	return (*sz_mask) == false;
}

std::string utilities_t::random_string(size_t size)
{
	std::string string;

	std::string alphabet = xor ("abcdefghijklmnopqrstuvwxyz");

	srand(time(0) * GetCurrentThreadId());

	for (uint32_t i = 0; i < size; i++)
	{
		string.push_back(alphabet.at(rand() % alphabet.length()));
	}

	return string;
}

uint64_t utilities_t::get_kernel_module_base(std::string module_name)
{
	void* buffer = nullptr;

	DWORD buffer_size = 0;

	NTSTATUS status = NtQuerySystemInformation((SYSTEM_INFORMATION_CLASS)(11), buffer, buffer_size, &buffer_size);

	while (status == 0xC0000004)
	{
		if (buffer != nullptr)
		{
			VirtualFree(buffer, 0, MEM_RELEASE);
		}

		buffer = VirtualAlloc(nullptr, buffer_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		status = NtQuerySystemInformation((SYSTEM_INFORMATION_CLASS)(11), buffer, buffer_size, &buffer_size);
	}

	if (!NT_SUCCESS(status))
	{
		if (buffer != nullptr)
		{
			VirtualFree(buffer, 0, MEM_RELEASE);
		}

		return 0;
	}

	PRTL_PROCESS_MODULES modules = static_cast<PRTL_PROCESS_MODULES>(buffer);

	if (!modules)
	{
		return 0;
	}

	for (uint32_t i = 0; i < modules->NumberOfModules; i++)
	{
		std::string current_module_name = std::string(reinterpret_cast<char*>(modules->Modules[i].FullPathName) + modules->Modules[i].OffsetToFileName);

		if (!_stricmp(current_module_name.c_str(), module_name.c_str()))
		{
			uint64_t result = reinterpret_cast<uint64_t>(modules->Modules[i].ImageBase);

			VirtualFree(buffer, 0, MEM_RELEASE);

			return result;
		}
	}

	VirtualFree(buffer, 0, MEM_RELEASE);

	return 0;
}

uint64_t utilities_t::find_pattern(uint64_t address, uint64_t len, byte* mask, char* sz_mask)
{
	size_t max_len = len - strlen(sz_mask);

	for (uint64_t i = 0; i < max_len; i++)
	{
		if (data_compare((byte*)(address + i), mask, sz_mask))
		{
			return (uint64_t)(address + i);
		}
	}

	return 0;
}

uint64_t utilities_t::find_section(char* name, uint64_t module_base, uint32_t* size)
{
	size_t namelength = strlen(name);

	PIMAGE_NT_HEADERS headers = (PIMAGE_NT_HEADERS)(module_base + ((PIMAGE_DOS_HEADER)module_base)->e_lfanew);

	PIMAGE_SECTION_HEADER sections = IMAGE_FIRST_SECTION(headers);

	for (uint32_t i = 0; i < headers->FileHeader.NumberOfSections; i++)
	{
		PIMAGE_SECTION_HEADER section = &sections[i];

		if (memcmp(section->Name, name, namelength) == 0 && namelength == strlen((char*)section->Name))
		{
			if (!section->VirtualAddress)
			{
				return 0;
			}
			if (size)
			{
				*size = section->Misc.VirtualSize;
			}
			return (module_base + section->VirtualAddress);
		}
	}
	return 0;
}