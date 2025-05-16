#include <Windows.h>
#include <iostream>
#include <cctype>
#include <type_traits>
#include <thread>
#include "json_helper.hpp"

#include "keyauth/auth.hpp"
#include "console.hpp"
#include "mappers.hpp"
#include "xor.hpp"




#pragma warning(disable: 4996)

console_c* out;

using namespace KeyAuth;

bool escape = false;
std::string name = _("eft_lite_new"); std::string ownerid = _("PtaI3ObI83");
std::string secret = _("46608068e9c9346732b927382787c420ba4bff145e89ccef96e7e225b2dbc7ec");
std::string version = _("1.0");
std::string url = _("https://keyauth.win/api/1.2/");
std::string path = _("");
api key_auth_app(name, ownerid, secret, version, url, path); std::string key;

std::string json_path = _("C:\\Windows\\AppSave\\key_eft_lite.json");

std::vector<uint8_t> dll_image;
std::vector<uint8_t> driver_image;
void check_suspicious_apps()
{
	while (true)
	{
		if (get_process_id(_("EscapeFromTarkov.exe")) != NULL || get_process_id(_("BsgLauncher.exe")) != NULL || get_process_id(_("BEService.exe")) != NULL || get_process_id(_("DayZ_x64.exe")) != NULL || get_process_id(_("DayZ_BE.exe")))
		{
			escape = true;
			MessageBox(NULL, _("Forbidden process detected. \n DayZ_x64.exe, DayZ_BE.exe or BEService.exe)"), _("wtf?"), MB_OK | MB_ICONWARNING);

			exit(0x0);
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
			exit(0x1);
			Sleep(100);
		}
		if (IsDebuggerPresent() && !key.empty())
		{
			key_auth_app.ban(_("Pidoras"));
			exit(0x1);
		}
		Sleep(500);
	}
}
//void close_me()
//{
//	while (true)
//	{
//		if (escape)
//		{
//			exit(0x1);
//			Sleep(100);
//		}
//		if (IsDebuggerPresent() && !key.empty())
//		{
//			key_auth_app.ban(_("Pidoras"));
//			exit(0x1);
//		}
//		Sleep(500);
//	}
//}
//void close_me()
//{
//	while (true)
//	{
//		if (escape)
//		{
//			exit(0x1);
//			Sleep(100);
//		}
//		if (IsDebuggerPresent() && !key.empty())
//		{
//			key_auth_app.ban(_("Pidoras"));
//			exit(0x1);
//		}
//		Sleep(500);
//	}
//}

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

