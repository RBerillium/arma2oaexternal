
#include "import.hpp"

uint64_t TranslateLinearAddress(uint64_t directoryTableBase, uint64_t virtualAddress);
NTSTATUS ReadPhysicalAddress(PVOID TargetAddress, PVOID lpBuffer, SIZE_T Size, SIZE_T* BytesRead);
NTSTATUS WritePhysicalAddress(PVOID TargetAddress, PVOID lpBuffer, SIZE_T Size, SIZE_T* BytesWritten);
ULONG_PTR process_dirbase = 0;
//PVOID GetProcessBaseAddress(int pid)
//{
//	PEPROCESS pProcess = NULL;
//	if (pid == 0) return STATUS_UNSUCCESSFUL;
//
//	NTSTATUS NtRet = PsLookupProcessByProcessId(pid, &pProcess);
//	if (NtRet != STATUS_SUCCESS) return NtRet;
//
//	PVOID Base = PsGetProcessSectionBaseAddress(pProcess);
//	ObDereferenceObject(pProcess);
//	return Base;
//}

//https://ntdiff.github.io/
#define WINDOWS_1803 17134
#define WINDOWS_1809 17763
#define WINDOWS_1903 18362
#define WINDOWS_1909 18363
#define WINDOWS_2004 19041
#define WINDOWS_20H2 19569
#define WINDOWS_21H1 20180

DWORD GetUserDirectoryTableBaseOffset()
{
	
	RTL_OSVERSIONINFOW ver = { 0 };
	KLI_CACHED_CALL(RtlGetVersion, &ver);

	switch (ver.dwBuildNumber)
	{
	case WINDOWS_1803:
		return 0x0278;
		break;
	case WINDOWS_1809:
		return 0x0278;
		break;
	case WINDOWS_1903:
		return 0x0280;
		break;
	case WINDOWS_1909:
		return 0x0280;
		break;
	case WINDOWS_2004:
		return 0x0388;
		break;
	case WINDOWS_20H2:
		return 0x0388;
		break;
	case WINDOWS_21H1:
		return 0x0388;
		break;
	default:
		return 0x0388;
	}
}

//check normal dirbase if 0 then get from UserDirectoryTableBas


#define DECRYPT_CR3(cr3, key, v33) (cr3 & 0xBFFF000000000FFF) | (((key ^ v33 ^ (v33 << 32)) & 0xFFFFFFFFF) << 12)

//ULONG_PTR GetProcessCr3(PEPROCESS pProcess)
//{
//	
//	KAPC_STATE State = { 0 };
//	KeStackAttachProcess(pProcess, &State);
//	ULONG_PTR Cr3 = __readcr3();
//	KeUnstackDetachProcess(&State);
//	return Cr3;
//}

ULONG_PTR GetProcessCr3(PEPROCESS pProcess)
{
	
	PUCHAR process = (PUCHAR)pProcess;
	ULONG_PTR process_dirbase = *(PULONG_PTR)(process + 0x28); //dirbase x64, 32bit is 0x18
	if (process_dirbase == 0)
	{
		DWORD UserDirOffset = GetUserDirectoryTableBaseOffset();
		ULONG_PTR process_userdirbase = *(PULONG_PTR)(process + UserDirOffset);
		return process_userdirbase;
	}
	return process_dirbase;
}


ULONG_PTR GetKernelDirBase()
{
	PUCHAR process = (PUCHAR)KLI_CACHED_CALL(PsGetCurrentProcess);
	ULONG_PTR cr3 = *(PULONG_PTR)(process + 0x28); //dirbase x64, 32bit is 0x18
	return cr3;
}

NTSTATUS ReadVirtual(uint64_t dirbase, uint64_t address, uint8_t* buffer, SIZE_T size, SIZE_T* read)
{
	uint64_t paddress = TranslateLinearAddress(dirbase, address);
	return ReadPhysicalAddress((PVOID)paddress, buffer, size, read);
}

NTSTATUS WriteVirtual(uint64_t dirbase, uint64_t address, uint8_t* buffer, SIZE_T size, SIZE_T* written)
{
	uint64_t paddress = TranslateLinearAddress(dirbase, address);
	return WritePhysicalAddress((PVOID)paddress, buffer, size, written);
}

NTSTATUS ReadPhysicalAddress(PVOID TargetAddress, PVOID lpBuffer, SIZE_T Size, SIZE_T* BytesRead)
{
	MM_COPY_ADDRESS AddrToRead = { 0 };
	AddrToRead.PhysicalAddress.QuadPart = (unsigned long long)TargetAddress;
	return KLI_CACHED_CALL(MmCopyMemory,lpBuffer, AddrToRead, Size, MM_COPY_MEMORY_PHYSICAL, BytesRead);
}

//MmMapIoSpaceEx limit is page 4096 byte
NTSTATUS WritePhysicalAddress(PVOID TargetAddress, PVOID lpBuffer, SIZE_T Size, SIZE_T* BytesWritten)
{
	if (!TargetAddress)
		return STATUS_UNSUCCESSFUL;

	PHYSICAL_ADDRESS AddrToWrite = { 0 };
	AddrToWrite.QuadPart = (unsigned long long)TargetAddress;

	PVOID pmapped_mem = KLI_CACHED_CALL(MmMapIoSpaceEx,AddrToWrite, Size, PAGE_READWRITE);

	if (!pmapped_mem)
		return STATUS_UNSUCCESSFUL;

	KLI_CACHED_CALL(memcpy,pmapped_mem, lpBuffer, Size);

	*BytesWritten = Size;
	KLI_CACHED_CALL(MmUnmapIoSpace,pmapped_mem, Size);
	return STATUS_SUCCESS;
}

