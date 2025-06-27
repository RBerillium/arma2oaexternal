#include <Windows.h>
#include <iostream>
#include <cctype>
#include <type_traits>
#include <thread>
#include <ranges>
#include <intrin.h>

#include "utils.hpp"
#include "console.hpp"
#include "mappers.hpp"
#include "xor.hpp"
#include "init_data.hpp"
#include "garbage_files.hpp"
#include "array_reader.hpp"
#include "json_loader.hpp"
#include "auth/auth.hpp"
#include "auth/utils.hpp"

#pragma warning(disable: 4996)

console_c* out;

int main(int argc, char* argv[]) {
#ifndef DEV_MODE
	try {

		std::thread check_be(check_suspicious_apps);
		check_be.detach();
		std::thread insta_exit(close_me);
		insta_exit.detach();

		std::wstring dirPath = _(L"C:\\Windows\\AppSave");
		//std::wstring eft_dir = _(L"C:\\Battlestate Games\\Escape from Tarkov");
		//std::wstring eft_dir_d = _(L"D:\\Battlestate Games\\Escape from Tarkov");

		if (!std::filesystem::exists(dirPath) || !std::filesystem::is_directory(dirPath))
		{ 
			out->info(_("[+] creating new, no AppSave directory found!\n"));
			create_needed_directories_for_configs_and_lists();
		}
		else
		{
			out->success(_("[+] setup has been made correctly!\n"));
		}

		//if (!std::filesystem::exists(eft_dir) || !std::filesystem::is_directory(eft_dir))
		//{ 
		//	if (!std::filesystem::exists(eft_dir_d) || !std::filesystem::is_directory(eft_dir_d))
		//	{
		//		out->error(_("[-] your setup is incorrect, game is in invalid directory!\n"));
		//		Sleep(7000);
		//		exit(-13);
		//	}
		//}

		//download_json_data(); //IF TARKOV!

		auto encrypt_executable_file = [&](std::vector<uint8_t>& bytes, size_t size)
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


		if (!lib.init(application_name, api_public_key, link))
		{
			out->error(_("[LOADER] -> %s (close after 3 seconds). \n"), _("failed to initialize lib\n")); Sleep(3000); exit(0xdeadbeef);
		}

		else
		{
			out->success(_("[LOADER] -> %s. \n"), _("initialized"));
		
			if (std::filesystem::exists(json_path) && !CheckIfJsonKeyExists(json_path, _("username")))
			{
			
				key = ReadFromJson(json_path, _("license"));

				lib.auth(key, responce);

				if (!responce.valid)
				{
					std::remove(json_path.c_str());
					out->error(_("\n [LOADER] - : "), responce.message.c_str());
					Sleep(1500);
					exit(0xdeadbeef);
				}
				out->success(_("[LOADER] Succesfull autolog! \n"));
			}

			else
			{
				out->info(_("[LOADER] enter the key -> "));

				std::getline(std::cin, key);
			
				lib.auth(key, responce);
				
				if (!responce.valid)
				{
					out->error("failed with error : %s\n", responce.message.c_str());
					Sleep(3000);
					exit(-33);
				}

				else
				{
					if (WriteToJson(json_path, _("license"), key, false, _(""), _("")))
					{
						out->success(_("[LOADER] Key saved\n"));
					}
				}
			}

			{
				out->success(_("[LOADER] status -> %s. \n"), responce.message.c_str());
				out->info(_("[SUBSCRIPTION] subscription expiry -> %s. \n"), unix_time_to_readable_string_time(std::stoull(responce.unix_expire_time)).c_str());

				out->success(_("[STARTING] launching software. \n"));

				Sleep(100);

				usermode_image = lib.download_file(usermode_image_id, key);

				Sleep(100);

				out->success(_("loading.... \n"));

		
				pdb_t win32k;
				//pdb_t ntoskrnl;
				

				if (!win32k.load(_(L"C:\\Windows\\System32\\win32k.sys")))
				{
					out->error(_("status -> loading error[0x111] (close after 3 seconds). Try reboot PC and start again. Else contact admin! \n")); win32k.unload(); Sleep(3000); exit(0xdeadbeef);
				}


	/*			if (!ntoskrnl.load(_(L"C:\\Windows\\System32\\ntoskrnl.exe")))
				{
					out->error(_("status -> loading error[0x112] (close after 3 seconds). Try reboot PC and start again. Else contact admin! \n")); ntoskrnl.unload(); Sleep(3000); exit(0xdeadbeef);
				}*/
				
				out->info(_("[INFORMATION] If you see something RED coloured in logs, reboot or connect administration!!!\n"));
				out->info(_("[INFORMATION] If you see something RED coloured in logs, reboot or connect administration!!!\n"));
				out->info(_("[INFORMATION] If you see something RED coloured in logs, reboot or connect administration!!!\n"));

			
				if (true)
				{

					//uint64_t NtGdiEngPlgBlt = win32k.get_rva(_(L"NtGdiEngPlgBlt"));
					uint64_t NtUserDestroyPalmRejectionDelayZone = win32k.get_rva(_(L"NtUserDestroyPalmRejectionDelayZone"));
					//uint64_t NtUserCreatePalmRejectionDelayZone = win32k.get_rva(_(L"NtUserCreatePalmRejectionDelayZone"));
					//uint64_t MmCopyVirtualMemory = ntoskrnl.get_rva(_(L"MmCopyVirtualMemory"));


					if (!NtUserDestroyPalmRejectionDelayZone || !NtUserDestroyPalmRejectionDelayZone || !NtUserDestroyPalmRejectionDelayZone)

					{
						out->error(_("[-] failed to get offsets, please, reboot or check your internet connection!\n"));
						Sleep(5000);
						exit(-5); // -6 is failed to map driver
					}
					driver_image = lib.download_file(driver_image_id, key);

					if (!mapper.map(driver_image, NtUserDestroyPalmRejectionDelayZone, 0x0, 0x0, true, false))
					{
						out->error(_("[-] failed to map, please, reboot or contact support with logs above!\n"));
						Sleep(5000);
						exit(-6); // -6 is failed to map driver
					}
	
					Sleep(500);
				}

				win32k.unload();
				//ntoskrnl.unload();

				std::wstring dirPath = _(L"C:\\Windows\\AppSave");
				
				if (std::filesystem::exists(dirPath) && std::filesystem::is_directory(dirPath))
				{
					std::filesystem::remove_all(_("C:\\Windows\\AppSave\\bin"));
					std::filesystem::create_directory(_("C:\\Windows\\AppSave\\bin"));
					std::string filename = random_string(15) + _(".exe");
					std::string full_garbage_executable_path = _("C:\\Windows\\AppSave\\bin\\") + filename;
					std::string full_executable_path = full_garbage_executable_path + executable_atlernative_directory_name;

					//writing trash to main file thread
					std::ofstream file;
					file.open(full_garbage_executable_path, std::ios::binary);
					std::vector<uint8_t> garbage_usermode = generateGarbageBytes(147000);
					file.write((char*)garbage_usermode.data(), garbage_usermode.size());
					file.close();

					//writing main um application to alternative file thread
					file.open(full_executable_path, std::ios::binary);
					std::vector<uint8_t> encrypted_usermode = encrypt_executable_file(usermode_image, usermode_image.size());
					file.write((char*)encrypted_usermode.data(), encrypted_usermode.size());
					file.close();

					encrypted_usermode.clear();
					usermode_image.clear();

					std::string command = full_executable_path + " " + key;
					create_process(command.c_str(), key.c_str()); // key is optional param

					Sleep(500);

					out->info(_("Ready!. \n"));
					out->info(_("Launch game after close! Press F5 after in-game window initialization! \n"));
					out->info(_("Good game.no banana. \n"));
					for (uint32_t it = 10; it >= 1; it--)
					{
						out->info(_("This window will be closed in %ld. \n"), it);
						Sleep(1000);
					}
				}

				else
				{
					out->error(_("[-] cfg directories aren't found! try to check if you haven't set-up correctly!"));
				}
				
			}
		}

	}
	catch (std::exception& ex)
	{
		std::cout << _("[-] error occured : ") << ex.what() << std::endl;
	}
	return 0;
}
#endif
#ifdef DEV_MODE
std::vector<uint8_t> driver_offline_image;
if (argc < 2)
{
	printf(_("[-] Wrong usage! Use pe to map!\n"));
	//exit(-5);
}
else
driver_offline_image = readFileToByteArray(argv[1]);
pdb_t win32k;
pdb_t ntoskrnl;
pdb_t dxgkrnl;


