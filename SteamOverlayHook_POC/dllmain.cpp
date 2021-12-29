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
#include <string>
#include "Detours/detours.h"
#include "GameClass.h"
#include <sstream>
#include <fstream>

#include <direct.h>
#pragma comment(lib, "detours.lib")
#pragma endregion headers


ID3D11RenderTargetView* rendertarget;
ID3D11DeviceContext* context;
ID3D11Device* device;
HRESULT(*present_original)(IDXGISwapChain* swapchain, UINT sync, UINT flags) = nullptr;
typedef double(*LogFn)(int a1, const char* text, __int64 a3, int a4, __int64 a5);
int X, Y;
HWND hwnd;
ActorArray SCAM;
int acount;
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
    if (GetAsyncKeyState(VK_INSERT))
    {
        acount = 0;
        for (size_t i = 0; i <= acount; i++)
        {
            SCAM.pActor[i] = nullptr;
        }
    }
    Vector3 scrPos;
    Engine::Worldtoscreen(&scrPos, { 500,18,400 });
    for (size_t i = 0; i <= acount; i++)
    {
        if (SCAM.pActor[i] != nullptr)
        {
            if (Engine::Worldtoscreen(&scrPos, SCAM.pActor[i]->Position))
            {
               window.DrawList->AddText(ImVec2(scrPos.x, scrPos.y), ImColor{ 255,1,1,255 }, "NEGR");
            }
        }
    }
    window.DrawList->AddCircle(ImVec2{ scrPos.x,scrPos.y }, 10, ImColor{ 255,0,0,255 }, 12, 2);

    window.DrawList->PushClipRectFullScreen();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
    ImGui::Render();

    return present_original(swapchain, sync, flags);
}
// bool __fastcall sub_14092A340(__int64 a1, unsigned __int16 TemplateId, _QWORD *a3)
//  int __fastcall sub_1418A12F0(__int64 a1, float a2, float xmm2_4_0, double a4, __int64 a5)
// 41 57 41 56 41 55 41 54 56 57 55 53 48 81 EC A8 0C 00 00 44 0F 29 BC 24 90 0C 00 00 66 44 0F 29 B4 24 80 0C 00 00
typedef bool(*TemplateHook)(__int64 a1, unsigned __int16 TemplateId, __int64* result);
typedef int(*ActorUpdateHook)(__int64 a1, float a2, float xmm2_4_0, double a4, __int64 a5);
__int64 templhookadr = Utils::sigscan(0, "41 57 41 56 41 55 41 54 56 57 55 53 48 81 EC A8 0C 00 00 44 0F 29 BC 24 90 0C 00 00 66 44 0F 29 B4 24 80 0C 00 00");
__int64 hookadr= Utils::sigscan(0,"41 57 41 56 41 55 41 54 56 57 55 53 48 83 EC 48 80 3D ? ? ? ? ? 0F 85 ? ? ? ? 44 89 CB 4D");
bool temphookedFn(__int64 a1, float a2, float xmm2_4_0, double a4, __int64 a5)
{
    ActorUpdateHook origFn = (ActorUpdateHook)(templhookadr);

    __int64 v586 = *(__int64*)(a5);
    Actor* pActor = (Actor*) v586;
    for (size_t i = 0; i <= 200; i++)
    {
        if (SCAM.pActor[i] == pActor)
            break;
        if (i == acount)
        {
            SCAM.pActor[acount] = pActor;
            acount++;
            break;
        }
        if (acount == 200)
        {
            acount = 0;
            break;
        }
    }

    return origFn(a1, a2, xmm2_4_0, a4, a5);
}
double hookedFn(int a1, const char* text, __int64 a3, int a4, __int64 a5)
{
    LogFn origFn = (LogFn)(hookadr);
    return origFn(1, "NIGERS", a3, a4, a5);
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

    initmyhook();
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

