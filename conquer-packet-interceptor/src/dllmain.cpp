// dllmain.cpp - DLL Entry Point

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include "libs/minhook/include/MinHook.h"
#include "dx9/dx9_hook.h"
#include "network/packet_hook.h"
#include "core/hook_manager.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "libMinHook.x86.lib")

static void HookInitializationThread() 
{
    while (!GetModuleHandleA("d3d9.dll")) 
        Sleep(100);

    MH_Initialize();

    if (!InstallDirectX9Hooks()) 
        return;

    InstallWindowProcedureHook();
    PacketHook_Init();

    while (true) 
    {
        Sleep(16);
        if (GetAsyncKeyState(VK_INSERT) & 1) 
        {
            ToggleImGuiWindow();
            Sleep(200);
        }
    }
}

BOOL APIENTRY DllMain(HMODULE moduleHandle, DWORD reason, LPVOID reserved) 
{
    switch (reason) 
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(moduleHandle);
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)HookInitializationThread, NULL, 0, NULL);
        break;
        
    case DLL_PROCESS_DETACH:
        
        PacketHook_Shutdown();
        ShutdownDirectX9Hooks();
        MH_Uninitialize();
        break;
    }
    return TRUE;
}