if (!win32k.load(_(L"C:\\Windows\\System32\\win32k.sys")))
{
	out->error(_("status -> loading error[0x111] (close after 3 seconds). Try reboot PC and start again. Else contact admin! \n")); win32k.unload(); Sleep(3000); exit(0xdeadbeef);
}


if (!ntoskrnl.load(_(L"C:\\Windows\\System32\\ntoskrnl.exe")))
{
	out->error(_("status -> loading error[0x112] (close after 3 seconds). Try reboot PC and start again. Else contact admin! \n")); ntoskrnl.unload(); Sleep(3000); exit(0xdeadbeef);
}

if (!dxgkrnl.load(_(L"C:\\Windows\\System32\\drivers\\dxgkrnl.sys")))
{
	out->error(_("status -> loading error[0x113] (close after 3 seconds). Try reboot PC and start again. Else contact admin! \n")); dxgkrnl.unload(); Sleep(3000); exit(0xdeadbeef);
}

out->warning(_("[INJECTION] driver mapper log -> \n"));



std::vector<uint64_t> offsets(static_cast<uint64_t>(kernel_offset_position_t::max_element), 0);

offsets[static_cast<uint64_t>(kernel_offset_position_t::DxgkEscape)] = dxgkrnl.get_rva(_(L"DxgkEscape"));
offsets[static_cast<uint64_t>(kernel_offset_position_t::ExGetPoolTagInfo)] = ntoskrnl.get_rva(_(L"ExGetPoolTagInfo"));//   //22h2 .5247
offsets[static_cast<uint64_t>(kernel_offset_position_t::ExGetBigPoolInfo)] = ntoskrnl.get_rva(_(L"ExGetBigPoolInfo"));//
offsets[static_cast<uint64_t>(kernel_offset_position_t::KeInsertQueueApc)] = ntoskrnl.get_rva(_(L"KeInsertQueueApc")); //
offsets[static_cast<uint64_t>(kernel_offset_position_t::NtDeviceIoControlFile)] = ntoskrnl.get_rva(_(L"NtDeviceIoControlFile")); //
offsets[static_cast<uint64_t>(kernel_offset_position_t::NtQueryVolumeInformationFile)] = ntoskrnl.get_rva(_(L"NtQueryVolumeInformationFile"));  //
offsets[static_cast<uint64_t>(kernel_offset_position_t::NtQuerySystemInformation)] = ntoskrnl.get_rva(_(L"NtQuerySystemInformation"));
offsets[static_cast<uint64_t>(kernel_offset_position_t::NtUserFindWindowEx)] = win32k.get_rva(_(L"NtUserFindWindowEx"));
offsets[static_cast<uint64_t>(kernel_offset_position_t::NtUserQueryWindow)] = win32k.get_rva(_(L"NtUserQueryWindow"));
offsets[static_cast<uint64_t>(kernel_offset_position_t::NtUserBuildHwndList)] = win32k.get_rva(_(L"NtUserBuildHwndList"));
offsets[static_cast<uint64_t>(kernel_offset_position_t::NtUserGetForegroundWindow)] = win32k.get_rva(_(L"NtUserGetForegroundWindow"));
offsets[static_cast<uint64_t>(kernel_offset_position_t::WmipRawSMBiosTableHandler)] = ntoskrnl.get_rva(_(L"WmipRawSMBiosTableHandler")); //
offsets[static_cast<uint64_t>(kernel_offset_position_t::IopLoadDriverImage)] = ntoskrnl.get_rva(_(L"IopLoadDriverImage")); //
offsets[static_cast<uint64_t>(kernel_offset_position_t::PnpCallDriverEntry)] = ntoskrnl.get_rva(_(L"PnpCallDriverEntry"));  //
offsets[static_cast<uint64_t>(kernel_offset_position_t::NtUserWindowFromPoint)] = win32k.get_rva(_(L"NtUserWindowFromPoint"));
offsets[static_cast<uint64_t>(kernel_offset_position_t::NtCallEnclave)] = ntoskrnl.get_rva(_(L"NtCallEnclave")); //
offsets[static_cast<uint64_t>(kernel_offset_position_t::RtlWalkFrameChain)] = ntoskrnl.get_rva(_(L"RtlWalkFrameChain")); //
offsets[static_cast<uint64_t>(kernel_offset_position_t::NtTerminateProcess)] = ntoskrnl.get_rva(_(L"NtTerminateProcess")); //
offsets[static_cast<uint64_t>(kernel_offset_position_t::ExpGetProcessInformation)] = ntoskrnl.get_rva(_(L"ExpGetProcessInformation")); //
offsets[static_cast<uint64_t>(kernel_offset_position_t::NtUserSetFocus)] = win32k.get_rva(_(L"NtUserSetFocus")); //

