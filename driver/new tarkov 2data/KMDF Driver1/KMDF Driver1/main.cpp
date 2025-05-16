#include <ntifs.h>
#include <stdint.h>
#include <ntstrsafe.h>

#include "physmem.hpp"

#include "import.hpp"
#include "utils.hpp"
#include "crypt.hpp"


__int64(__fastcall* original_win32k)(__int64 a1);

__int64(__fastcall* original_fastfat)(__int64 a1, __int64 a2, __int64 a3, __int64 a4, __int64 a5, __int64 a6);

//__int64(__fastcall* original_pdc)(__int64 a1);

__int64(__fastcall* oSleepstudyHelperBlockerActiveReference)(__int64 a1);









#define magic_value 0xef192ac9b

PEPROCESS game_process{};
PEPROCESS usermode_process{};
uint32_t game_pid;
uint32_t usermode_pid;

enum class type_t { module, memory, init, apc };

enum class copy_type { read, write };

PVOID ProcessModuleKernel(int processpid, PEPROCESS pProcess, const wchar_t* ModuleName)
{
	NTSTATUS status;
	PROCESS_BASIC_INFORMATION ProcInfo;
	ULONG Length = 0;
	SIZE_T Read = 0;

	HANDLE hProcess;
	status = KLI_CACHED_CALL(ObOpenObjectByPointer, pProcess, 0, NULL, 0, 0, KernelMode, &hProcess);
	if (!NT_SUCCESS(status))
		return NULL;

	if (!NT_SUCCESS(KLI_CACHED_CALL(ZwQueryInformationProcess,
		hProcess,
		ProcessBasicInformation,
		&ProcInfo,
		sizeof(PROCESS_BASIC_INFORMATION),
		&Length)))
	{
		return NULL;
	}

	PEB procPeb = { 0 };
	status = ReadProcessMemory(game_process, (PVOID)ProcInfo.PebBaseAddress, &procPeb, sizeof(PEB), &Read);
	if (!NT_SUCCESS(status))
		return NULL;

	PEB_LDR_DATA pebLdrData = { 0 };
	status = ReadProcessMemory(game_process, (PVOID)procPeb.Ldr, &pebLdrData, sizeof(PEB_LDR_DATA), &Read);
	if (!NT_SUCCESS(status))
		return NULL;


	LIST_ENTRY* pLdrListHead = (LIST_ENTRY*)pebLdrData.InLoadOrderModuleList.Flink;
	LIST_ENTRY* pLdrCurrentNode = pebLdrData.InLoadOrderModuleList.Flink;
	do
	{
		LDR_DATA_TABLE_ENTRY lstEntry = { 0 };
		status = ReadProcessMemory(game_process, (PVOID)pLdrCurrentNode, &lstEntry, sizeof(LDR_DATA_TABLE_ENTRY), &Read);
		if (!NT_SUCCESS(status))
			return NULL;

		pLdrCurrentNode = lstEntry.InLoadOrderLinks.Flink;

		if (lstEntry.BaseDllName.Length > 0)
		{
			WCHAR strBaseDllName[MAX_PATH] = { 0 };
			status = ReadProcessMemory(game_process, (PVOID)lstEntry.BaseDllName.Buffer, &strBaseDllName, lstEntry.BaseDllName.Length, &Read);
			if (NT_SUCCESS(status))
			{
				if (KLI_CACHED_CALL(wcsstr, strBaseDllName, ModuleName))
				{
					if (lstEntry.DllBase != nullptr && lstEntry.SizeOfImage != 0)
					{

						return lstEntry.DllBase;
					}
				}
			}
		}

	} while (pLdrListHead != pLdrCurrentNode);

	return NULL;
}

struct packet_struct
{
	uint64_t magic = 0xef192ac9b;

	type_t type;


	union
	{
		struct { uint64_t module_address; const wchar_t* module_name; } module;

		struct { uint32_t game; uint32_t usermode; } initinitialization;

		struct { uint64_t source_address; uint64_t dest_address; uint32_t size; copy_type type; } memory;

		struct { uint32_t thread; uint32_t new_value; uint32_t old_value; } apc;

	} data;
};

