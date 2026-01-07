// dx9_hook.cpp - DirectX 9 Hooks Implementation

#include "dx9_hook.h"
#include "../ui/packet_viewer.h"
#include "../core/hook_manager.h"
#include "../libs/minhook/include/MinHook.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx9.h"
#include "../imgui/imgui_impl_win32.h"
#include <vector>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Dx9Context g_dx9Context;
HWND g_parentWindowHandle = NULL;
static bool g_isImGuiInitialized = false;
static HWND g_gameWindowHandle = NULL;
static WNDPROC g_originalWindowProcedure = NULL;
static bool g_isGuiWindowOpen = true;

HRESULT WINAPI HookedEndScene(LPDIRECT3DDEVICE9 device) 
{
    if (!g_dx9Context.device) 
        g_dx9Context.device = device;

    if (!g_isImGuiInitialized) 
    {
        if (!g_gameWindowHandle) 
            g_gameWindowHandle = HookManager::FindGameWindowHandle();
        
        if (g_gameWindowHandle && g_parentWindowHandle) 
        {
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
            
            ImGui_ImplWin32_Init(g_parentWindowHandle);
            ImGui_ImplDX9_Init(device);
            g_isImGuiInitialized = true;
        }
    }

    if (g_isImGuiInitialized && g_isGuiWindowOpen) 
    {
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        RenderImGuiInterface();
        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    }

    return g_dx9Context.originalEndScene(device);
}

HRESULT WINAPI HookedReset(LPDIRECT3DDEVICE9 device, D3DPRESENT_PARAMETERS* presentationParameters) 
{
    if (g_isImGuiInitialized) 
        ImGui_ImplDX9_InvalidateDeviceObjects();

    HRESULT result = g_dx9Context.originalReset(device, presentationParameters);

    if (SUCCEEDED(result) && g_isImGuiInitialized) 
        ImGui_ImplDX9_CreateDeviceObjects();

    return result;
}

LRESULT CALLBACK HookedWindowProcedure(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam) 
{
    if (g_isImGuiInitialized) 
    {
        if (ImGui_ImplWin32_WndProcHandler(windowHandle, message, wParam, lParam))
            return 1;
        
        ImGuiIO& io = ImGui::GetIO();
        if (g_isGuiWindowOpen && (io.WantCaptureMouse || io.WantCaptureKeyboard)) 
        {
            switch (message) 
            {
            case WM_LBUTTONDOWN: 
            case WM_LBUTTONUP: 
            case WM_LBUTTONDBLCLK:
            case WM_RBUTTONDOWN: 
            case WM_RBUTTONUP: 
            case WM_RBUTTONDBLCLK:
            case WM_MBUTTONDOWN: 
            case WM_MBUTTONUP: 
            case WM_MBUTTONDBLCLK:
            case WM_MOUSEWHEEL: 
            case WM_MOUSEHWHEEL:
            case WM_KEYDOWN: 
            case WM_KEYUP: 
            case WM_SYSKEYDOWN: 
            case WM_SYSKEYUP:
            case WM_CHAR: 
            case WM_IME_CHAR: 
            case WM_IME_COMPOSITION:
                return 0;
            default:
                break;
            }
        }
    }
    
    return CallWindowProcA(g_originalWindowProcedure, windowHandle, message, wParam, lParam);
}