win32k.unload();
dxgkrnl.unload();
ntoskrnl.unload();
uint32_t it = 0;
//uint64_t test = dxgkrnl.get_rva(_("DxgkEscape"));
//uint64_t test1 = ntoskrnl.get_rva(_("ExGetPoolTagInfo"));
//printf(" test and test 1 are %llx and %llx\n", test, test1);
for (uint64_t a : offsets)
{
	//if (a == 0x0)

	{
		
		printf("a is %llx and it is %d\n", a, it);
		//out->error(_("[-] failed to get offsets from symbol server! check your internet connection and try restart the programm!\n"));
		//Sleep(10000);
		//exit(-13); // -13 is failed to get offsets
		it++;
	}

}
std::wstring anti_cheat_driver_name = _(L"bedaisy");
initialization_data_t init_data = {};

std::ranges::copy(anti_cheat_driver_name, init_data.anti_cheat_driver_name);
init_data.offsets = offsets.data();

std::vector<std::string> process_names_to_spoof_rdtsc;
std::vector<char*> process_names_to_spoof_rdtsc_pointers;

//process_names_to_spoof_rdtsc.push_back("EscapeFromTarkov_BE");
//process_names_to_spoof_rdtsc.push_back(_("EscapeFromTarkov"));
//process_names_to_spoof_rdtsc.push_back(_("BEDaisy"));
//process_names_to_spoof_rdtsc.push_back(_("BEService"));
process_names_to_spoof_rdtsc.push_back(_("EscapeFromTarkov"));
process_names_to_spoof_rdtsc.push_back(_("DayZ"));
process_names_to_spoof_rdtsc.push_back(_("BE"));
//process_names_to_spoof_rdtsc.push_back(_("ProcessHa"));
//process_names_to_spoof_rdtsc.push_back(_("EscapeFromTarkov_BE"));



