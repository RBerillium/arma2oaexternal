#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE
#include <windows.h>
#include <stdio.h>
#include <dwmapi.h> 
#include <iostream>
#include <thread>


#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"
#include <d3d11.h>
#include <d3dx11tex.h>
#include "xor.h"
//#include "..//menubackground.h"



#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "D3DX11.lib")

#include "font.h"
ID3D11ShaderResourceView* Image = nullptr;


namespace Nvidia
{
    HWND window = NULL;
    ID3D11Device* device = NULL;
    IDXGISwapChain* swap_chain = NULL;
    ID3D11DeviceContext* device_context = NULL;
    ID3D11RenderTargetView* render_target_view = NULL;

    ImFont* main_font;

    ImFont* menu_font;

    ImFont* ammo_font;

    void suspend_process(uint32_t process, uint32_t thread)
    {
        HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
        if (handle != INVALID_HANDLE_VALUE)
        {
            THREADENTRY32 thread_entry; thread_entry.dwSize = sizeof(thread_entry);
            if (Thread32First(handle, &thread_entry))
            {
                do
                {
                    if (thread_entry.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(thread_entry.th32OwnerProcessID))
                    {
                        if (thread_entry.th32OwnerProcessID == process && thread_entry.th32ThreadID != thread)
                        {
                            HANDLE open_thread = OpenThread(THREAD_ALL_ACCESS, FALSE, thread_entry.th32ThreadID);
                            if (open_thread != NULL)
                            {
                                SuspendThread(open_thread);
                                CloseHandle(open_thread);
                            }
                        }
                    }
                    thread_entry.dwSize = sizeof(thread_entry);
                } while (Thread32Next(handle, &thread_entry));
            }
        }
    }
    bool initialization()
    {
        while (!window)
        {
            // window = FindWindowA(_("CEF-OSC-WIDGET"), _("NVIDIA GeForce Overlay DT"));
            //window = FindWindowA(xor ("CGdiCapWindowWnd"), xor (""));
          
            window = FindWindowA(_("Chrome_WidgetWin_1"), _("Discord Overlay"));
            Sleep(10);
        }

        //Sleep(10);

        //auto info = GetWindowLongA(window, -20);

        //if (!info)
        //    return false;

        //MARGINS margin;
        //margin.cyBottomHeight = margin.cyTopHeight = margin.cxLeftWidth = margin.cxRightWidth = -1;

        //if (DwmExtendFrameIntoClientArea(window, &margin) != S_OK)
        //    return false;

        //if (!SetLayeredWindowAttributes(window, 0x000000, 0xFF, 0x02))
        //    return false;
        ////SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0, 0x0002 | 0x0001);
        //ShowWindow(window, SW_SHOW);

        DXGI_SWAP_CHAIN_DESC swap_chain_desc;
        swap_chain_desc.BufferCount = 2;
        swap_chain_desc.BufferDesc.Width = 0;
        swap_chain_desc.BufferDesc.Height = 0;
        swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swap_chain_desc.BufferDesc.RefreshRate.Numerator = 1000;
        swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
        swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swap_chain_desc.OutputWindow = window;
        swap_chain_desc.SampleDesc.Count = 1;
        swap_chain_desc.SampleDesc.Quality = 0;
        swap_chain_desc.Windowed = true;
        swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        D3D_FEATURE_LEVEL feature_level;
        const D3D_FEATURE_LEVEL feature_level_array[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
        D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, feature_level_array, 2, D3D11_SDK_VERSION, &swap_chain_desc, &swap_chain, &device, &feature_level, &device_context);

        ID3D11Texture2D* back_buffer{ };
        swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));
        device->CreateRenderTargetView(back_buffer, NULL, &render_target_view);

        back_buffer->Release();
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(window);
        ImGui_ImplDX11_Init(device, device_context);

        ImGuiStyle* style = &ImGui::GetStyle();
        ImGuiIO* io = &ImGui::GetIO();




        // ImFontConfig cfg;

        // cfg.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_MonoHinting | ImGuiFreeTypeBuilderFlags_Monochrome;
        main_font = io->Fonts->AddFontFromFileTTF(xor ("C:\\Windows\\Fonts\\Arial.ttf"), 12.0f, NULL, io->Fonts->GetGlyphRangesCyrillic());

        // cfg.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_MonoHinting | ImGuiFreeTypeBuilderFlags_Monochrome;
         //menu_font = io->Fonts->AddFontFromFileTTF(_("C:\\Windows\\Fonts\\Arial.ttf"), 14.f, NULL, io->Fonts->GetGlyphRangesCyrillic());

         //cfg.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_MonoHinting | ImGuiFreeTypeBuilderFlags_Monochrome;
         //ammo_font = io->Fonts->AddFontFromFileTTF(_("C:\\Windows\\Fonts\\Arial.ttf"), 14.f, NULL, io->Fonts->GetGlyphRangesCyrillic());

        return true;
    }

    void start()
    {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }

    void end()
    {
        ImGui::Render();
        ImVec4 clear = { 0.0f, 0.0f, 0.0f, 0.0f };
        device_context->OMSetRenderTargets(1, &render_target_view, NULL);
        device_context->ClearRenderTargetView(render_target_view, (float*)&clear);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        swap_chain->Present(0, 0);
    }

}
