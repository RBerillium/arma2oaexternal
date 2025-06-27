#include <iostream>
#include <stdint.h>
#include <windows.h>
#include <tlhelp32.h>
#include <string.h>
#include "render/hijack.h"
#include "driver/kernel.h"
#include "sdk.h"
#include "render/xor.h"
#include "vector2.hpp"
#include "auth/auth.hpp"
#include "auth/utils.hpp"
#include "menu.h"

#include <intrin.h>
std::string soldier = xor("soldier");






namespace s = settings;

void Text(float X, float Y, ImColor Color, const char* const Format, ...)
{
	va_list List;
	char Text[1024] = { 0 };
	va_start(List, Format);
	_vsnprintf_s(Text, sizeof(Text), Format, List);
	va_end(List);

	ImGui::GetForegroundDrawList()->AddText(ImVec2(X - ImGui::CalcTextSize(Text).x / 2, Y), Color, Text);

	ImGui::GetBackgroundDrawList()->AddText(ImVec2(X - ImGui::CalcTextSize(Text).x / 2 + 1, Y + 1), IM_COL32_BLACK, Text);
	ImGui::GetBackgroundDrawList()->AddText(ImVec2(X - ImGui::CalcTextSize(Text).x / 2 - 1, Y - 1), IM_COL32_BLACK, Text);
	ImGui::GetBackgroundDrawList()->AddText(ImVec2(X - ImGui::CalcTextSize(Text).x / 2 + 1, Y - 1), IM_COL32_BLACK, Text);
	ImGui::GetBackgroundDrawList()->AddText(ImVec2(X - ImGui::CalcTextSize(Text).x / 2 - 1, Y + 1), IM_COL32_BLACK, Text);
	ImGui::GetBackgroundDrawList()->AddText(ImVec2(X - ImGui::CalcTextSize(Text).x / 2 + 1, Y), IM_COL32_BLACK, Text);
	ImGui::GetBackgroundDrawList()->AddText(ImVec2(X - ImGui::CalcTextSize(Text).x / 2 - 1, Y), IM_COL32_BLACK, Text);
	ImGui::GetBackgroundDrawList()->AddText(ImVec2(X - ImGui::CalcTextSize(Text).x / 2, Y + 1), IM_COL32_BLACK, Text);
	ImGui::GetBackgroundDrawList()->AddText(ImVec2(X - ImGui::CalcTextSize(Text).x / 2, Y - 1), IM_COL32_BLACK, Text);
}

