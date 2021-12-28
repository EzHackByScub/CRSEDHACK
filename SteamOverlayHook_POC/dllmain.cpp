#define _CRT_SECURE_NO_WARNINGS

#pragma region headers
#include <Windows.h>
#include <iostream>
#include <D3DX11.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_internal.h"
#include "Engine.h"
#include "Utils.h"
#include "Addr.h"
#include "Detours/detours.h"
#include <string>
#pragma comment(lib, "detours.lib")
#pragma endregion headers


ID3D11RenderTargetView* rendertarget;
ID3D11DeviceContext* context;
ID3D11Device* device;
HRESULT(*present_original)(IDXGISwapChain* swapchain, UINT sync, UINT flags) = nullptr;
typedef double(*LogFn)(int a1, const char* text, __int64 a3, int a4, __int64 a5);
int X, Y;
HWND hwnd;

HRESULT present_hooked(IDXGISwapChain* swapchain, UINT sync, UINT flags)
{
    if (!device)
    {
        ID3D11Texture2D* renderTarget = 0; 
        ID3D11Texture2D* backBuffer = 0;
        D3D11_TEXTURE2D_DESC backBufferDesc = { 0 };

        swapchain->GetDevice(__uuidof(device), (PVOID*)&device);
        device->GetImmediateContext(&context);

        swapchain->GetBuffer(0, __uuidof(renderTarget), (PVOID*)&renderTarget);
        device->CreateRenderTargetView(renderTarget, nullptr, &rendertarget);
        renderTarget->Release();

        swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (PVOID*)&backBuffer);
        backBuffer->GetDesc(&backBufferDesc);

        X = backBufferDesc.Width;
        Y = backBufferDesc.Height;

        backBuffer->Release();

        if (!hwnd)
        {
            //Change window name and class for the specific game, to check use spy++ its in the tool tab of visual studio :D
            //For loading a non steam game for example rogue company press add a game -> add a non-steam game1 
            hwnd = FindWindowW(L"DagorWClass", L"CRSED");
            //if not found try to use foreground window?
            if (!hwnd)
                hwnd = GetForegroundWindow();
        }

        ImGui_ImplDX11_Init(hwnd, device, context);
        ImGui_ImplDX11_CreateDeviceObjects();
    }
    context->OMSetRenderTargets(1, &rendertarget, nullptr);

    ImGui_ImplDX11_NewFrame();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
    ImGui::Begin("##scene", nullptr, ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar);
    ImGuiIO& IO = ImGui::GetIO();
    ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetWindowSize(ImVec2(IO.DisplaySize.x, IO.DisplaySize.y), ImGuiCond_Always);
    ImGuiWindow& window = *ImGui::GetCurrentWindow();



    window.DrawList->AddCircle({ 123,123 }, 30, ImColor{ 255,2,2,255 }, 12, 3);

    Vector3 scrPos;
    Engine::Worldtoscreen(&scrPos, { 500,18,400 });
    window.DrawList->AddCircle(ImVec2{ scrPos.x,scrPos.y }, 10, ImColor{ 255,0,0,255 }, 12, 2);

    window.DrawList->PushClipRectFullScreen();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
    ImGui::Render();

    return present_original(swapchain, sync, flags);
}
// bool __fastcall sub_14092A340(__int64 a1, unsigned __int16 TemplateId, _QWORD *a3)
// 
// 7DEEC0
typedef bool(*TemplateHook)(__int64 a1, unsigned __int16 TemplateId, __int64* result);
__int64 hookadr = Addr::base_address + 0x7DEEC0;
__int64 templhookadr = Addr::base_address + 0x92A340;
bool temphookedFn(__int64 a1, unsigned __int16 TemplateId, __int64* result)
{
    TemplateHook origFn = (TemplateHook)(templhookadr);
    std::cout << std::hex <<a1  <<" a1 [Executed TamplateId] " << TemplateId << std::endl;

    return origFn(a1, TemplateId, result);
}
double hookedFn(int a1, const char* text, __int64 a3, int a4, __int64 a5)
{
    LogFn origFn = (LogFn)(hookadr);
        origFn(1, "NIGERS", a3, a4, a5);    
    return origFn(a1, text, a3, a4, a5);
}
void initmyhook()
{

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)hookadr, &hookedFn);
    DetourAttach(&(PVOID&)templhookadr, &temphookedFn);
    DetourTransactionCommit();
}

//uintptr_t LogFunctionAddres = Utils::sigscan(0, "s");
//B27850

void hook(__int64 addr, __int64 func, __int64* orig)
{
    static uintptr_t hook_addr;
    if (!hook_addr)
        hook_addr = Utils::sigscan("GameOverlayRenderer64.dll", "48 ? ? ? ? 57 48 83 EC 30 33 C0");
    auto hook = ((__int64(__fastcall*)(__int64 addr, __int64 func, __int64* orig, __int64 smthng))(hook_addr));
    hook((__int64)addr, (__int64)func, orig, (__int64)1);
}

void init()
{
    if (!GetModuleHandleA("GameOverlayRenderer64.dll"))
    {
        exit(0);
    }
    AllocConsole();
    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    uintptr_t Steam_DXGI_PresentScene = Utils::sigscan("GameOverlayRenderer64.dll", "48 89 6C 24 18 48 89 74 24 20 41 56 48 83 EC 20 41 8B E8");
    if (Steam_DXGI_PresentScene)
        hook(Steam_DXGI_PresentScene, (__int64)present_hooked, (__int64*)&present_original);
    initmyhook();
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
        init();

    return TRUE;
}