#define PAGE_OFFSET_SIZE 12
static const uint64_t PMASK = (~0xfull << 8) & 0xfffffffffull;

uint64_t TranslateLinearAddress(uint64_t directoryTableBase, uint64_t virtualAddress) {
	directoryTableBase &= ~0xf;

	uint64_t pageOffset = virtualAddress & ~(~0ul << PAGE_OFFSET_SIZE);
	uint64_t pte = ((virtualAddress >> 12) & (0x1ffll));
	uint64_t pt = ((virtualAddress >> 21) & (0x1ffll));
	uint64_t pd = ((virtualAddress >> 30) & (0x1ffll));
	uint64_t pdp = ((virtualAddress >> 39) & (0x1ffll));

	SIZE_T readsize = 0;
	uint64_t pdpe = 0;
	ReadPhysicalAddress((PVOID)(directoryTableBase + 8 * pdp), &pdpe, sizeof(pdpe), &readsize);
	if (~pdpe & 1)
		return 0;

	uint64_t pde = 0;
	ReadPhysicalAddress((PVOID)((pdpe & PMASK) + 8 * pd), &pde, sizeof(pde), &readsize);
	if (~pde & 1)
		return 0;

	/* 1GB large page, use pde's 12-34 bits */
	if (pde & 0x80)
		return (pde & (~0ull << 42 >> 12)) + (virtualAddress & ~(~0ull << 30));

	uint64_t pteAddr = 0;
	ReadPhysicalAddress((PVOID)((pde & PMASK) + 8 * pt), &pteAddr, sizeof(pteAddr), &readsize);
	if (~pteAddr & 1)
		return 0;

	/* 2MB large page */
	if (pteAddr & 0x80)
		return (pteAddr & PMASK) + (virtualAddress & ~(~0ull << 21));

	virtualAddress = 0;
	ReadPhysicalAddress((PVOID)((pteAddr & PMASK) + 8 * pte), &virtualAddress, sizeof(virtualAddress), &readsize);
	virtualAddress &= PMASK;

	if (!virtualAddress)
		return 0;

	return virtualAddress + pageOffset;
}


//
NTSTATUS ReadProcessMemory(PEPROCESS process, PVOID Address, PVOID AllocatedBuffer, SIZE_T size, SIZE_T* read)
{
	
	//if (pid == 0) return STATUS_UNSUCCESSFUL;

	NTSTATUS NtRet; //= KLI_CACHED_CALL(PsLookupProcessByProcessId,(HANDLE)pid, &pProcess);
	//if (NtRet != STATUS_SUCCESS) return NtRet;
	if(process_dirbase == 0)
	process_dirbase = GetProcessCr3(process);
	//KLI_CACHED_CALL(ObfDereferenceObject,pProcess);

	SIZE_T CurOffset = 0;
	SIZE_T TotalSize = size;
	while (TotalSize)
	{

		uint64_t CurPhysAddr = TranslateLinearAddress(process_dirbase, (ULONG64)Address + CurOffset);
		if (!CurPhysAddr) return STATUS_UNSUCCESSFUL;

		ULONG64 ReadSize = min(PAGE_SIZE - (CurPhysAddr & 0xFFF), TotalSize);
		SIZE_T BytesRead = 0;
		NtRet = ReadPhysicalAddress((PVOID)CurPhysAddr, (PVOID)((ULONG64)AllocatedBuffer + CurOffset), ReadSize, &BytesRead);
		TotalSize -= BytesRead;
		CurOffset += BytesRead;
		if (NtRet != STATUS_SUCCESS) break;
		if (BytesRead == 0) break;
	}

	*read = CurOffset;
	return NtRet;
}

NTSTATUS WriteProcessMemory(PEPROCESS process, PVOID Address, PVOID AllocatedBuffer, SIZE_T size, SIZE_T* written)
{
	PEPROCESS pProcess = process;
	//if (pid == 0) return STATUS_UNSUCCESSFUL;

	NTSTATUS NtRet;// = KLI_CACHED_CALL(PsLookupProcessByProcessId, (HANDLE)pid, &pProcess);
	//if (NtRet != STATUS_SUCCESS) return NtRet;
	if(process_dirbase == 0)
		process_dirbase = GetProcessCr3(process);
	
	if (!Address)
	{
		return STATUS_UNSUCCESSFUL;
	}
	SIZE_T CurOffset = 0;
	SIZE_T TotalSize = size;
	while (TotalSize)
	{
		uint64_t CurPhysAddr = TranslateLinearAddress(process_dirbase, (ULONG64)Address + CurOffset);
		if (!CurPhysAddr) return STATUS_UNSUCCESSFUL;

		ULONG64 WriteSize = min(PAGE_SIZE - (CurPhysAddr & 0xFFF), TotalSize);
		SIZE_T BytesWritten = 0;
		NtRet = WritePhysicalAddress((PVOID)CurPhysAddr, (PVOID)((ULONG64)AllocatedBuffer + CurOffset), WriteSize, &BytesWritten);
		TotalSize -= BytesWritten;
		CurOffset += BytesWritten;
		if (NtRet != STATUS_SUCCESS) break;
		if (BytesWritten == 0) break;
	}

	*written = CurOffset;
	return NtRet;
}