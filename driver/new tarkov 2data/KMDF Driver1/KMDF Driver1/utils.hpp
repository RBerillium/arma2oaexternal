#include <ntimage.h>
#include <minwindef.h>

#include "import.hpp"

#define rva(ptr, size) ((DWORD64)ptr + size + *(LONG*)((DWORD64)ptr + (size - sizeof(LONG)))) // <-- use this

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
    UCHAR  FullPathName[256];

} RTL_PROCESS_MODULE_INFORMATION, * PRTL_PROCESS_MODULE_INFORMATION;

typedef struct _RTL_PROCESS_MODULES
{
    ULONG NumberOfModules;
    RTL_PROCESS_MODULE_INFORMATION Modules[1];

} RTL_PROCESS_MODULES, * PRTL_PROCESS_MODULES;

typedef struct _PEB
{
    UCHAR InheritedAddressSpace;                                            //0x0
    UCHAR ReadImageFileExecOptions;                                         //0x1
    UCHAR BeingDebugged;                                                    //0x2
    union
    {
        UCHAR BitField;                                                     //0x3
        struct
        {
            UCHAR ImageUsesLargePages : 1;                                    //0x3
            UCHAR IsProtectedProcess : 1;                                     //0x3
            UCHAR IsImageDynamicallyRelocated : 1;                            //0x3
            UCHAR SkipPatchingUser32Forwarders : 1;                           //0x3
            UCHAR IsPackagedProcess : 1;                                      //0x3
            UCHAR IsAppContainer : 1;                                         //0x3
            UCHAR IsProtectedProcessLight : 1;                                //0x3
            UCHAR IsLongPathAwareProcess : 1;                                 //0x3
        };
    };
    UCHAR Padding0[4];                                                      //0x4
    VOID* Mutant;                                                           //0x8
    VOID* ImageBaseAddress;                                                 //0x10
    struct _PEB_LDR_DATA* Ldr;                                              //0x18
    struct _RTL_USER_PROCESS_PARAMETERS* ProcessParameters;                 //0x20
    VOID* SubSystemData;                                                    //0x28
    VOID* ProcessHeap;                                                      //0x30
    struct _RTL_CRITICAL_SECTION* FastPebLock;                              //0x38
    union _SLIST_HEADER* volatile AtlThunkSListPtr;                         //0x40
    VOID* IFEOKey;                                                          //0x48
    union
    {
        ULONG CrossProcessFlags;                                            //0x50
        struct
        {
            ULONG ProcessInJob : 1;                                           //0x50
            ULONG ProcessInitializing : 1;                                    //0x50
            ULONG ProcessUsingVEH : 1;                                        //0x50
            ULONG ProcessUsingVCH : 1;                                        //0x50
            ULONG ProcessUsingFTH : 1;                                        //0x50
            ULONG ProcessPreviouslyThrottled : 1;                             //0x50
            ULONG ProcessCurrentlyThrottled : 1;                              //0x50
            ULONG ProcessImagesHotPatched : 1;                                //0x50
            ULONG ReservedBits0 : 24;                                         //0x50
        };
    };
    UCHAR Padding1[4];                                                      //0x54
    union
    {
        VOID* KernelCallbackTable;                                          //0x58
        VOID* UserSharedInfoPtr;                                            //0x58
    };
    ULONG SystemReserved;                                                   //0x60
    ULONG AtlThunkSListPtr32;                                               //0x64
    VOID* ApiSetMap;                                                        //0x68
    ULONG TlsExpansionCounter;                                              //0x70
    UCHAR Padding2[4];                                                      //0x74
    struct _RTL_BITMAP* TlsBitmap;                                          //0x78
    ULONG TlsBitmapBits[2];                                                 //0x80
    VOID* ReadOnlySharedMemoryBase;                                         //0x88
    VOID* SharedData;                                                       //0x90
    VOID** ReadOnlyStaticServerData;                                        //0x98
    VOID* AnsiCodePageData;                                                 //0xa0
    VOID* OemCodePageData;                                                  //0xa8
    VOID* UnicodeCaseTableData;                                             //0xb0
    ULONG NumberOfProcessors;                                               //0xb8
    ULONG NtGlobalFlag;                                                     //0xbc
    union _LARGE_INTEGER CriticalSectionTimeout;                            //0xc0
    ULONGLONG HeapSegmentReserve;                                           //0xc8
    ULONGLONG HeapSegmentCommit;                                            //0xd0
    ULONGLONG HeapDeCommitTotalFreeThreshold;                               //0xd8
    ULONGLONG HeapDeCommitFreeBlockThreshold;                               //0xe0
    ULONG NumberOfHeaps;                                                    //0xe8
    ULONG MaximumNumberOfHeaps;                                             //0xec
    VOID** ProcessHeaps;                                                    //0xf0
    VOID* GdiSharedHandleTable;                                             //0xf8
    VOID* ProcessStarterHelper;                                             //0x100
    ULONG GdiDCAttributeList;                                               //0x108
    UCHAR Padding3[4];                                                      //0x10c
    struct _RTL_CRITICAL_SECTION* LoaderLock;                               //0x110
    ULONG OSMajorVersion;                                                   //0x118
    ULONG OSMinorVersion;                                                   //0x11c
    USHORT OSBuildNumber;                                                   //0x120
    USHORT OSCSDVersion;                                                    //0x122
    ULONG OSPlatformId;                                                     //0x124
    ULONG ImageSubsystem;                                                   //0x128
    ULONG ImageSubsystemMajorVersion;                                       //0x12c
    ULONG ImageSubsystemMinorVersion;                                       //0x130
    UCHAR Padding4[4];                                                      //0x134
    ULONGLONG ActiveProcessAffinityMask;                                    //0x138
    ULONG GdiHandleBuffer[60];                                              //0x140
    VOID(*PostProcessInitRoutine)();                                       //0x230
    struct _RTL_BITMAP* TlsExpansionBitmap;                                 //0x238
    ULONG TlsExpansionBitmapBits[32];                                       //0x240
    ULONG SessionId;                                                        //0x2c0
    UCHAR Padding5[4];                                                      //0x2c4
    union _ULARGE_INTEGER AppCompatFlags;                                   //0x2c8
    union _ULARGE_INTEGER AppCompatFlagsUser;                               //0x2d0
    VOID* pShimData;                                                        //0x2d8
    VOID* AppCompatInfo;                                                    //0x2e0
    struct _UNICODE_STRING CSDVersion;                                      //0x2e8
    struct _ACTIVATION_CONTEXT_DATA* ActivationContextData;                 //0x2f8
    struct _ASSEMBLY_STORAGE_MAP* ProcessAssemblyStorageMap;                //0x300
    struct _ACTIVATION_CONTEXT_DATA* SystemDefaultActivationContextData;    //0x308
    struct _ASSEMBLY_STORAGE_MAP* SystemAssemblyStorageMap;                 //0x310
    ULONGLONG MinimumStackCommit;                                           //0x318
    VOID* SparePointers[2];                                                 //0x320
    VOID* PatchLoaderData;                                                  //0x330
    struct _CHPEV2_PROCESS_INFO* ChpeV2ProcessInfo;                         //0x338
    ULONG AppModelFeatureState;                                             //0x340
    ULONG SpareUlongs[2];                                                   //0x344
    USHORT ActiveCodePage;                                                  //0x34c
    USHORT OemCodePage;                                                     //0x34e
    USHORT UseCaseMapping;                                                  //0x350
    USHORT UnusedNlsField;                                                  //0x352
    VOID* WerRegistrationData;                                              //0x358
    VOID* WerShipAssertPtr;                                                 //0x360
    VOID* EcCodeBitMap;                                                     //0x368
    VOID* pImageHeaderHash;                                                 //0x370
    union
    {
        ULONG TracingFlags;                                                 //0x378
        struct
        {
            ULONG HeapTracingEnabled : 1;                                     //0x378
            ULONG CritSecTracingEnabled : 1;                                  //0x378
            ULONG LibLoaderTracingEnabled : 1;                                //0x378
            ULONG SpareTracingBits : 29;                                      //0x378
        };
    };
    UCHAR Padding6[4];                                                      //0x37c
    ULONGLONG CsrServerReadOnlySharedMemoryBase;                            //0x380
    ULONGLONG TppWorkerpListLock;                                           //0x388
    struct _LIST_ENTRY TppWorkerpList;                                      //0x390
    VOID* WaitOnAddressHashTable[128];                                      //0x3a0
    VOID* TelemetryCoverageHeader;                                          //0x7a0
    ULONG CloudFileFlags;                                                   //0x7a8
    ULONG CloudFileDiagFlags;                                               //0x7ac
    CHAR PlaceholderCompatibilityMode;                                      //0x7b0
    CHAR PlaceholderCompatibilityModeReserved[7];                           //0x7b1
    struct _LEAP_SECOND_DATA* LeapSecondData;                               //0x7b8
    union
    {
        ULONG LeapSecondFlags;                                              //0x7c0
        struct
        {
            ULONG SixtySecondEnabled : 1;                                     //0x7c0
            ULONG Reserved : 31;                                              //0x7c0
        };
    };
    ULONG NtGlobalFlag2;                                                    //0x7c4
    ULONGLONG ExtendedFeatureDisableMask;                                   //0x7c8
} PEB, * PPEB;

