#pragma once

#include <iostream>
#include <ctime>
#include <TlHelp32.h>
#include <Windows.h>
#include <memory>

#include "../render/xor.h"

response_t responce;
authlib lib;
std::string application_name = xor ("arma2oa");
std::string api_public_key = xor ("624a1831398676bc9c1affed3c309911ce6386f7fd9ca2420833a22d171cab9d");
std::string link = xor ("https://moonshinesolutions.ru");
std::string key = "";



bool escape = false;

enum class custom_exit_code : int
{
	SUCCESS = 0,
	FAILED_TO_INIT = -2,
	FAILED_HWID = -3,
	FAILED_KEY_INVALID = -4,
	FORBIDDEN_PROCESS_DETECTED = -5,
	BAN_KEY = -6,
	UNCONDITIONAL_EXIT = -7
};

namespace utils
{
    std::string unix_time_to_readable_string_time(const time_t unix_time) {

        std::tm tm_buff{};
        localtime_s(&tm_buff, &unix_time);

        char buffer[64];
        //std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm_buff);
        std::strftime(buffer, sizeof(buffer), _("%d-%m-%Y %H:%M"), &tm_buff);

        return std::string(buffer);
    }

	uint32_t get_process_id(std::wstring process_name)
	{
		PROCESSENTRY32 process{ sizeof(PROCESSENTRY32) };

		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

		if (!Process32First(snapshot, &process))
		{
			return 0;
		}

		uint32_t process_identifier = 0;

		do
		{
			if (std::wstring(process.szExeFile) == process_name)
			{
				process_identifier = process.th32ProcessID;
				break;
			}
		} while (Process32Next(snapshot, &process));

		CloseHandle(snapshot);

		return process_identifier;
	}
}

std::string random_string(size_t size)
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

void check_suspicious_apps()
{
	while (true)
	{
		if (utils::get_process_id(_(L"EscapeFromTarkov.exe")) != NULL || utils::get_process_id(_(L"BsgLauncher.exe")) != NULL || utils::get_process_id(_(L"BEService.exe")) != NULL || utils::get_process_id(_(L"DayZ_x64.exe")) != NULL || utils::get_process_id(_(L"DayZ_BE.exe")))
		{
			escape = true;
			MessageBox(NULL, _(L"Forbidden process detected. \n DayZ_x64.exe, DayZ_BE.exe or BEService.exe)"), _(L"wtf?"), MB_OK | MB_ICONWARNING);

			exit(static_cast<int>(custom_exit_code::FORBIDDEN_PROCESS_DETECTED));
		}
		Sleep(300);
	}
}

void close_me()
{
	while (true)
	{
		if (escape)
		{
			exit(static_cast<int>(custom_exit_code::UNCONDITIONAL_EXIT));
			Sleep(100);
		}
		if (IsDebuggerPresent() && lib.is_authorized())
		{
			bool banned = false;
			while (!banned)
			{
				banned = lib.ban_key(key);
			}
			exit(static_cast<int>(custom_exit_code::BAN_KEY));
		}
		Sleep(500);
	}
}