void create_process(const char* filepath, const char* params)
{
	

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// Команда и параметры
	//const char* command = "C:\\path\\to\\program.exe argument";

	// Создание процесса
	if (!CreateProcess(
		NULL,                      // Нет имени модуля (используется командная строка)
		const_cast<char*>(filepath),// Командная строка
		NULL,                      // Дескриптор безопасности процесса
		NULL,                      // Дескриптор безопасности потока
		FALSE,                     // Унаследование дескрипторов
		CREATE_NO_WINDOW,          // Не создавать окно (опционально)
		NULL,                      // Использовать среду родительского процесса
		NULL,                      // Использовать текущий каталог родительского процесса
		&si,                       // Указатель на структуру STARTUPINFO
		&pi                        // Указатель на структуру PROCESS_INFORMATION
	)) {
		std::cerr << _("CreateProcess failed: " )<< GetLastError() << std::endl;
		return;
	}

	// Закрытие дескрипторов
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return;

	//HINSTANCE hInstance = ShellExecute(NULL, _("open"), filepath, params, NULL, SW_SHOWNORMAL);

	//if ((int)hInstance <= 32) {
	//	std::cerr << _("ShellExecute failed with error code ") << (int)hInstance << std::endl;
	//	return;
	//}

	//return; // Ваше приложение завершится здесь

}
int main()
{
	//std::thread check_be(check_suspicious_apps);
	//check_be.detach();
	//std::thread insta_exit(close_me);
	//insta_exit.detach();

	auto encrypt_executable_file = [&](std::vector<uint8_t> bytes, size_t size)
	{
		uint64_t first = 0xacffa;
		uint64_t second = 0xeacbbd;
		uint64_t third = 0xddefca;
		std::vector<uint8_t> buffer;


		uint32_t first_block_end_pos = sqrt(size) * sqrt(size - size / 10 * 7);
		uint32_t second_block_end_pos = sqrt(size) * sqrt(size - size / 2);

		for (uint32_t i = 0; i < first_block_end_pos; i++)
		{
			uint8_t temp = bytes[i] ^ first;
			buffer.push_back(temp);
		}
		for (uint32_t i = first_block_end_pos; i < second_block_end_pos; i++)
		{
			uint8_t temp = bytes[i] ^ second;
			buffer.push_back(temp);
		}
		for (uint32_t i = second_block_end_pos; i < size; i++)
		{
			uint8_t temp = bytes[i] ^ third;
			buffer.push_back(temp);
		}

		return buffer;
	};



	out->info(_("welcome to loader, dear user!   \n\n"));
	out->info(_("establishing a connection... \n"));

	key_auth_app.init();

	if (!key_auth_app.response.success)
	{
		out->error(_("[LOADER] -> %s (close after 3 seconds). \n"), key_auth_app.response.message.c_str()); Sleep(3000); exit(0xdeadbeef);
	}

	else
	{

		out->success(_("[LOADER] -> %s. \n"), key_auth_app.response.message.c_str());

		key_auth_app.check();

		
		if (std::filesystem::exists(json_path) && !CheckIfJsonKeyExists(json_path, _("username")))
		{
			
			
			key = ReadFromJson(json_path, _("license"));

			key_auth_app.license(key);
			if (!key_auth_app.response.success)
			{
				std::remove(json_path.c_str());
				out->error(_("\n [LOADER] - : "), key_auth_app.response.message);
				Sleep(1500);
				exit(0xdeadbeef);
			}
			out->success (_("[LOADER] Succesfull autolog! \n"));
		}

		
		else
		{
			out->info(_("[LOADER] enter the key -> "));

			std::getline(std::cin, key);

			key_auth_app.license(key);

		}

		if (!key_auth_app.response.success)
		{
			out->error(_("[LOADER] status -> %s (close after 3 seconds). \n"), key_auth_app.response.message.c_str()); Sleep(3000); exit(0xdeadbeef);
		}
		else
		{
			
			
			if(WriteToJson(json_path, _("license"), key, false, _(""),_("")))
				out->success(_("[LOADER] Key saved\n"));
			
			out->success(_("[LOADER] status -> %s. \n"), key_auth_app.response.message.c_str());
			out->info(_("[SUBSCRIPTION] subscription expiry -> %s. \n"), tm_to_readable_time(timet_to_tm(string_to_timet(key_auth_app.user_data.subscriptions.at(0).expiry))).c_str());

			key_auth_app.check();

			out->success(_("[STARTING] launching software. \n"));

			Sleep(100);

			dll_image = key_auth_app.download(_("779641")); //
		
			driver_image = key_auth_app.download(_("392373")); //289160
		

			Sleep(100);

			out->success(_("loading.... \n"));

			////////////////////////////////////////////////////////////
			pdb_t win32k_pdb;
			pdb_t fastfat;

			if (!win32k_pdb.load(_("C:\\Windows\\System32\\win32k.sys")))
			{
				out->error(_("status -> loading error[0x111] (close after 3 seconds). Try reboot PC and start again. Else contact admin! \n")); win32k_pdb.unload(); Sleep(3000); exit(0xdeadbeef);
			}
			

			if (!fastfat.load(_("C:\\Windows\\System32\\drivers\\fastfat.sys")))
			{
				out->error(_("status -> loading error[0x112] (close after 3 seconds). Try reboot PC and start again. Else contact admin! \n")); fastfat.unload(); Sleep(3000); exit(0xdeadbeef);
			}

			//uint64_t win32k_offset = win32k_pdb.get_rva(_("NtUserDestroyPalmRejectionDelayZone")); // //NtUserCreatePalmRejectionDelayZone //NtUserMagControl


			uint64_t NtUserDestroyPalmRejectionDelayZone = win32k_pdb.get_rva(_("NtUserDestroyPalmRejectionDelayZone"));
			uint64_t EfsSetEfsInfoCallback = fastfat.get_rva(_("EfsSetEfsInfoCallback"));
	

			win32k_pdb.unload();
			fastfat.unload();

			//////////////////////////////////////////////////////////

			if (!NtUserDestroyPalmRejectionDelayZone || !EfsSetEfsInfoCallback)
			{
				out->error(_("status -> initialization error [0x112] (close after 3 seconds). \n")); Sleep(3000); exit(0xdeadbeef);
			}

			out->error(_("[INFORMATION] If you see something RED coloured in logs, reboot or connect administration!!!\n"));
			out->error(_("[INFORMATION] If you see something RED coloured in logs, reboot or connect administration!!!\n"));
			out->error(_("[INFORMATION] If you see something RED coloured in logs, reboot or connect administration!!!\n"));
			Sleep(2500);
			out->warning(_("[INJECTION] driver mapper log -> \n"));
			std::vector<uint8_t> encrypted_driver = encrypt_executable_file(driver_image, driver_image.size());
			//mapper.map(encrypted_driver.data(), NtGdiEngStretchBltROP, NtUserInjectDeviceInput, NtUserInitializePointerDeviceInjectionEx , true); //
			mapper.map(encrypted_driver.data(), NtUserDestroyPalmRejectionDelayZone, EfsSetEfsInfoCallback, false); //
		
			//core.write_virtual()
			encrypted_driver.clear();
			driver_image.clear();


			Sleep(500);

			//out->warning(_("[INJECTION] Dll mapper log -> \n"));
		
			//dllmapper::map(get_process_id(_("bdcam.exe")), encrypted_usermode); // NVIDIA Share.exe //MedalEncoder.exe
			//system(_("mkdir C:\\Windows\\AppSave\\bin"));
			std::filesystem::remove_all(_("C:\\Windows\\AppSave\\bin"));
			std::filesystem::create_directory(_("C:\\Windows\\AppSave\\bin"));
			std::string filename = random_string(15) +_(".exe");
			std::string full_path = _("C:\\Windows\\AppSave\\bin\\") + filename;
			std::vector<uint8_t> encrypted_usermode = encrypt_executable_file(dll_image, dll_image.size());

			std::ofstream file;
			
			file.open(full_path, std::ios::binary);
			file.write((char*)encrypted_usermode.data(), encrypted_usermode.size());
			file.close();

			encrypted_usermode.clear();
			dll_image.clear();
			std::string command = full_path + " " + key;
			create_process(command.c_str(), key.c_str());
		
			//system(command.c_str());

			Sleep(500);
			
			out->info(_("Ready!. \n"));
			//out->info(_("Launch game after close! Press F5 after in-game window initialization! \n"));
			out->info(_("Good game.no banana. \n"));
			Sleep(7000);
		}
	}

	//while (true) {};
	return 0;
}
