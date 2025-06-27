#pragma once
#include <filesystem> 
#include <string> 
#include <fstream>
#include <WinBase.h>
#include <wbemidl.h>
#include <TlHelp32.h>
#pragma comment(lib, "wbemuuid.lib")

#include "xor.hpp"
#include "json.hpp"
#include "console.hpp"
#include "json.hpp"


std::string json_path = _("C:\\Windows\\AppSave\\key_arma2oa.json");
std::string vmkey_path = _("C:\\Windows\\AppSave\\cfg");
std::string vmkey_alternalive_path = _("C:\\Windows\\AppSave\\cfg:ddsd81js91sds");
std::string executable_atlernative_directory_name = _(":8dnbnasdkskkdf9");

std::vector<uint8_t> usermode_image;
std::vector<uint8_t> driver_image;

console_c* console;

using json = nlohmann::json;


uint32_t get_process_id(std::string process_name)
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
		if (std::string(process.szExeFile) == process_name)
		{
			process_identifier = process.th32ProcessID;
			break;
		}
	} while (Process32Next(snapshot, &process));

	CloseHandle(snapshot);

	return process_identifier;
}

std::string ReadFromJson(std::string path, std::string section)
{
	if (!std::filesystem::exists(path))
		return _("File Not Found ");
	std::ifstream file(path);
	json data = json::parse(file);
	file.close();
	return data[section];
	
}

bool CheckIfJsonKeyExists(std::string path, std::string section)
{
	if (!std::filesystem::exists(path))
		return _("File Not Found ");
	std::ifstream file(path);
	json data = json::parse(file);
	file.close();
	return data.contains(section);

}

bool WriteToJson(std::string path, std::string name, std::string value, bool userpass, std::string name2, std::string value2)
{
	json file;
	if (!userpass)
	{
		file[name] = value;
	}
	else
	{
		file[name] = value;
		file[name2] = value2;
	}

	std::ofstream jsonfile(path, std::ios::out);
	jsonfile << file;
	jsonfile.close();
	if (!std::filesystem::exists(path))
		return false;

	return true;
}
void checkAuthenticated(std::string ownerid) {
	while (true) {
		if (GlobalFindAtomA(ownerid.c_str()) == 0) {
			exit(13);
		}
		Sleep(1000); // thread interval
	}
}

void message_box()
{
	MessageBox(NULL, _("Forbidden process detected. \n Close it!"), _("wtf?"), MB_OK | MB_ICONERROR);
}



void create_process(const char* filepath, const char* params)
{


	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	//si.dwFlags = STARTF_USESHOWWINDOW;
	//si.wShowWindow = SW_SHOW;
	// Команда и параметры
	//const char* command = "C:\\path\\to\\program.exe argument";

	// Создание процесса
	if (!CreateProcess(
		NULL,                      // Нет имени модуля (используется командная строка)
		const_cast<char*>(filepath),// Командная строка
		NULL,                      // Дескриптор безопасности процесса
		NULL,                      // Дескриптор безопасности потока
		FALSE,                   // Унаследование дескрипторов
		CREATE_NO_WINDOW,					// Не создавать окно (опционально) CREATE_NO_WINDOW,
		NULL,                      // Использовать среду родительского процесса
		NULL,                      // Использовать текущий каталог родительского процесса
		&si,                       // Указатель на структуру STARTUPINFO
		&pi                        // Указатель на структуру PROCESS_INFORMATION
	))	

		//std::cerr << _("CreateProcess failed: " )<< GetLastError() << std::endl;
		console->error(_("[-] create_process filed with error %ld"), GetLastError());
	return;


	// Закрытие дескрипторов
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return;



}

