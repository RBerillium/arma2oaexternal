#pragma once
#include "driver/kernel.h"
#include "vector3.h"
#include "render/xor.h"
#include <mutex>
#define _CRT_SECURE_NO_WARNINGS
typedef  uint32_t int_ptr;
namespace Driver = memory_system;
uintptr_t base_address{};
uintptr_t world = 0xA8490C;
uintptr_t grass = 0x14F0;
uintptr_t entity_table_offset = 0x888; // 0x5FC; //888 - slow
uintptr_t table_size = 0x4;
uintptr_t entity_size = 0x34;
uintptr_t visual_state_offset = 0x18;
uintptr_t camera_on_offset = 0x13B4;//0x13B4; // 0x13B0 //13A8 // real player
uintptr_t transport_on_offset = 0x13A8; //0x13A8;
uintptr_t night_vis = 0xC61; // 0x0C15 0x0C16 0x0C17 0x0C18 - therm
// 0x930 - far
// 0x888 - near
// 0x7d8 - bullet
uintptr_t bullet_table = 0x548;
//helicopter //car //tank //motorcycle //airplane

//uintptr_t transformation_offset = 0xDD8A14; //A5E189
////// W2S values
uintptr_t P_Transformation{};
uintptr_t dwTransformations{};
uintptr_t dwTransData{};
Vector3 InvViewRight{};
Vector3 InvViewUp{};
Vector3 InvViewForward{};
Vector3 InvViewTranslation{};
Vector3 ViewPortMatrix{};
Vector3 ProjD1{};
Vector3 ProjD2{};

namespace settings
{
	bool print_all_coords = false;
	bool aimbot = false;
	bool vehicles_aimbot = false;
	bool players_esp = true;
	bool vehicles_esp = false;
	bool show_bots = false;
	f32 fov = 50.f;
	f32 screen_x = 1920.f;
	f32 screen_y = 1080.f;
	bool teleport = false;
	bool night_vision = false;

	bool hide_empty_vehicles = false;

	float players_distance = 20000.f;
	float vehicles_distance = 20000.f;
	float aim_distance = 20000.f;
}



////// W2S END


namespace globals
{
	float screen_x;
	float screen_y;
	int_ptr world;
	int_ptr dwTransData;

	int_ptr local_player;
	Vector3 local_coords;
}

Vector3 get_coords(int_ptr entity)
{
	int_ptr entity_ptr = Driver::read<int_ptr>(entity + 0x4);
	//printf(xor("local is %llx\n"), entity_ptr);
	int_ptr visual_state = Driver::read<int_ptr>(entity_ptr + visual_state_offset);
	//auto pos = Driver::read<Vector3>(visual_state + 0x28);
	//printf(xor ("pos is %0.1f %0.1f %0.1f\n"), pos.x, pos.y, pos.z);
	return Driver::read<Vector3>(visual_state + 0x28);
}

Vector3 get_local_player_coords()
{
	int_ptr local_player = Driver::read<int_ptr>(globals::world + 0x13a8);
	//int_ptr entity_ptr = Driver::read<int_ptr>(local_player + 0x4);
	//int_ptr entity_cfg = Driver::read<int_ptr>(entity_ptr + 0x3C);
	//int_ptr model_name = Driver::read<int_ptr>(entity_cfg + 0x38); // 0x38- model name

	//int model_string_size = Driver::read<int>(model_name + 0x4);
	//std::string model_name_string = std::string(ReadString(model_name + 0x8, model_string_size));
	//printf(xor("local model name is %s\n"), model_name_string.c_str());
	return get_coords(local_player);
}
void get_w2s_values()
{
	//dwTransformations = Driver::Read<uintptr_t>(base_address + world);
	//dwTransformations = globals::world;
	dwTransData = globals::dwTransData;
	//dwTransData = Driver::read<int_ptr>(globals::world + 0xE4);
	InvViewRight = Driver::read<Vector3>(dwTransData + 0x4);
	InvViewUp = Driver::read<Vector3>(dwTransData + 0x10);
	InvViewForward = Driver::read<Vector3>(dwTransData + 0x1C);
	InvViewTranslation = Driver::read<Vector3>(dwTransData + 0x28); //0x24
	ViewPortMatrix = Driver::read<Vector3>(dwTransData + 0x54);
	ProjD1 = Driver::read<Vector3>(dwTransData + 0xCC);
	ProjD2 = Driver::read<Vector3>(dwTransData + 0xD8);
}
void cache_globals()
{
	globals::world = Driver::read<int_ptr>(base_address + world);
	globals::dwTransData = Driver::read<int_ptr>(globals::world + 0xE4);

	get_w2s_values();
	globals::local_coords = get_local_player_coords();

}
typedef struct player_t
{
	Vector3 location;
	int_ptr entity_ptr;
	std::string name;
	bool is_bot = false;
	ImColor color = ImColor(255, 0, 0);
};