// Locates DirectX 9 VMT using pattern scanning (VMT index 42 = EndScene, 16 = Reset)
bool InstallDirectX9Hooks() 
{
    HMODULE d3d9Module = nullptr;

    while (!(d3d9Module = GetModuleHandleA("d3d9.dll"))) 
        Sleep(100);

    uintptr_t d3d9Base = reinterpret_cast<uintptr_t>(d3d9Module);

    size_t moduleSize = 0x100000;

    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)d3d9Module;

    if (dosHeader && dosHeader->e_magic == IMAGE_DOS_SIGNATURE) 
    {
        PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)(d3d9Base + dosHeader->e_lfanew);

        if (ntHeaders && ntHeaders->Signature == IMAGE_NT_SIGNATURE) 
            moduleSize = ntHeaders->OptionalHeader.SizeOfImage;
    }

    std::vector<int> vmtPattern = { 0xC7, 0x06, -1, -1, -1, -1, 0x89, 0x86, -1, -1, -1, -1, 0x89, 0x86 };
    
    uintptr_t vmtBase = HookManager::FindMemoryPattern(d3d9Base, moduleSize, vmtPattern);

    if (!vmtBase) 
        return false;

    uintptr_t* vmt = *reinterpret_cast<uintptr_t**>(vmtBase + 2);
    EndSceneFunc originalEndScene = reinterpret_cast<EndSceneFunc>(vmt[42]);
    ResetFunc originalReset = reinterpret_cast<ResetFunc>(vmt[16]);

    g_dx9Context.originalEndScene = originalEndScene;
    g_dx9Context.originalReset = originalReset;
    g_dx9Context.originalEndSceneAddress = (LPVOID)originalEndScene;
    g_dx9Context.originalResetAddress = (LPVOID)originalReset;

    MH_CreateHook(g_dx9Context.originalEndSceneAddress, (LPVOID)HookedEndScene, (LPVOID*)&g_dx9Context.originalEndScene);
    MH_CreateHook(g_dx9Context.originalResetAddress, (LPVOID)HookedReset, (LPVOID*)&g_dx9Context.originalReset);

    MH_EnableHook(g_dx9Context.originalEndSceneAddress);
    MH_EnableHook(g_dx9Context.originalResetAddress);

    g_dx9Context.isInitialized = true;
    return true;
}

void InstallWindowProcedureHook() 
{
    while (!g_gameWindowHandle) 
    {
        g_gameWindowHandle = HookManager::FindGameWindowHandle();
        Sleep(100);
    }
    
    if (g_gameWindowHandle && !g_originalWindowProcedure) 
    {
        g_originalWindowProcedure = (WNDPROC)SetWindowLongPtrA(g_gameWindowHandle, GWLP_WNDPROC, (LONG_PTR)HookedWindowProcedure);
    }
}

void ToggleImGuiWindow() 
{
    g_isGuiWindowOpen = !g_isGuiWindowOpen;
}

bool IsImGuiWindowOpen() 
{
    return g_isGuiWindowOpen;
}

void ShutdownDirectX9Hooks() 
{
    __try 
    {
        if (g_gameWindowHandle != NULL && g_originalWindowProcedure != NULL) 
        {
            // Verify window is still valid before attempting to restore WNDPROC
            if (IsWindow(g_gameWindowHandle)) 
            {
                SetWindowLongPtrA(g_gameWindowHandle, GWLP_WNDPROC, (LONG_PTR)g_originalWindowProcedure);
                g_originalWindowProcedure = NULL;
            }
        }
    }
    __except(EXCEPTION_EXECUTE_HANDLER) 
    {

    }

    __try 
    {
        if (g_isImGuiInitialized) 
        {

            if (g_dx9Context.device != nullptr) 
            {
                ImGui_ImplDX9_Shutdown();
            }
            
            // Shutdown Win32 implementation (safe to call even if DX9 failed)
            ImGui_ImplWin32_Shutdown();
            
            // Destroy ImGui context (releases all fonts, textures, and internal state)
            ImGui::DestroyContext();
            
            g_isImGuiInitialized = false;
        }
    }
    __except(EXCEPTION_EXECUTE_HANDLER) 
    {
        // If ImGui cleanup fails (resources may have been freed by the game),
        g_isImGuiInitialized = false;
    }

    __try 
    {
        if (g_dx9Context.isInitialized) 
        {
            // Disable EndScene hook first
            if (g_dx9Context.originalEndSceneAddress != nullptr) 
            {
                MH_DisableHook(g_dx9Context.originalEndSceneAddress);
                MH_RemoveHook(g_dx9Context.originalEndSceneAddress);
            }
            
            // Disable Reset hook
            if (g_dx9Context.originalResetAddress != nullptr) 
            {
                MH_DisableHook(g_dx9Context.originalResetAddress);
                MH_RemoveHook(g_dx9Context.originalResetAddress);
            }
            
            g_dx9Context.isInitialized = false;
        }
    }
    __except(EXCEPTION_EXECUTE_HANDLER) 
    {
        g_dx9Context.isInitialized = false;
    }

    g_dx9Context.device = nullptr;
}