typedef struct _PEB_LDR_DATA
{
    ULONG Length;                                                           //0x0
    UCHAR Initialized;                                                      //0x4
    VOID* SsHandle;                                                         //0x8
    struct _LIST_ENTRY InLoadOrderModuleList;                               //0x10
    struct _LIST_ENTRY InMemoryOrderModuleList;                             //0x20
    struct _LIST_ENTRY InInitializationOrderModuleList;                     //0x30
    VOID* EntryInProgress;                                                  //0x40
    UCHAR ShutdownInProgress;                                               //0x48
    VOID* ShutdownThreadId;                                                 //0x50
} PEB_LDR_DATA, * PPEB_LDR_DATA;

typedef struct _LDR_DATA_TABLE_ENTRY
{
    struct _LIST_ENTRY InLoadOrderLinks;                                    //0x0
    struct _LIST_ENTRY InMemoryOrderLinks;                                  //0x10
    struct _LIST_ENTRY InInitializationOrderLinks;                          //0x20
    VOID* DllBase;                                                          //0x30
    VOID* EntryPoint;                                                       //0x38
    ULONG SizeOfImage;                                                      //0x40
    struct _UNICODE_STRING FullDllName;                                     //0x48
    struct _UNICODE_STRING BaseDllName;                                     //0x58
    union
    {
        UCHAR FlagGroup[4];                                                 //0x68
        ULONG Flags;                                                        //0x68
        struct
        {
            ULONG PackagedBinary : 1;                                         //0x68
            ULONG MarkedForRemoval : 1;                                       //0x68
            ULONG ImageDll : 1;                                               //0x68
            ULONG LoadNotificationsSent : 1;                                  //0x68
            ULONG TelemetryEntryProcessed : 1;                                //0x68
            ULONG ProcessStaticImport : 1;                                    //0x68
            ULONG InLegacyLists : 1;                                          //0x68
            ULONG InIndexes : 1;                                              //0x68
            ULONG ShimDll : 1;                                                //0x68
            ULONG InExceptionTable : 1;                                       //0x68
            ULONG ReservedFlags1 : 2;                                         //0x68
            ULONG LoadInProgress : 1;                                         //0x68
            ULONG LoadConfigProcessed : 1;                                    //0x68
            ULONG EntryProcessed : 1;                                         //0x68
            ULONG ProtectDelayLoad : 1;                                       //0x68
            ULONG ReservedFlags3 : 2;                                         //0x68
            ULONG DontCallForThreads : 1;                                     //0x68
            ULONG ProcessAttachCalled : 1;                                    //0x68
            ULONG ProcessAttachFailed : 1;                                    //0x68
            ULONG CorDeferredValidate : 1;                                    //0x68
            ULONG CorImage : 1;                                               //0x68
            ULONG DontRelocate : 1;                                           //0x68
            ULONG CorILOnly : 1;                                              //0x68
            ULONG ChpeImage : 1;                                              //0x68
            ULONG ChpeEmulatorImage : 1;                                      //0x68
            ULONG ReservedFlags5 : 1;                                         //0x68
            ULONG Redirected : 1;                                             //0x68
            ULONG ReservedFlags6 : 2;                                         //0x68
            ULONG CompatDatabaseProcessed : 1;                                //0x68
        };
    };
    USHORT ObsoleteLoadCount;                                               //0x6c
    USHORT TlsIndex;                                                        //0x6e
    struct _LIST_ENTRY HashLinks;                                           //0x70
    ULONG TimeDateStamp;                                                    //0x80
    struct _ACTIVATION_CONTEXT* EntryPointActivationContext;                //0x88
    VOID* Lock;                                                             //0x90
    struct _LDR_DDAG_NODE* DdagNode;                                        //0x98
    struct _LIST_ENTRY NodeModuleLink;                                      //0xa0
    struct _LDRP_LOAD_CONTEXT* LoadContext;                                 //0xb0
    VOID* ParentDllBase;                                                    //0xb8
    VOID* SwitchBackContext;                                                //0xc0
    struct _RTL_BALANCED_NODE BaseAddressIndexNode;                         //0xc8
    struct _RTL_BALANCED_NODE MappingInfoIndexNode;                         //0xe0
    ULONGLONG OriginalBase;                                                 //0xf8
    union _LARGE_INTEGER LoadTime;                                          //0x100
    ULONG BaseNameHashValue;                                                //0x108
    enum _LDR_DLL_LOAD_REASON LoadReason;                                   //0x10c
    ULONG ImplicitPathOptions;                                              //0x110
    ULONG ReferenceCount;                                                   //0x114
    ULONG DependentLoadFlags;                                               //0x118
    UCHAR SigningLevel;                                                     //0x11c
    ULONG CheckSum;                                                         //0x120
    VOID* ActivePatchImageBase;                                             //0x128
    enum _LDR_HOT_PATCH_STATE HotPatchState;                                //0x130
} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;