typedef struct vehicle_t
{
	Vector3 location;
	int_ptr entity_ptr;
	ImColor color = ImColor(0,255,0);
	std::string name;
	bool empty = true;
};
typedef struct corpse_t
{
	Vector3 location;
};
std::vector<player_t> players;
std::vector<vehicle_t> vehicles;

Vector3 w2s(Vector3 Position)
{
	
	Vector3 in = Position - InvViewTranslation;

	float x = in.Dot(InvViewRight);
	float y = in.Dot(InvViewUp);
	float z = in.Dot(InvViewForward);

	return Vector3(
		ViewPortMatrix.x * (1 + (x / ProjD1.x / z)),
		ViewPortMatrix.y * (1 - (y / ProjD2.y / z)),
		z);
}



char* ReadString(int_ptr Address, size_t Size)
{
	char Buffer[1024] = "";
	//ReadProcessMemory(ProcessHandle, LPCVOID(Address), Buffer, Size, 0);
	if (Size > 1024)
		return Buffer;
	Driver::read_memory(Address, &Buffer,Size);
	return Buffer;
};

Vector3 get_correct_coords(int_ptr entity)
{
	//int_ptr entity_ptr = Driver::read<int_ptr>(entity + 0x4);
	int_ptr visual_state = Driver::read<int_ptr>(entity + visual_state_offset);
	return Driver::read<Vector3>(visual_state + 0x28);
}

Vector3 get_vehicle_engine_coords(int_ptr entity)
{
	//int_ptr entity_ptr = Driver::read<int_ptr>(entity + 0x4);
	int_ptr visual_state = Driver::read<int_ptr>(entity + visual_state_offset);
	return Driver::read<Vector3>(visual_state + 0x128);
}

Vector3 get_head_coords(int_ptr entity)
{
	//int_ptr entity_ptr = Driver::read<int_ptr>(entity + 0x4);
	int_ptr visual_state = Driver::read<int_ptr>(entity + visual_state_offset);
	return Driver::read<Vector3>(visual_state + 0x010C);
}
std::mutex player_mtx;
std::mutex vehicles_mtx;
void safe_cache_players(std::vector<player_t> &players_buff)
{
	std::lock_guard<std::mutex> guard(player_mtx);

	players = players_buff;
	//vehicles = vehicles_buff;

}

void safe_cache_vehicles( std::vector<vehicle_t>& vehicles_buff)
{
	std::lock_guard<std::mutex> guard(vehicles_mtx);
	vehicles = vehicles_buff;

}

std::vector<player_t> safe_get_players()
{
	std::lock_guard<std::mutex> guard(player_mtx);
	return players;
}

std::vector<vehicle_t> safe_get_vehicles()
{
	std::lock_guard<std::mutex> guard(vehicles_mtx);
	return vehicles;
}
bool is_dead(int_ptr entity)
{
	bool dead = Driver::read<bool>(entity + 0x258);
	//printf(xor ("dead is %d"), dead);
	return dead;
}
void aimbot_func(uint64_t entity)
{
	Vector3 coords = get_head_coords(entity);

	int_ptr bullet_table = Driver::read<int_ptr>(globals::world + 0x7dc);
	uint32_t bullet_table_size = Driver::read<uint32_t>(globals::world + 0x7dc + table_size);

	for (uint32_t i = 0; i < bullet_table_size; i++)
	{
		int_ptr bullet = Driver::read<int_ptr>(bullet_table + 0x4 * i);
		int_ptr visual_state = Driver::read<int_ptr>(bullet + visual_state_offset);
		//Driver::write<Vector3>(visual_state + 0x28, {coords.x, coords.y + 0.1f, coords.z});
		Driver::write<Vector3>(visual_state + 0x28, { coords.x, coords.y , coords.z });

	}
}

