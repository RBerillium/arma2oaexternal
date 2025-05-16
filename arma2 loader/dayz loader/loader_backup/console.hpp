#pragma once

#include <stdio.h>
#include "xor.hpp"

class console_c
{
public:
    void success(const char* const format, ...)
    {
        set_color(color::green, color::black);
        printf(_("[+] "));

        va_list list; char text[1024] = { 0 }; va_start(list, format);
        vsnprintf_s(text, sizeof(text), format, list); va_end(list);

        printf(_("%s"), text);
    }

    void info(const char* const format, ...)
    {
        set_color(color::cyan, color::black);
        printf(_("[*] "));

        va_list list; char text[1024] = { 0 }; va_start(list, format);
        vsnprintf_s(text, sizeof(text), format, list); va_end(list);

        printf(_("%s"), text);
    }

    void warning(const char* const format, ...)
    {
        set_color(color::yellow, color::black);
        printf(_("[!] "));

        va_list list; char text[1024] = { 0 }; va_start(list, format);
        vsnprintf_s(text, sizeof(text), format, list); va_end(list);

        printf(_("%s"), text);
    }

    void error(const char* const format, ...)
    {
        set_color(color::red, color::black);
        printf(_("[-] "));

        va_list list; char text[1024] = { 0 }; va_start(list, format);
        vsnprintf_s(text, sizeof(text), format, list); va_end(list);

        printf(_("%s"), text);
    }

    void title(const char* const format, ...)
    {
        va_list list; char text[1024] = { 0 }; va_start(list, format);
        vsnprintf_s(text, sizeof(text), format, list); va_end(list);

        SetConsoleTitleA(text);
    }

private:

    enum color
    {
        black = 0, blue = 1, green = 2, cyan = 3, red = 4, magenta = 5, brown = 6,
        light_gray = 7, dark_gray = 8, light_blue = 9, light_green = 10, light_cyan = 11, light_red = 12, light_magenta = 13, yellow = 14, white = 15
    };

    void set_color(color text, color background)
    {
        HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(console_handle, (WORD)((background << 4) | text));
    };  
};