union _KWAIT_STATUS_REGISTER
{
    UCHAR Flags;                                                            //0x0
    UCHAR State : 3;                                                          //0x0
    UCHAR Affinity : 1;                                                       //0x0
    UCHAR Priority : 1;                                                       //0x0
    UCHAR Apc : 1;                                                            //0x0
    UCHAR UserApc : 1;                                                        //0x0
    UCHAR Alert : 1;                                                          //0x0
};

typedef struct _KTHREAD
{
    struct _DISPATCHER_HEADER Header;                                       //0x0
    VOID* SListFaultAddress;                                                //0x18
    ULONGLONG QuantumTarget;                                                //0x20
    VOID* InitialStack;                                                     //0x28
    VOID* volatile StackLimit;                                              //0x30
    VOID* StackBase;                                                        //0x38
    ULONGLONG ThreadLock;                                                   //0x40
    volatile ULONGLONG CycleTime;                                           //0x48
    ULONG CurrentRunTime;                                                   //0x50
    ULONG ExpectedRunTime;                                                  //0x54
    VOID* KernelStack;                                                      //0x58
    struct _XSAVE_FORMAT* StateSaveArea;                                    //0x60
    struct _KSCHEDULING_GROUP* volatile SchedulingGroup;                    //0x68
    union _KWAIT_STATUS_REGISTER WaitRegister;                              //0x70
    volatile UCHAR Running;                                                 //0x71
    UCHAR Alerted[2];                                                       //0x72
    union
    {
        struct
        {
            ULONG AutoBoostActive : 1;                                        //0x74
            ULONG ReadyTransition : 1;                                        //0x74
            ULONG WaitNext : 1;                                               //0x74
            ULONG SystemAffinityActive : 1;                                   //0x74
            ULONG Alertable : 1;                                              //0x74
            ULONG UserStackWalkActive : 1;                                    //0x74
            ULONG ApcInterruptRequest : 1;                                    //0x74
            ULONG QuantumEndMigrate : 1;                                      //0x74
            ULONG SecureThread : 1;                                           //0x74
            ULONG TimerActive : 1;                                            //0x74
            ULONG SystemThread : 1;                                           //0x74
            ULONG ProcessDetachActive : 1;                                    //0x74
            ULONG CalloutActive : 1;                                          //0x74
            ULONG ScbReadyQueue : 1;                                          //0x74
            ULONG ApcQueueable : 1;                                           //0x74
            ULONG ReservedStackInUse : 1;                                     //0x74
            ULONG Spare : 1;                                                  //0x74
            ULONG TimerSuspended : 1;                                         //0x74
            ULONG SuspendedWaitMode : 1;                                      //0x74
            ULONG SuspendSchedulerApcWait : 1;                                //0x74
            ULONG CetUserShadowStack : 1;                                     //0x74
            ULONG BypassProcessFreeze : 1;                                    //0x74
            ULONG CetKernelShadowStack : 1;                                   //0x74
            ULONG StateSaveAreaDecoupled : 1;                                 //0x74
            ULONG Reserved : 8;                                               //0x74
        };
        LONG MiscFlags;                                                     //0x74
    };
} KTHREAD_T, * PKTHREAD_T;

