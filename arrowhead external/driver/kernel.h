#pragma once
#include <TlHelp32.h>
#include <psapi.h>
#include <vector>
#include <string>
#include <map>

typedef uint32_t u32;
typedef uint64_t u64;

typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;

typedef const char* chars;
typedef const wchar_t* wchars;

typedef std::string str;
template<class t> using vec = std::vector<t>;
template<class t1, class t2> using map = std::map<t1, t2>;

#pragma once



//#define OFFLINE_MODE
#define ONLINE_MODE

namespace memory_system
{
	HANDLE process_handle = 0;	// for usermode

	u32 process_id = 0;			// for usermode

	enum class packet_type { module, memory, init, apc };

	enum class copy_type { read, write };

	struct packet_struct
	{
		u64 magic = 0xef192ac9b;

		packet_type type;

		union
		{
			struct { u64 module_address; const wchar_t* module_name; } module;

			struct { u32 game; u32 usermode; } initialization;

			struct { u64 source_address; u64 dest_address; u32 size; copy_type type; } memory;

			struct { u32 thread; u32 new_value; u32 old_value; } apc;

		} data;
	};

	uint64_t(__stdcall* function)(__int64 a1) = 0;

	void send(packet_struct& packet)
	{
		function(reinterpret_cast<__int64>(&packet));
	};

	u32 safe_thread()
	{
		packet_struct packet{}; // 0xffffbfff


		packet.data.apc.thread = GetCurrentThreadId();
		packet.data.apc.new_value = 0xffffbfff;
		packet.type = packet_type::apc;

		send(packet);

		return packet.data.apc.old_value;
		//return 0;
	}

	void unsafe_thread(u32 value)
	{
		packet_struct packet{}; // 0xffffbfff

		packet.data.apc.thread = GetCurrentThreadId();
		packet.data.apc.new_value = value;
		packet.type = packet_type::apc;

		send(packet);
	}

	u32 initialization(u32 process_identifier)
	{
#ifdef ONLINE_MODE



		auto init = [&](u32 game_pid, u32 usermode_pid) -> void
			{
				packet_struct packet{};

				packet.data.initialization.game = game_pid;
				packet.data.initialization.usermode = usermode_pid;
				packet.type = packet_type::init;

				send(packet);

			};
		u32 usermode_pid = GetCurrentProcessId();
		if (process_identifier && usermode_pid)
		{
			init(process_identifier, usermode_pid);
		}

		return 0;
#endif
#ifdef OFFLINE_MODE
		process_id = process_identifier;

		process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_identifier);
		return 0;

#endif


	}

	void copy_memory(u64 source_address, u64 dest_address, u32 size, copy_type type)
	{
		packet_struct packet{};


		packet.data.memory.source_address = source_address;
		packet.data.memory.dest_address = dest_address;
		packet.data.memory.size = size;
		packet.data.memory.type = type;

		packet.type = packet_type::memory;


		send(packet);
	}

	void read_memory(u64 address, void* buffer, u32 size)
	{
#ifdef ONLINE_MODE
		copy_memory(address, (u64)buffer, size, copy_type::read);
#endif

#ifdef OFFLINE_MODE
		ReadProcessMemory(process_handle, (LPCVOID)address, buffer, size, 0);
#endif


	}

	template<typename type>
	type read(u64 address)
	{
		type buffer{};

		read_memory(address, &buffer, sizeof(type));

		return buffer;
	}

	template<typename type>
	type read(u64 address, vec<u64> chain)
	{
		u64 current = address;

		for (u32 i = 0; i < chain.size() - 1; i++) { current = read<u64>(current + chain[i]); }

		return read<type>(current + chain[chain.size() - 1]);
	}

	void write_memory(u64 address, void* buffer, u32 size)
	{

#ifdef ONLINE_MODE
		copy_memory((u64)buffer, address, size, copy_type::write);
#endif
#ifdef OFFLINE_MODE
		WriteProcessMemory(process_handle, (LPVOID)address, buffer, size, 0);
#endif


	}

	template<typename type>
	void write(u64 address, type value)
	{
		type buffer = value;

		write_memory(address, &buffer, sizeof(type));
	}

	u64 get_module(const wchar_t* module_name)
	{
#ifdef ONLINE_MODE
		packet_struct packet{};

		packet.data.module.module_name = module_name;
		packet.type = packet_type::module;

		send(packet);

		return packet.data.module.module_address;
#endif

#ifdef OFFLINE_MODE
		MODULEENTRY32 process{ sizeof(MODULEENTRY32) };

		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, process_id);

		if (!Module32First(snapshot, &process))
		{
			return 0;
		}

		DWORD64 process_identifier = 0;

		do
		{
			if (std::wstring(process.szModule) == module_name)
			{
				process_identifier = (DWORD64)process.modBaseAddr;
				break;
			}
		} while (Module32Next(snapshot, &process));

		CloseHandle(snapshot);

		return process_identifier;
#endif

	}

	u32 get_process_by_name(std::wstring process_name)
	{
		PROCESSENTRY32 process{ sizeof(PROCESSENTRY32) };

		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

		if (!Process32First(snapshot, &process))
		{
			return 0;
		}

		u32 process_identifier = 0;

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