void Render()
{

	Menu pMenu;
	pMenu.AddUselessItem(xor ("ESP"));
	pMenu.AddBooleanItem(xor ("Players"), &s::players_esp);
	pMenu.AddBooleanItem(xor ("Bots"), &s::show_bots);
	pMenu.AddBooleanItem(xor ("Vehicles"), &s::vehicles_esp);
	pMenu.AddBooleanItem(xor ("Hide empty vehicles"), &s::hide_empty_vehicles);
	pMenu.AddUselessItem(xor ("Misc"));
	pMenu.AddBooleanItem(xor ("Teleport"), &s::teleport);
	pMenu.AddUselessItem(xor ("Aim (AIM KEY - LALT)"));
	pMenu.AddBooleanItem(xor ("Aimbot"), &s::aimbot);
	pMenu.AddBooleanItem(xor ("Vehicles aim"), &s::vehicles_aimbot);
	pMenu.AddFloatItem(xor ("Fov"), 0.f, 500.f, 10.f, &s::fov);
	pMenu.AddUselessItem(xor ("Distance"));
	pMenu.AddFloatItem(xor ("Players"), 0, 20000.f, 500.f, &s::players_distance);
	pMenu.AddFloatItem(xor ("Vehicles"), 0, 20000.f, 500.f, &s::vehicles_distance);
	pMenu.AddFloatItem(xor ("Aimbot"), 0, 20000.f, 500.f, &s::aim_distance);
	pMenu.AddUselessItem(xor ("============="));
	pMenu.AddUselessItem(xor ("INFO"));
	pMenu.AddUselessItem(xor ("END - Close software"));
	pMenu.AddUselessItem(xor ("F1 - print local coords"));
	pMenu.AddUselessItem(xor ("F2 - print all coords"));
	pMenu.AddUselessItem(xor ("F9 - repair car (use in vehicle only!)"));
	pMenu.AddUselessItem(xor ("F10 - teleport to coords (console)"));
	pMenu.AddUselessItem(xor ("F11 - night vision"));
	pMenu.AddUselessItem(xor ("============="));

	pMenu.SetIndex(1);

	bool IsActive = true;

	//pMenu.AddBooleanItem(xor(""))
	while (true)
	{
		if (GetAsyncKeyState(VK_END))
		{
			for (int i = 0; i < 5; i++)
			{
				Nvidia::start();
				Nvidia::end();
				
			}
			exit(0);
		}
		Nvidia::start();
		if (IsActive)
		{
			pMenu.Navigation();

			ImGui::SetNextWindowSize(ImVec2(280, NULL));
		

			ImGui::SetNextWindowPos(ImVec2(50, 50));

			ImGui::SetNextWindowBgAlpha(0.6f);

			//ImGui::GetBackgroundDrawList()->AddImage(Image, ImVec2(50, 50),ImVec2(370,700));
		
			ImGui::Begin(xor ("##Menu"), NULL,/* ImGuiWindowFlags_AlwaysAutoResize |*/ ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

			//ImGui::Image(Image, ImVec2(320, 650))

			pMenu.MenuLoop();



			ImGui::End();
		}
		if (GetAsyncKeyState(VK_INSERT))
		{
			IsActive = !IsActive;
			Sleep(150);
		}
		//Text(globals::screen_x / 2 - 50, 50, ImColor(255, 0, 0), xor ("Arma2 dollari"));


		
		if (s::players_esp) {
			for (player_t player : safe_get_players())
			{
				if (!s::show_bots)
				{
					if (player.is_bot)
						continue;
				}
				Vector3 coords = player.location;
				float distance = coords.Distance(globals::local_coords);

				

				Vector3 screen = w2s(coords);



				if (screen.z > 0.f && distance <= s::players_distance)
				{	
					Text(screen.x, screen.y, player.color, xor ("%s [%0.f]"), player.name.c_str(), distance);
					if (s::aimbot)
					{
						if (GetAsyncKeyState(VK_LMENU))
						{


							ImGui::GetForegroundDrawList()->AddCircle(ImVec2(s::screen_x / 2, s::screen_y / 2), s::fov, ImColor(0, 255, 0), 36);

							vec2 vec2_screen = { screen.x, screen.y };
							if (vec2_screen.distance({ s::screen_x / 2,s::screen_y / 2 }) <= s::fov && distance <= s::aim_distance) //&& distance <= aim_distance
							{
								Text(screen.x, screen.y, ImColor(0, 255, 0), xor ("%s [%0.f]"),  player.name.c_str(), distance);
								aimbot_func(player.entity_ptr);
							}
						}
					}

					else if (s::teleport)

					{
						if (GetAsyncKeyState(VK_LMENU))
						{


							ImGui::GetForegroundDrawList()->AddCircle(ImVec2(s::screen_x / 2, s::screen_y / 2), s::fov, ImColor(0, 255, 0), 16);

							vec2 vec2_screen = { screen.x, screen.y };
							if (GetAsyncKeyState(VK_RBUTTON)) {

								if (vec2_screen.distance({ s::screen_x / 2, s::screen_y / 2 }) <= s::fov)
								{
									set_local_position(player.location);
									razyeb(player.location);
									s::teleport = false;
								}
							}
						}
							
					}

				}

			}
		}
		
		if (GetAsyncKeyState(VK_F9)) {
		
			heal_me();
			Sleep(100);
		}
		//heal_local();
		if (s::vehicles_esp) {
			for (vehicle_t vehicle : safe_get_vehicles())
			{
				//DWORD entity = players[i];
				if (s::hide_empty_vehicles)
				{
					if (vehicle.empty)
						continue;
				}
				Vector3 coords = vehicle.location;
				float distance = coords.Distance(globals::local_coords);
				Vector3 screen = w2s(coords);

				if (screen.z > 0.0f && distance <= s::vehicles_distance)
				{
					Text(screen.x, screen.y, vehicle.color, xor ("%s [%0.f]"), vehicle.name.c_str(), distance);

					if (s::vehicles_aimbot)
					{
						if (GetAsyncKeyState(VK_LMENU))
						{

							
							ImGui::GetForegroundDrawList()->AddCircle(ImVec2(s::screen_x / 2, s::screen_y / 2), s::fov, ImColor(0, 255, 0), 16);

							vec2 vec2_screen = { screen.x, screen.y };
							if (vec2_screen.distance({ s::screen_x / 2, s::screen_y / 2 }) <= s::fov)
							{
								Text(screen.x, screen.y, ImColor(255, 0, 0), xor ("%s [%0.f]\n target!!!"), vehicle.name.c_str(), distance);
								aimbot_vehicles(vehicle.entity_ptr);
							}
						}
					}

					else if (s::teleport)

					{
						if (GetAsyncKeyState(VK_LMENU))
						{


							ImGui::GetForegroundDrawList()->AddCircle(ImVec2(s::screen_x, s::screen_y), s::fov, ImColor(0, 255, 0), 16);

							vec2 vec2_screen = { screen.x, screen.y };
							if (GetAsyncKeyState(VK_RBUTTON)) {

								if (vec2_screen.distance({ s::screen_x / 2, s::screen_y / 2 }) <= s::fov)
								{
									set_local_position(vehicle.location);
									razyeb(vehicle.location);
									s::teleport = false;
								}
							}
						}

					}
					

				}
			}
		}
		//heal_me();
		//auto a = w2s({ 5018.f, 33.f,  1851.f });
		//if (GetAsyncKeyState(VK_F3))
		//{
		//	players_esp = !players_esp;
		//	Sleep(100);
		//}

		//if (GetAsyncKeyState(VK_F4))
		//{
		//	vehicles_esp = !vehicles_esp;
		//	Sleep(100);
		//}

		//if (GetAsyncKeyState(VK_F5))
		//{
		//	aimbot = !aimbot;
		//	Sleep(100);
		//}

		//if (GetAsyncKeyState(VK_F6))	
		//{
		//	teleport = !teleport;
		//	if (teleport)
		//	{
		//		aimbot = false;
		//		vehicles_aimbot = false;
		//	}
		//	
		//	Sleep(100);
		//}

		//if (GetAsyncKeyState(VK_F7))
		//{
		//	s::aimbot = !s::aimbot;
		//	s::vehicles_aimbot = !s::vehicles_aimbot;
		//	Sleep(100);
		//}
		if (GetAsyncKeyState(VK_F10))
		{
			printf(xor ("enter coords\n"));
			float x, y, z;
			printf(xor ("enter x\n"));
			std::cin >> x;
			printf(xor ("enter y\n"));
			std::cin >> y;
			printf(xor ("enter z\n"));
			std::cin >> z;
			Vector3 coords{ x,y,z };
			set_local_position(coords);
		}
		if (GetAsyncKeyState(VK_F11))
		{
			s::night_vision = !s::night_vision;
			//vehicles_aimbot = !vehicles_aimbot;
			int_ptr local = Driver::read<int_ptr>(globals::world + camera_on_offset);
			local = Driver::read<int_ptr>(local + 0x4);

			if (s::night_vision)
			{

				Driver::write<bool>(local + night_vis, true);
			}
			else
				Driver::write<bool>(local + night_vis, false);
			Sleep(100);

		}
		
		if (GetAsyncKeyState(VK_F1))
		{
			//aimbot = !aimbot;
			auto coords = get_local_player_coords();
			printf(xor ("local coords are %0.f %0.f %0.f\n"), coords.x, coords.y, coords.z);
			Sleep(100);
		}

		if (GetAsyncKeyState(VK_F2))
		{
			settings::print_all_coords = !settings::print_all_coords;
			Sleep(100);
		}
		//Text(settings::screen_x -100,  50, ImColor(0, 255, 0), xor (" F1 - print local coords\n F2  - print all coords\n F3 - player esp\n F4 - vehicles esp \n F5 - aim\n F6 - teleport \n F7 - vehicles aimbot\n F9 heal car\n F10 teleport to coords \n F11 - night vision \n" ));
		//Text(50, 20, ImColor(0, 255, 0), xor("Aim is %d and tp is %d \n vehicles aim %d"), settings::aimbot, settings::teleport, settings::vehicles_aimbot); //_("Aim is %d and teleport is %d")
		f32 fps = ImGui::GetIO().Framerate;
		Nvidia::end();

		f32 ms = 1000.f / fps;

		if (ms < 7.f)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(7 - (u32)ms));
		}
	}
}
void print_all_coords_func()
{
	while (true)
	{
		if (settings::print_all_coords) {
			system(xor ("cls"));
			Vector3 local_coords = get_local_player_coords();
			printf(xor ("[+] local coords are : %0.f %0.f %0.f\n"), local_coords.x, local_coords.y, local_coords.z);
			for (auto it : safe_get_players())
			{
				printf(xor ("[+] player coords are: %0.f %0.f %0.f\n"), it.location.x, it.location.y, it.location.z);
			}

			for (auto it : safe_get_vehicles())
			{
				printf(xor ("[+] vehicle coords are: %0.f %0.f %0.f\n"), it.location.x, it.location.y, it.location.z);
			}
		
		}
		Sleep(500);
	}


}
void cache_all()
{
	while(true)
	{
		
		cache_globals();
		//cache_entities();
		cache_entities_test();
		
	}

}
int main(int argc, char* argv[])
{
#ifndef OFFLINE_MODE
	//if (!lib.init(application_name, api_public_key, link))
	//{
	//	exit(-55);
	//}
	//if (argc < 2)
	//{
	//	exit(0xdeadbeef);
	//}
	//else
	//{
	//	key = argv[1];
	//}

	//lib.auth(key, responce);

	//if (!responce.valid)
	//{	
	//	Sleep(1500);
	//	exit(0xdeadbeef);
	//}

#endif
	LoadLibraryA(xor("user32.dll")); HMODULE module_win32u = LoadLibraryA(xor("win32u.dll"));

	if (module_win32u)
	{
		*(PVOID*)&memory_system::function = GetProcAddress(module_win32u, xor("NtUserDestroyPalmRejectionDelayZone")); // for um 
		//*(PVOID*)&memory_system::function = GetProcAddress(module_win32u, _("NtUserMagControl"));
	}
	printf(_("comm addr is %llx\n"), &memory_system::function);
	settings::screen_x = GetSystemMetrics(SM_CXSCREEN); settings::screen_y = GetSystemMetrics(SM_CYSCREEN);
	//uint32_t pID = 10112;
	//uintptr_t pBase = 0xee0000;
	//ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, false, pID);
	u32 process_identifier = 0;
	while (!process_identifier)
	{
		process_identifier = Driver::get_process_by_name(xor(L"ArmA2OA.exe"));
	}

	// Sleep(5000);
	Driver::initialization(process_identifier);
	base_address = Driver::get_module(xor(L"ArmA2OA.exe"));
	//printf(_("addr base is %llx\n"), base_address);

	bool initialized = false;
	printf(xor("press F5 after in-game initialization"));
	while (!initialized)
	{
		if (GetAsyncKeyState(VK_F5))
		{
			initialized = true;
		
		}
		Sleep(100);
	}
	Nvidia::initialization();
	printf(xor("init success!\n"));
	RECT Desktop; HWND hDesktop = Nvidia::window;
	GetWindowRect(hDesktop, &Desktop); uint32_t x = Desktop.right; uint32_t y = Desktop.bottom;
	globals::screen_x = x;
	globals::screen_y = y;
	printf(xor("thread created!\n"));
	Sleep(3000);

	std::thread cache(cache_all);
	cache.detach();

	std::thread print(print_all_coords_func);
	print.detach();
	std::thread RenderThread(Render);
	RenderThread.join();
}

