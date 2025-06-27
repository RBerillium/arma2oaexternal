#pragma once

#include <windows.h>
#include <winternl.h>
#include <string>
#include <stdint.h>
#include <vector>
#include <cmath>

#pragma comment(lib, "ntdll.lib")

#include "core.hpp"
#include "executable.hpp"
#include "..//pdb_parser.hpp"
#include "utilities.hpp"
#include "rtcore64.hpp"

#define logger_on

typedef struct _RTL_PROCESS_MODULE_INFORMATION
{
    HANDLE Section;
    PVOID MappedBase;
    PVOID ImageBase;
    ULONG ImageSize;
    ULONG Flags;
    USHORT LoadOrderIndex;
    USHORT InitOrderIndex;
    USHORT LoadCount;
    USHORT OffsetToFileName;
    UCHAR FullPathName[256];

} RTL_PROCESS_MODULE_INFORMATION, * PRTL_PROCESS_MODULE_INFORMATION;

typedef struct _RTL_PROCESS_MODULES
{
    ULONG NumberOfModules;
    RTL_PROCESS_MODULE_INFORMATION Modules[1];

} RTL_PROCESS_MODULES, * PRTL_PROCESS_MODULES;


typedef struct _SYSTEM_HANDLE
{
	PVOID Object;
	HANDLE UniqueProcessId;
	HANDLE HandleValue;
	ULONG GrantedAccess;
	USHORT CreatorBackTraceIndex;
	USHORT ObjectTypeIndex;
	ULONG HandleAttributes;
	ULONG Reserved;

} SYSTEM_HANDLE, * PSYSTEM_HANDLE;

typedef struct _SYSTEM_HANDLE_INFORMATION_EX
{
	ULONG_PTR HandleCount;
	ULONG_PTR Reserved;
	SYSTEM_HANDLE Handles[1];

} SYSTEM_HANDLE_INFORMATION_EX, * PSYSTEM_HANDLE_INFORMATION_EX;

typedef struct _RTL_BALANCED_LINKS 
{
	struct _RTL_BALANCED_LINKS* Parent;
	struct _RTL_BALANCED_LINKS* LeftChild;
	struct _RTL_BALANCED_LINKS* RightChild;
	CHAR Balance;
	UCHAR Reserved[3];
} RTL_BALANCED_LINKS, * PRTL_BALANCED_LINKS;

typedef struct _RTL_AVL_TABLE
{
	RTL_BALANCED_LINKS BalancedRoot;
	PVOID OrderedPointer;
	ULONG WhichOrderedElement;
	ULONG NumberGenericTableElements;
	ULONG DepthOfTree;
	PVOID RestartKey;
	ULONG DeleteCount;
	PVOID CompareRoutine;
	PVOID AllocateRoutine;
	PVOID FreeRoutine;
	PVOID TableContext;

} RTL_AVL_TABLE, * PRTL_AVL_TABLE;

typedef struct _PiDDBCacheEntry
{
	LIST_ENTRY		List;
	UNICODE_STRING	DriverName;
	ULONG			TimeDateStamp;
	NTSTATUS		LoadStatus;
	char			_0x0028[16];

} PiDDBCacheEntry, * NPiDDBCacheEntry;

typedef struct _HashBucketEntry
{
	struct _HashBucketEntry* Next;
	UNICODE_STRING DriverName;
	ULONG CertHash[5];

} HashBucketEntry, * PHashBucketEntry;

extern "C"
{
    NTSYSAPI NTSTATUS NTAPI NtLoadDriver(_In_ PUNICODE_STRING DriverServiceName);

    NTSYSAPI NTSTATUS NTAPI NtUnloadDriver(_In_ PUNICODE_STRING DriverServiceName);

    NTSYSAPI NTSTATUS NTAPI RtlAdjustPrivilege(_In_ ULONG Privilege, _In_ BOOLEAN Enable, _In_ BOOLEAN Client, _Out_ PBOOLEAN WasEnabled);
}

#ifdef logger_on
#define log(...) printf(__VA_ARGS__)
#endif

#ifdef logger_off
#define log(...)
#endif