void heal_me()
{


	int_ptr local_player = Driver::read<int_ptr>(globals::world + transport_on_offset);
	local_player = Driver::read<int_ptr>(local_player + 0x4);

	int_ptr visual_state = Driver::read<int_ptr>(local_player + visual_state_offset);
	Driver::write<f32>(visual_state + 0xAC, 500000.f);
	int_ptr player_damage = Driver::read<int_ptr>(local_player + 0xC0); //0x00C4  
	uint32_t damaged_parts_count = Driver::read<uint32_t>(local_player + 0xC4);

	//int_ptr entity_cfg = Driver::read<int_ptr>(local_player + 0x3C);
//	int_ptr object_name_ptr = Driver::read<int_ptr>(entity_cfg + 0x6C);
	//int string_size = Driver::read<int>(object_name_ptr + 0x4);
//	char* string = ReadString(object_name_ptr + 0x8, string_size);
	//if (!strcmp(string, xor ("soldier")))
		//return;
		
	//printf("name is %s\n", string);
	//Sleep(3000);
	for (uint32_t i = 0; i < damaged_parts_count; i++)
	{
		Driver::write<f32>(player_damage + i * 0x4, 0.f);
	}
		/*if (this)
		{
			this->entityVisualState->fuelLevel = 500000.f;

			for (int i = 0; i < this->damagePartCount; ++i)
				this->playerDamage[i] = 0.f;

		}*/
	
}

//void heal_local()
//{
//	int_ptr local_player = Driver::read<int_ptr>(globals::world + camera_on_offset);
//	local_player = Driver::read<int_ptr>(local_player + 0x4);
//
//	int_ptr visual_state = Driver::read<int_ptr>(local_player + visual_state_offset);
//	Driver::write<f32>(visual_state + 0xAC, 100.f);
//	int_ptr player_damage = Driver::read<int_ptr>(local_player + 0xC0); //0x00C4  
//	uint32_t damaged_parts_count = Driver::read<uint32_t>(local_player + 0xC4);
//
//	for (uint32_t i = 0; i < damaged_parts_count; i++)
//	{
//		Driver::write<f32>(player_damage + i * 0x4, 0.f);
//	}
//	/*if (this)
//	{
//		this->entityVisualState->fuelLevel = 500000.f;
//
//		for (int i = 0; i < this->damagePartCount; ++i)
//			this->playerDamage[i] = 0.f;
//
//	}*/
//}

void aimbot_vehicles(uint64_t entity)
{
	//printf(xor ("vehice is %llx\n"), entity);
	Vector3 coords = { 0,0,0 };
	

	coords = get_correct_coords(entity); // 0x128

	int_ptr bullet_table = Driver::read<int_ptr>(globals::world + 0x7dc);
	uint32_t bullet_table_size = Driver::read<uint32_t>(globals::world + 0x7dc + table_size);

	for (uint32_t i = 0; i < bullet_table_size; i++)
	{
		int_ptr bullet = Driver::read<int_ptr>(bullet_table + 0x4 * i);
		int_ptr visual_state = Driver::read<int_ptr>(bullet + visual_state_offset);
		Driver::write<Vector3>(visual_state + 0x28, { coords.x, coords.y , coords.z });

	}
}
void set_local_position(Vector3 position)
{
	int_ptr local_player = Driver::read<int_ptr>(globals::world + camera_on_offset);
	int_ptr entity_ptr = Driver::read<int_ptr>(local_player + 0x4);
	int_ptr visual_state = Driver::read<int_ptr>(entity_ptr + visual_state_offset);
	Driver::write<Vector3>(visual_state + 0x28, position);
}

void razyeb(Vector3 position)
{
	int_ptr local_player = Driver::read<int_ptr>(globals::world + 0x13A8);
	int_ptr entity_ptr = Driver::read<int_ptr>(local_player + 0x4);
	int_ptr visual_state = Driver::read<int_ptr>(entity_ptr + visual_state_offset);
	Driver::write<Vector3>(visual_state + 0x28, position);
}

