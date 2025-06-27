#pragma once

#include "kdmapper/mapper.hpp"
#include "dllmapper/injector.hpp"
#include "xor.hpp"

mapper_t mapper;

namespace dllmapper
{
	int map(uint32_t process, std::vector<uint8_t> raw_image)
	{
		TOKEN_PRIVILEGES priv = { 0 };
		HANDLE hToken = NULL;
		if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		{
			priv.PrivilegeCount = 1;
			priv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

			if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &priv.Privileges[0].Luid))
				AdjustTokenPrivileges(hToken, FALSE, &priv, 0, NULL, NULL);

			CloseHandle(hToken);
		}

		HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process);
		if (!hProc) 
		{
			DWORD Err = GetLastError();
			//printf("OpenProcess failed: 0x%X\n", Err);
			system(_("PAUSE"));
			return -2;
		}

		printf(_("Injecting...\n"));

		if (!ManualMapDll(hProc, reinterpret_cast<BYTE*>(raw_image.data()), raw_image.size()))
		{
			CloseHandle(hProc);
			//printf("Error while mapping.\n");
			system(_("PAUSE"));
			return -8;
		}

		Sleep(3000);
		CloseHandle(hProc);
		//printf("OK\n");
		return 0;
	}
}