NTSTATUS hook_function(__int64 a1)
{
	auto packet = reinterpret_cast<packet_struct*>(a1);
	if (!packet)
	{
		return oSleepstudyHelperBlockerActiveReference(a1);
	}
	if (packet->magic == magic_value)
	{	

		if (packet->type == type_t::init)
		{

			game_pid = packet->data.initinitialization.game;
			usermode_pid = packet->data.initinitialization.usermode;

			HANDLE game = reinterpret_cast<HANDLE>(packet->data.initinitialization.game);
			HANDLE usermode = reinterpret_cast<HANDLE>(packet->data.initinitialization.usermode);
			process_dirbase = 0;
			KLI_CACHED_CALL(PsLookupProcessByProcessId, game, &game_process);
			KLI_CACHED_CALL(PsLookupProcessByProcessId, usermode, &usermode_process);


			//KLI_CACHED_CALL(ObfDereferenceObject, game);
			//KLI_CACHED_CALL(ObfDereferenceObject, usermode);

		}

		else if (packet->type == type_t::module)
		{
			void* address = NULL;
			PEPROCESS process = game_process;
			address = ProcessModuleKernel(game_pid, process, (const wchar_t*)packet->data.module.module_name);
			packet->data.module.module_address = reinterpret_cast<uint64_t>(address);

		}
		else if (packet->type == type_t::memory)
		{
			size_t bytes;
			if (packet->data.memory.type == copy_type::read)
			{

				//ReadProcessMemory(game_process, (PVOID)packet->data.memory.source_address, (PVOID)packet->data.memory.dest_address, packet->data.memory.size, &bytes);
				KLI_CACHED_CALL(MmCopyVirtualMemory, game_process, (PVOID)packet->data.memory.source_address, usermode_process, (PVOID)packet->data.memory.dest_address, packet->data.memory.size, UserMode, &bytes);
			}
			else
			{
				//WriteProcessMemory(game_process, (PVOID)packet->data.memory.dest_address, (PVOID)packet->data.memory.source_address, packet->data.memory.size, &bytes);
				KLI_CACHED_CALL(MmCopyVirtualMemory, usermode_process, (PVOID)packet->data.memory.source_address, game_process, (PVOID)packet->data.memory.dest_address, packet->data.memory.size, UserMode, &bytes);
			}
		}

		else if (packet->type == type_t::apc)
		{
			PETHREAD thread{};

			KLI_CACHED_CALL(PsLookupThreadByThreadId, (HANDLE)packet->data.apc.thread, &thread);

			packet->data.apc.old_value = reinterpret_cast<PKTHREAD_T>(thread)->MiscFlags;

			if (packet->data.apc.new_value == 0xffffbfff)
			{
				reinterpret_cast<PKTHREAD_T>(thread)->MiscFlags &= packet->data.apc.new_value;
			}
			else
			{
				reinterpret_cast<PKTHREAD_T>(thread)->MiscFlags = packet->data.apc.new_value;
			}

			KLI_CACHED_CALL(ObfDereferenceObject, thread);
		}
	}
	else
		return oSleepstudyHelperBlockerActiveReference(a1);

	return STATUS_SUCCESS;
}

NTSTATUS hook_setup(uint64_t win32_offset)
{
	
	////fastfat
	//const char* fastfat = "fastfat.sys";

	//auto fastfat_module = utils::get_kernel_module(fastfat);

	//fastfat = "";

	//if (!fastfat_module)
	//{
	//	return STATUS_UNSUCCESSFUL;
	//}

	//auto fastfat_function_address = fastfat_module + fastfat_offset;

	//auto fastfat_data_address = rva(fastfat_function_address + 0x16, 7);

	//*(void**)&original_fastfat = _InterlockedExchangePointer((void**)fastfat_data_address, (void*)hook_function);
	//DbgPrintEx(0, 0, "Fastfat hooked! \n");
	////fastfat

	//pds.sys
	
	
	//auto fastfat = skCrypt("fastfat.sys");

	//auto fastfat_module_address = utils::get_kernel_module(fastfat);

	//fastfat.clear();

	//if (!fastfat_module_address)
	//{
	//	return STATUS_UNSUCCESSFUL;
	//}

	//auto fastfat_function_address = fastfat_module_address + pci_offset;

	//auto fastfat_data_address = rva(fastfat_function_address + 0x19, 7);

	//*(void**)&oSleepstudyHelperBlockerActiveReference = _InterlockedExchangePointer((void**)fastfat_data_address, (void*)hook_function);
	

	//pdc.sys


	auto win32k = skCrypt("win32k.sys");

	auto win32k_module = utils::get_kernel_module(win32k);

	win32k.clear();

	if (!win32k_module)
	{
		return STATUS_UNSUCCESSFUL;
	}
	
	auto win32k_function_address = win32k_module + win32_offset;

	auto win32k_data_address = rva(win32k_function_address + 0x4, 7);

	*(void**)&original_win32k = _InterlockedExchangePointer((void**)win32k_data_address, (void*)hook_function);
	

	return STATUS_SUCCESS;


}
//uint64_t win32_offset,uint64_t offset_fastfat2
NTSTATUS driver_entry() //uint64_t win32_offset, uint64_t offset_fastfat ..uint64_t win32k_offset, uint64_t fastfat_offset
{
	uint64_t win32k_offset = 0xAB3C;// 0xAB3C - win11 //Windows10 0x90B0;
	//uint64_t fastfat_offset = 0x3A50;
	//EfsSetEfsInfoCallback

	
	init_import();
	
	hook_setup(win32k_offset);

	return STATUS_SUCCESS;
}