namespace utils
{
    void* get_system_information(INT info_class)
    {
        unsigned long size = 32; char buffer[32];

        KLI_CACHED_CALL(ZwQuerySystemInformation, info_class, buffer, size, &size);

        void* info = KLI_CACHED_CALL(ExAllocatePoolWithTag, NonPagedPool, size, 'gkmd');

        if (!info)
        {
            return nullptr;
        }

        if (!NT_SUCCESS(KLI_CACHED_CALL(ZwQuerySystemInformation, info_class, info, size, &size)))
        {
            KLI_CACHED_CALL(ExFreePoolWithTag, info, 'gkmd');

            return nullptr;
        }

        return info;
    }

    uint64_t get_kernel_module(const char* name)
    {
        auto to_lower = [](char* string) -> char*
        {
            for (char* pointer = string; *pointer != '\0'; pointer++)
            {
                *pointer = (char)(short)KLI_CACHED_CALL(tolower, *pointer);
            }

            return string;
        };

        PRTL_PROCESS_MODULES info = (PRTL_PROCESS_MODULES)get_system_information(11);

        if (!info) { return NULL; }

        for (uint64_t i = 0; i < info->NumberOfModules; i++)
        {
            RTL_PROCESS_MODULE_INFORMATION& module_info = info->Modules[i];

            if (KLI_CACHED_CALL(strcmp, to_lower((char*)module_info.FullPathName + module_info.OffsetToFileName), name) == 0)
            {
                void* address = module_info.ImageBase;

                KLI_CACHED_CALL(ExFreePoolWithTag, info, 'gkmd'); return (uint64_t)address;
            }
        }

        KLI_CACHED_CALL(ExFreePoolWithTag, info, 'gkmd'); return NULL;
    }

