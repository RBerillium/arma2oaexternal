#pragma once
#include "kli.hpp"

extern "C"
{
	PPEB NTAPI PsGetProcessPeb(PEPROCESS);

	PVOID NTAPI RtlFindExportedRoutineByName(PVOID, PCCH);

	NTSTATUS NTAPI ZwQuerySystemInformation(INT, PVOID, ULONG, PULONG);

	NTSTATUS NTAPI MmCopyVirtualMemory(PEPROCESS, PVOID, PEPROCESS, PVOID, SIZE_T, KPROCESSOR_MODE, PSIZE_T);

	NTSTATUS NTAPI ZwQueryInformationProcess(
		_In_      HANDLE           ProcessHandle,
		_In_      PROCESSINFOCLASS ProcessInformationClass,
		_Out_     PVOID            ProcessInformation,
		_In_      ULONG            ProcessInformationLength,
		_Out_opt_ PULONG           ReturnLength
	);
}

KLI_CACHED_DEF(tolower);
KLI_CACHED_DEF(wcsstr);
KLI_CACHED_DEF(strcmp);
KLI_CACHED_DEF(RtlInitAnsiString);
KLI_CACHED_DEF(RtlAnsiStringToUnicodeString);
KLI_CACHED_DEF(RtlCompareUnicodeString);
KLI_CACHED_DEF(RtlFreeUnicodeString);
KLI_CACHED_DEF(ExAllocatePoolWithTag);
KLI_CACHED_DEF(ExFreePoolWithTag);
KLI_CACHED_DEF(CmRegisterCallback);
KLI_CACHED_DEF(ObfDereferenceObject);
KLI_CACHED_DEF(RtlRandomEx);
KLI_CACHED_DEF(KeStackAttachProcess);
KLI_CACHED_DEF(KeUnstackDetachProcess);
KLI_CACHED_DEF(PsLookupProcessByProcessId);
KLI_CACHED_DEF(PsGetProcessPeb);
KLI_CACHED_DEF(ZwQuerySystemInformation);
KLI_CACHED_DEF(MmCopyVirtualMemory);
KLI_CACHED_DEF(PsLookupThreadByThreadId);
KLI_CACHED_DEF(ZwQueryInformationProcess);
KLI_CACHED_DEF(MmCopyMemory);
KLI_CACHED_DEF(RtlGetVersion);
KLI_CACHED_DEF(PsGetCurrentProcess);
KLI_CACHED_DEF(MmMapIoSpaceEx);
KLI_CACHED_DEF(memcpy);
KLI_CACHED_DEF(MmUnmapIoSpace);
KLI_CACHED_DEF(ObOpenObjectByPointer);


void init_import()
{
	KLI_CACHED_SET(tolower);
	KLI_CACHED_SET(wcsstr);
	KLI_CACHED_SET(strcmp);
	KLI_CACHED_SET(RtlInitAnsiString);
	KLI_CACHED_SET(RtlAnsiStringToUnicodeString);
	KLI_CACHED_SET(RtlCompareUnicodeString);
	KLI_CACHED_SET(RtlFreeUnicodeString);
	KLI_CACHED_SET(ExAllocatePoolWithTag);
	KLI_CACHED_SET(ExFreePoolWithTag);
	KLI_CACHED_SET(CmRegisterCallback);
	KLI_CACHED_SET(ObfDereferenceObject);
	KLI_CACHED_SET(RtlRandomEx);
	KLI_CACHED_SET(KeStackAttachProcess);
	KLI_CACHED_SET(KeUnstackDetachProcess);
	KLI_CACHED_SET(PsLookupProcessByProcessId);
	KLI_CACHED_SET(PsGetProcessPeb);
	KLI_CACHED_SET(ZwQuerySystemInformation);
	KLI_CACHED_SET(MmCopyVirtualMemory);
	KLI_CACHED_SET(PsLookupThreadByThreadId);
	KLI_CACHED_SET(ZwQueryInformationProcess);
	KLI_CACHED_SET(MmCopyMemory);
	KLI_CACHED_SET(RtlGetVersion);
	KLI_CACHED_SET(PsGetCurrentProcess);
	KLI_CACHED_SET(MmMapIoSpaceEx);
	KLI_CACHED_SET(memcpy);
	KLI_CACHED_SET(MmUnmapIoSpace);
	KLI_CACHED_SET(ObOpenObjectByPointer);

}