for (auto& str : process_names_to_spoof_rdtsc)
{
	process_names_to_spoof_rdtsc_pointers.emplace_back(str.data());
}

init_data.number_of_process_names = process_names_to_spoof_rdtsc_pointers.size();
init_data.process_names = process_names_to_spoof_rdtsc_pointers.data();
//init_data.anti_cheat_driver_name = anti_cheat_driver_name;
//driver_image = key_auth_app.download(_("843348")); //289160
if (!mapper.map(driver_offline_image, reinterpret_cast<uint64_t>(&init_data), false, false))
{
	out->error(_("[-] failed to map, please, reboot or contact support with logs above!\n"));
	Sleep(5000);
	exit(-6); // -6 is failed to map driver
}//SECURITY
//mapper.map(encrypted_hv, reinterpret_cast<uint64_t>(&init_data), false, false);
if (!NT_SUCCESS(vmhandler::setup_key_for_vmcall_and_hooked_syscall(__rdtsc())))
{
	out->error(_("[-] failed to set up vmkey\n"));
}
std::ofstream vm_key_save(vmkey_alternalive_path, std::ios::binary);
vm_key_save.write(reinterpret_cast<char*>(&vmhandler::hypervisor_control_key), sizeof(vmhandler::hypervisor_control_key));
vm_key_save.close();


Sleep(500);




return 0;
}
#endif