bool is_bot(int_ptr entity)
{
	uint32_t id = Driver::read<uint32_t>(entity + 0xB14);

	if (id > 1)
		return false;

	return true;
	return false;
}
//void cache_entities()
//{
//
//		
//	std::vector<player_t> players_buffer;
//	std::vector<vehicle_t> vehicles_buffer;
//
//	uint32_t world_base = globals::world;
//	//printf("base is %llx\n", world_base);
//	uint32_t entity_table = Driver::read<int_ptr>(world_base + entity_table_offset);
//
//	
//	//uintptr_t entity_table = Driver::read<uintptr_t>(entity_table_ptr + 0x0);
//	uint32_t entity_table_size = Driver::read<int>(world_base + entity_table_offset + table_size);
//	
//	//printf("test is %d\n", Driver::read<int>(world_base + 0x0));
//	for (uint32_t i = 0; i < entity_table_size; i++)
//	{
//		int_ptr entity = Driver::read<int_ptr>(entity_table + i * 0x4);
//		int_ptr entity_cfg = Driver::read<int_ptr>(entity+ 0x3C);
//		int_ptr object_name_ptr = Driver::read<int_ptr>(entity_cfg + 0x6C); //0x38 - model name
//		
//		
//		int string_size = Driver::read<int>(object_name_ptr + 0x4);
//		char* string = ReadString(object_name_ptr + 0x8, string_size);
//		//int_ptr model_name = Driver::read<int_ptr>(entity_cfg + 0x38); // 0x38- model name
//
//		//int model_string_size = Driver::read<int>(model_name + 0x4);
//		//std::string model_name_string = std::string(ReadString(model_name +0x8, model_string_size));
//	
//
//		
//
//	
//	
//		//if (!strcmp(string, xor("soldier")))
//		//if(model_name_string.find(xor("soldier")) != std::string::npos || model_name_string.find(xor ("survivor")) != std::string::npos || model_name_string.find(xor ("rifleman")) != std::string::npos || model_name_string.find(xor ("ghill")) != std::string::npos) //ghill
//		if (!strcmp(string, xor ("soldier")))
//		{
//			//if (is_bot(entity))
//				//continue;
//			//if (is_dead(entity))
//				//continue;
//			//printf("name is %s \n", model_name_string.c_str());
//			player_t player;
//			player.entity_ptr = entity;
//			auto coords = get_correct_coords(entity);
//			player.location = coords;
//			player.name = xor ("soldier");
//			//else
//				//player.name = xor ("bot");
//			players_buffer.push_back(player);
//
//		}
//		//helicopter //car //tank //motorcycle //airplane
//
//		else if (!strcmp(string, xor("helicopter")) || !strcmp(string, xor("car")) || !strcmp(string, xor( "motorcycle")) || !strcmp(string, xor("airplane")) || !strcmp(string, xor("tank")))
//		{
//			vehicle_t vehicle;
//			vehicle.entity_ptr = entity;
//			vehicle.location = get_correct_coords(entity);
//			vehicle.name = string;
//
//			vehicles_buffer.push_back(vehicle);
//		}
//		//else if (!strcmp(string, xor ("soldier")))	
//		//{
//		//	player_t player;
//		//	player.entity_ptr = entity;
//		//	auto coords = get_correct_coords(entity);
//		//	player.location = coords;
//		//	player.name = model_name_string;
//		//	players_buffer.push_back(player);
//		//}
//		
//	}
//
//	
//
//
//	//uintptr_t entity_table = Driver::read<uintptr_t>(entity_table_ptr + 0x0);
//
//	int_ptr far_entity_table = Driver::read<int_ptr>(world_base + 0x930);
//	uint32_t far_entity_table_size = Driver::read<int>(world_base + 0x930 + table_size);
//	//printf(xor (" far_entity_table is %llx\n"), far_entity_table);
//	//printf(xor (" far_entity_table_size is %d\n"), far_entity_table_size);
//	
//	for (uint32_t i = 0; i < far_entity_table_size; i++)
//	{
//		int_ptr entity = Driver::read<int_ptr>(far_entity_table + i * 0x4);
//		int_ptr entity_cfg = Driver::read<int_ptr>(entity + 0x3C);
//		int_ptr object_name_ptr = Driver::read<int_ptr>(entity_cfg + 0x6C); //0x38 - model name
//
//
//		int string_size = Driver::read<int>(object_name_ptr + 0x4);
//		char* string = ReadString(object_name_ptr + 0x8, string_size);
//		//int_ptr model_name = Driver::read<int_ptr>(entity_cfg + 0x38); // 0x38- model name
//
//		//int model_string_size = Driver::read<int>(model_name + 0x4);
//	std::string model_name_string = std::string(ReadString(model_name +0x8, model_string_size));
//
//
//
//
//
//
//		//if (!strcmp(string, xor("soldier")))
//		//if(model_name_string.find(xor("soldier")) != std::string::npos || model_name_string.find(xor ("survivor")) != std::string::npos || model_name_string.find(xor ("rifleman")) != std::string::npos || model_name_string.find(xor ("ghill")) != std::string::npos) //ghill
//		if (!strcmp(string, xor ("soldier")))
//		{
//			//if (is_bot(entity))
//				//continue;
//			//if (is_dead(entity))
//				//continue;
//			//printf("name is %s \n", model_name_string.c_str());
//			player_t player;
//			player.entity_ptr = entity;
//			auto coords = get_correct_coords(entity);
//			player.location = coords;
//			player.name = xor ("soldier");
//			//else
//				//player.name = xor ("bot");
//			players_buffer.push_back(player);
//
//		}
//		//helicopter //car //tank //motorcycle //airplane
//
//		else if (!strcmp(string, xor ("helicopter")) || !strcmp(string, xor ("car")) || !strcmp(string, xor ("motorcycle")) || !strcmp(string, xor ("airplane")) || !strcmp(string, xor ("tank")))
//		{
//			vehicle_t vehicle;
//			vehicle.entity_ptr = entity;
//			vehicle.location = get_correct_coords(entity);
//			vehicle.name = string;
//
//			vehicles_buffer.push_back(vehicle);
//		}
//		//else if (!strcmp(string, xor ("soldier")))
//		//{
//		//	player_t player;
//		//	player.entity_ptr = entity;
//		//	auto coords = get_correct_coords(entity);
//		//	player.location = coords;
//		//	player.name = model_name_string;
//		//	players_buffer.push_back(player);
//		//}
//
//	}
//	safe_cache_players(players_buffer);
//	//players = players_buffer;
//	safe_cache_vehicles(vehicles_buffer);
//	//vehicles = vehicles_buffer;
//}




