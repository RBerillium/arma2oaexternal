#include "pdb.hpp"

#include "../xor.hpp"

bool pdb_t::load(std::string name)
{
	auto random_string = [](size_t size) -> std::string
	{
		std::string string;

		std::string alphabet = xor ("abcdefghijklmnopqrstuvwxyz");

		srand(time(0) * GetCurrentThreadId());

		for (uint32_t i = 0; i < size; i++)
		{
			string.push_back(alphabet.at(rand() % alphabet.length()));
		}

		return string;
	};

	std::string pdb_path = xor("C:\\windows\\temp\\") + random_string(16) + xor(".pdb");

	this->path = pdb_path;

	std::ifstream file(name, std::ios::binary | std::ios::ate);

	if (!file)
	{
		return false;
	}

	std::streamsize size = file.tellg();

	file.seekg(0, std::ios::beg);

	std::vector<char> buffer(size);

	if (!file.read(buffer.data(), size) || size == 0)
	{
		return false;
	}

	IMAGE_DOS_HEADER* pDos = (IMAGE_DOS_HEADER*)buffer.data();
	IMAGE_NT_HEADERS* pNT = (IMAGE_NT_HEADERS*)(buffer.data() + pDos->e_lfanew);
	IMAGE_FILE_HEADER* pFile = &pNT->FileHeader;
	IMAGE_OPTIONAL_HEADER64* pOpt64 = NULL;
	IMAGE_OPTIONAL_HEADER32* pOpt32 = NULL;

	BOOL x86 = FALSE;

	if (pFile->Machine == IMAGE_FILE_MACHINE_AMD64)
	{
		pOpt64 = (IMAGE_OPTIONAL_HEADER64*)(&pNT->OptionalHeader);
	}
	else if (pFile->Machine == IMAGE_FILE_MACHINE_I386)
	{
		pOpt32 = (IMAGE_OPTIONAL_HEADER32*)(&pNT->OptionalHeader);

		x86 = TRUE;
	}
	else
	{
		return false;
	}
	DWORD ImageSize = x86 ? pOpt32->SizeOfImage : pOpt64->SizeOfImage;

	// file buffer to image buffer
	PBYTE ImageBuffer = (PBYTE)malloc(ImageSize);

	if (!ImageBuffer)
	{
		return false;
	}

	memcpy(ImageBuffer, buffer.data(), x86 ? pOpt32->SizeOfHeaders : pOpt64->SizeOfHeaders);

	IMAGE_SECTION_HEADER* pCurrentSectionHeader = IMAGE_FIRST_SECTION(pNT);

	for (UINT i = 0; i != pFile->NumberOfSections; ++i, ++pCurrentSectionHeader)
	{
		if (pCurrentSectionHeader->SizeOfRawData)
		{
			memcpy(ImageBuffer + pCurrentSectionHeader->VirtualAddress, buffer.data() + pCurrentSectionHeader->PointerToRawData, pCurrentSectionHeader->SizeOfRawData);
		}
	}

	IMAGE_DATA_DIRECTORY* pDataDir = nullptr;

	if (x86)
	{
		pDataDir = &pOpt32->DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG];
	}
	else
	{
		pDataDir = &pOpt64->DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG];
	}

	IMAGE_DEBUG_DIRECTORY* pDebugDir = (IMAGE_DEBUG_DIRECTORY*)(ImageBuffer + pDataDir->VirtualAddress);

	if (!pDataDir->Size || IMAGE_DEBUG_TYPE_CODEVIEW != pDebugDir->Type)
	{
		free(ImageBuffer);

		return false;
	}

	struct PdbInfo
	{
		DWORD	Signature;
		GUID	Guid;
		DWORD	Age;
		char	PdbFileName[1];
	};

	PdbInfo* pdb_info = (PdbInfo*)(ImageBuffer + pDebugDir->AddressOfRawData);

	if (pdb_info->Signature != 0x53445352)
	{
		free(ImageBuffer);

		return false;
	}

	if (!std::filesystem::exists(pdb_path))
	{
		// download pdb
		wchar_t w_GUID[100] = { 0 };
		if (!StringFromGUID2(pdb_info->Guid, w_GUID, 100))
		{
			free(ImageBuffer);

			return false;
		}

		char a_GUID[100]{ 0 };
		size_t l_GUID = 0;
		if (wcstombs_s(&l_GUID, a_GUID, w_GUID, sizeof(a_GUID)) || !l_GUID)
		{
			free(ImageBuffer);

			return false;
		}

		char guid_filtered[256] = { 0 };

		for (UINT i = 0; i != l_GUID; ++i)
		{
			if ((a_GUID[i] >= '0' && a_GUID[i] <= '9') || (a_GUID[i] >= 'A' && a_GUID[i] <= 'F') || (a_GUID[i] >= 'a' && a_GUID[i] <= 'f'))
			{
				guid_filtered[strlen(guid_filtered)] = a_GUID[i];
			}
		}

		char age[3] = { 0 };
		_itoa_s(pdb_info->Age, age, 10);

		std::string url = xor("https://msdl.microsoft.com/download/symbols/");
		url += pdb_info->PdbFileName;
		url += "/";
		url += guid_filtered;
		url += age;
		url += "/";
		url += pdb_info->PdbFileName;

		// download
		HRESULT hr = URLDownloadToFileA(NULL, url.c_str(), pdb_path.c_str(), NULL, NULL);

		if (FAILED(hr))
		{
			free(ImageBuffer);

			return false;
		}

		free(ImageBuffer);
	}

	// get pdb file size
	WIN32_FILE_ATTRIBUTE_DATA file_attr_data{ 0 };
	if (!GetFileAttributesExA(pdb_path.c_str(), GetFileExInfoStandard, &file_attr_data))
	{
		return false;
	}
	auto pdbSize = file_attr_data.nFileSizeLow;

	// open pdb file
	HANDLE hPdbFile = CreateFileA(pdb_path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
	if (hPdbFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	// open current process
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, GetCurrentProcessId());
	if (!hProcess)
	{
		CloseHandle(hPdbFile);
		return false;
	}

	// Initializes the symbol handler for a process
	if (!SymInitialize(hProcess, pdb_path.c_str(), FALSE))
	{
		CloseHandle(hProcess);
		CloseHandle(hPdbFile);
		return false;
	}

	SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_AUTO_PUBLICS | SYMOPT_DEBUG | SYMOPT_LOAD_ANYTHING);
	//SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_AUTO_PUBLICS | SYMOPT_LOAD_ANYTHING);

	DWORD64 SymbolTable = SymLoadModuleEx(hProcess, NULL, pdb_path.c_str(), NULL, 0x10000000, pdbSize, NULL, NULL);
	if (!SymbolTable)
	{
		SymCleanup(hProcess);
		CloseHandle(hProcess);
		CloseHandle(hPdbFile);
		return false;
	}

	this->pdb_file = hPdbFile;
	this->process = hProcess;
	return true;

	return true;
}

void pdb_t::unload()
{
	SymUnloadModule64(this->process, 0x10000000);
	SymCleanup(this->process);
	CloseHandle(this->process);
	CloseHandle(this->pdb_file);

	remove(path.c_str());
}

uint32_t pdb_t::get_rva(std::string name)
{
	SYMBOL_INFO si = { 0 };
	si.SizeOfStruct = sizeof(SYMBOL_INFO);
	if (!SymFromName(this->process, name.c_str(), &si))
	{
		return 0;
	}
	return (ULONG)(si.Address - si.ModBase);
}

pdb_t::struct_info pdb_t::get_struct(std::string name)
{
	return {};
}