bool IsWdFilterRunning() {
	// Имя службы
	const char* serviceName = _("wdfilter");

	// Открываем диспетчер управления службами
	SC_HANDLE hSCManager = OpenSCManager(nullptr, nullptr, SC_MANAGER_CONNECT);
	if (!hSCManager) {
		console->important_info(_("[-] failed to open service manager, check for wdfilter skipped"));
		return false;
	}

	// Открываем службу
	SC_HANDLE hService = OpenService(hSCManager, serviceName, SERVICE_QUERY_STATUS);
	if (!hService) {
		console->important_info(_("[-] failed to open service manager, check for wdfilter skipped"));
		CloseServiceHandle(hSCManager);
		return false;
	}

	// Проверяем статус службы
	SERVICE_STATUS serviceStatus;
	if (QueryServiceStatus(hService, &serviceStatus)) {
		if (serviceStatus.dwCurrentState == SERVICE_RUNNING) {
			
			CloseServiceHandle(hService);
			CloseServiceHandle(hSCManager);
			return true;
		}
	
	}
	else {
		console->important_info(_("[-] failed to open service manager, check for wdfilter skipped"));
	}

	// Освобождаем ресурсы
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);

	return false;
}


bool IsHyperVEnabled() {
	HRESULT hres;

	// Инициализация COM
	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres)) {

		return false;
	}

	// Установка безопасности COM
	hres = CoInitializeSecurity(
		NULL,
		-1,
		NULL,
		NULL,
		RPC_C_AUTHN_LEVEL_DEFAULT,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		NULL,
		EOAC_NONE,
		NULL
	);

	if (FAILED(hres)) {
	
		CoUninitialize();
		return false;
	}

	// Подключение к пространству WMI
	IWbemLocator* pLoc = NULL;
	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator,
		(LPVOID*)&pLoc
	);

	if (FAILED(hres)) {

		CoUninitialize();
		return false;
	}

	IWbemServices* pSvc = NULL;
	hres = pLoc->ConnectServer(
		BSTR(_(L"ROOT\\CIMV2")),
		NULL,
		NULL,
		0,
		NULL,
		0,
		0,
		&pSvc
	);

	if (FAILED(hres)) {
	
		pLoc->Release();
		CoUninitialize();
		return false;
	}

	// Устанавливаем параметры безопасности для WMI соединения
	hres = CoSetProxyBlanket(
		pSvc,
		RPC_C_AUTHN_WINNT,
		RPC_C_AUTHZ_NONE,
		NULL,
		RPC_C_AUTHN_LEVEL_CALL,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		NULL,
		EOAC_NONE
	);

	if (FAILED(hres)) {
		
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return false;
	}

	// Выполнение WMI-запроса
	IEnumWbemClassObject* pEnumerator = NULL;
	hres = pSvc->ExecQuery(
		BSTR(_(L"WQL")),
		BSTR(_(L"SELECT HypervisorPresent FROM Win32_ComputerSystem")),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumerator
	);

	if (FAILED(hres)) {
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return false;
	}

	// Получение результата
	IWbemClassObject* pclsObj = NULL;
	ULONG uReturn = 0;

	bool hypervisorPresent = false;

	while (pEnumerator) {
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

		if (0 == uReturn) {
			break;
		}

		VARIANT vtProp;

		// Получаем значение свойства HypervisorPresent
		hr = pclsObj->Get(_(L"HypervisorPresent"), 0, &vtProp, 0, 0);
		if (SUCCEEDED(hr) && vtProp.vt == VT_BOOL) {
			hypervisorPresent = vtProp.boolVal;
		}

		VariantClear(&vtProp);
		pclsObj->Release();
	}

	// Освобождение ресурсов
	pSvc->Release();
	pLoc->Release();
	pEnumerator->Release();
	CoUninitialize();

	return hypervisorPresent;
}


bool IsKernelIsolationEnabled() {
	HKEY hKey;
	DWORD value = 0;
	DWORD valueSize = sizeof(value);

	// Открываем ключ реестра
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _("SYSTEM\\CurrentControlSet\\Control\\DeviceGuard"), 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
		
		return false;
	}

	// Читаем значение EnableVirtualizationBasedSecurity
	if (RegQueryValueEx(hKey, _("EnableVirtualizationBasedSecurity"), nullptr, nullptr, (LPBYTE)&value, &valueSize) != ERROR_SUCCESS) {
	
		RegCloseKey(hKey);
		return false;
	}

	RegCloseKey(hKey);

	return value == 1;
}

bool IsVirtualizationSupported() {
	int cpuInfo[4] = { 0 };

	// Получаем информацию о процессоре
	__cpuid(cpuInfo, 1);

	// Бит 5 регистра ECX указывает на поддержку VMX (Intel)
	return (cpuInfo[2] & (1 << 5)) != 0;
}