    uint64_t pattern_scan(uint64_t base, size_t range, const char* pattern, const char* mask)
    {
        auto check_mask = [](const char* base, const char* pattern, const char* mask) -> bool
        {
            for (; *mask; base++, pattern++, mask++)
            {
                if (*mask == 'x' && *base != *pattern) { return false; }
            }

            return true;
        };

        range -= strlen(mask);

        for (uintptr_t i = 0; i < range; i++)
        {
            if (check_mask((const char*)base + i, pattern, mask)) { return base + i; }
        }

        return NULL;
    }

    uint64_t pattern_scan(uint64_t base, const char* pattern, const char* mask)
    {
        PIMAGE_NT_HEADERS headers = (PIMAGE_NT_HEADERS)(base + ((PIMAGE_DOS_HEADER)base)->e_lfanew);

        PIMAGE_SECTION_HEADER sections = IMAGE_FIRST_SECTION(headers);

        for (uintptr_t i = 0; i < headers->FileHeader.NumberOfSections; i++)
        {
            PIMAGE_SECTION_HEADER section = &sections[i];

            if (section->Characteristics && IMAGE_SCN_MEM_EXECUTE)
            {
                uintptr_t buffer = pattern_scan(base + section->VirtualAddress, section->Misc.VirtualSize, pattern, mask);

                if (buffer) { return buffer; }
            }
        }

        return 0;
    }

    void* get_trampoline(void* pe)
    {
        auto dos = PIMAGE_DOS_HEADER(pe);
        auto nt = PIMAGE_NT_HEADERS((char*)pe + dos->e_lfanew);

        auto section_header = PIMAGE_SECTION_HEADER(nt + 1);
        auto section_count = nt->FileHeader.NumberOfSections;

        for (auto i = 0u; i < section_count; i++)
        {
            if (!(section_header[i].Characteristics & IMAGE_SCN_MEM_EXECUTE))
                continue;

            if (!(section_header[i].Characteristics & IMAGE_SCN_MEM_NOT_PAGED))
                continue;

            auto begin = (char*)pe + section_header[i].VirtualAddress;
            auto end = begin + section_header[i].SizeOfRawData;

            for (auto it = begin; it < end - 1; ++it)
            {
                if (*(USHORT*)it == 0xE1FF)
                    return it;
            }
        }


        return nullptr;
    }
}