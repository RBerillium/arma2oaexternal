#pragma once
#include <windows.h>
#include <wininet.h>
#include <iostream>
#include <fstream>
#include "xor.hpp"

#pragma comment(lib, "wininet.lib")

void download_json_data() {
    HINTERNET hInternet, hConnect, hRequest;
    const char* host = _("api.tarkov.dev");
    const char* path = _("/graphql");
    const char* data =_(R"({"query": "{ items(lang: en) { id name shortName avg24hPrice basePrice width height } }"})");

    // Открываем сессию
    hInternet = InternetOpen(_("WinINet Client"), INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) {

        return;
    }

    // Подключаемся к серверу
    hConnect = InternetConnect(hInternet, host, INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    if (!hConnect) {

        InternetCloseHandle(hInternet);
        return;
    }

    // Заголовки HTTP
    const char* headers =_("Content-Type: application/json\r\n");

    // Создаём запрос
    hRequest = HttpOpenRequest(hConnect, _("POST"), path, NULL, NULL, NULL, INTERNET_FLAG_SECURE | INTERNET_FLAG_RELOAD, 0);
    if (!hRequest) {

        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return;
    }

    // Отправляем запрос
    if (!HttpSendRequest(hRequest, headers, strlen(headers), (LPVOID)data, strlen(data))) {

        InternetCloseHandle(hRequest);
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return;
    }

    // Читаем ответ
    char buffer[4096];
    DWORD bytesRead;
    std::string response;

    while (InternetReadFile(hRequest, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead) {
        buffer[bytesRead] = '\0';  // Добавляем завершающий нуль
        response.append(buffer, bytesRead);
    }

    // Записываем ответ в JSON-файл
    char buff[MAX_PATH];
    GetTempPathA(sizeof(buff), buff);
    std::string json_path(buff);
    json_path += _("prices.json");
    std::ofstream file(json_path);
    if (file.is_open()) {
        file << response;
        file.close();
    }


    // Закрываем соединения
    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
}