void cache_entities_test()
{

	
	std::vector<player_t> players_buffer;
	std::vector<vehicle_t> vehicles_buffer;

	uint32_t world_base = globals::world;
	//printf("base is %llx\n", world_base);
	uint32_t entity_table_ptr = Driver::read<int_ptr>(world_base + 0x5FC);


	//uintptr_t entity_table = Driver::read<uintptr_t>(entity_table_ptr + 0x0);
	uint32_t entity_table_size = Driver::read<int>(entity_table_ptr + table_size);
	int_ptr entity_table = Driver::read<int_ptr>(entity_table_ptr + 0x0);

	//printf("test is %d\n", Driver::read<int>(world_base + 0x0));

	for (uint32_t i = 0; i < entity_table_size; i++)
	{
		int_ptr entity_ptr = Driver::read<int_ptr>(entity_table + i * 0x34);
		int_ptr entity = Driver::read<int_ptr>(entity_ptr + 0x4);
		int_ptr entity_cfg = Driver::read<int_ptr>(entity + 0x3C);
		int_ptr object_name_ptr = Driver::read<int_ptr>(entity_cfg + 0x6C); //0x38 - model name


		int string_size = Driver::read<int>(object_name_ptr + 0x4);
		char* string = ReadString(object_name_ptr + 0x8, string_size);
		//int_ptr model_name = Driver::read<int_ptr>(entity_cfg + 0x38); // 0x38- model name

		//int model_string_size = Driver::read<int>(model_name + 0x4);
		//std::string model_name_string = std::string(ReadString(model_name +0x8, model_string_size));






		
	
		if (!strcmp(string, xor ("soldier"))) {
		
			//if (is_dead(entity))
				//continue;
			//printf("name is %s \n", model_name_string.c_str());
			player_t player;
		
				//continue;
			player.entity_ptr = entity;
			auto coords = get_correct_coords(entity);
			player.location = coords;
			if (is_bot(entity))
			{
				player.is_bot = true;
				player.name = xor ("bot");
				player.color = ImColor(255, 255, 102);
			}
			else
				player.name = xor ("soldier");
		
			players_buffer.push_back(player);

		}
		//helicopter //car //tank //motorcycle //airplane

		else if (!strcmp(string, xor ("helicopter")) || !strcmp(string, xor ("car")) || !strcmp(string, xor ("motorcycle")) || !strcmp(string, xor ("airplane")) || !strcmp(string, xor ("tank")))
		{
			


			vehicle_t vehicle;
			vehicle.entity_ptr = entity;
			auto coords = get_correct_coords(entity);
			int_ptr vodila = Driver::read<int_ptr>(entity + 0xAFC);
			if (vodila)
			{
				vehicle.empty = false;
				vehicle.color = ImColor(255, 102, 0);
			}
			vehicle.location = coords;
			vehicle.name = string;
			

			vehicles_buffer.push_back(vehicle);
		}
	
		//else if (!strcmp(string, xor ("soldier")))	
		//{
		//	player_t player;
		//	player.entity_ptr = entity;
		//	auto coords = get_correct_coords(entity);
		//	player.location = coords;
		//	player.name = model_name_string;
		//	players_buffer.push_back(player);
		//}

	}



	//uintptr_t entity_table = Driver::read<uintptr_t>(entity_table_ptr + 0x0);


	


	safe_cache_players(players_buffer);
	//players = players_buffer;
	safe_cache_vehicles(vehicles_buffer);
	//vehicles = vehicles_buffer;
}