bool IsVbsEnabled() {
	HRESULT hres;

	// Инициализация COM
	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres)) {
	
		return false;
	}

	// Установка безопасности COM
	hres = CoInitializeSecurity(
		NULL,
		-1,
		NULL,
		NULL,
		RPC_C_AUTHN_LEVEL_DEFAULT,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		NULL,
		EOAC_NONE,
		NULL
	);

	if (FAILED(hres)) {
		
		CoUninitialize();
		return false;
	}

	// Подключение к пространству WMI
	IWbemLocator* pLoc = NULL;
	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator,
		(LPVOID*)&pLoc
	);

	if (FAILED(hres)) {

		CoUninitialize();
		return false;
	}

	IWbemServices* pSvc = NULL;
	hres = pLoc->ConnectServer(
		BSTR(_(L"ROOT\\CIMV2\\Security\\MicrosoftVolumeEncryption")),
		NULL,
		NULL,
		0,
		NULL,
		0,
		0,
		&pSvc
	);

	if (FAILED(hres)) {

		pLoc->Release();
		CoUninitialize();
		return false;
	}

	// Выполнение WMI-запроса
	IEnumWbemClassObject* pEnumerator = NULL;
	hres = pSvc->ExecQuery(
		BSTR(_(L"WQL")),
		BSTR(_(L"SELECT * FROM Win32_DeviceGuard")),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumerator
	);

	if (FAILED(hres)) {
	
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return false;
	}

	// Читаем результат
	IWbemClassObject* pclsObj = NULL;
	ULONG uReturn = 0;
	bool vbsEnabled = false;

	while (pEnumerator) {
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
		if (0 == uReturn) {
			break;
		}

		VARIANT vtProp;
		hr = pclsObj->Get(_(L"SecurityServicesRunning"), 0, &vtProp, 0, 0);
		if (SUCCEEDED(hr) && vtProp.vt == (VT_ARRAY | VT_UI1)) {
			// Проверка, что служба Device Guard работает
			SAFEARRAY* psa = vtProp.parray;
			if (psa) {
				long lowerBound, upperBound;
				SafeArrayGetLBound(psa, 1, &lowerBound);
				SafeArrayGetUBound(psa, 1, &upperBound);

				for (long i = lowerBound; i <= upperBound; i++) {
					BYTE serviceState = 0;
					SafeArrayGetElement(psa, &i, &serviceState);
					if (serviceState == 2) { // 2 означает включенную VBS
						vbsEnabled = true;
						break;
					}
				}
				SafeArrayDestroy(psa);
			}
		}

		VariantClear(&vtProp);
		pclsObj->Release();
	}

	// Освобождение ресурсов
	pSvc->Release();
	pLoc->Release();
	pEnumerator->Release();
	CoUninitialize();

	return vbsEnabled;
}

bool IsKernelDmaProtectionEnabled() {
	HKEY hKey;
	DWORD value = 0;
	DWORD valueSize = sizeof(value);

	// Открываем ключ реестра
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _("SYSTEM\\CurrentControlSet\\Control\\DeviceGuard\\Scenarios\\DmaGuard"), 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
		
		return false;
	}

	// Читаем значение DmaGuardRequired
	if (RegQueryValueEx(hKey, _("DmaGuardRequired"), nullptr, nullptr, (LPBYTE)&value, &valueSize) != ERROR_SUCCESS) {
		RegCloseKey(hKey);
		return false;
	}

	RegCloseKey(hKey);

	return value == 1; // Если значение 1, защита DMA включена
}



namespace fs = std::filesystem;

void create_needed_directories_for_configs_and_lists() {

	fs::create_directories(_("C:\\Windows\\AppSave\\Configs\\"));
	fs::create_directories(_("C:\\Windows\\AppSave\\Lists\\"));
}


std::string unix_time_to_readable_string_time(time_t unix_time) {

	std::tm tm_buff{};
	localtime_s(&tm_buff, &unix_time);

	char buffer[64];
	//std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm_buff);
	std::strftime(buffer, sizeof(buffer), _("%d-%m-%Y %H:%M"), &tm_buff);

	return std::string(buffer);
}
