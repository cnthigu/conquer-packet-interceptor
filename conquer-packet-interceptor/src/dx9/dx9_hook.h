#pragma once
// dx9_hook.h - DirectX 9 Hooks for ImGui Overlay

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <d3d9.h>

// DirectX 9 function types
typedef HRESULT(WINAPI* EndSceneFunc)(LPDIRECT3DDEVICE9);
typedef HRESULT(WINAPI* ResetFunc)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);

// DirectX 9 context structure
struct Dx9Context {
    LPDIRECT3DDEVICE9 device;
    EndSceneFunc originalEndScene;
    ResetFunc originalReset;
    LPVOID originalEndSceneAddress;
    LPVOID originalResetAddress;
    bool isInitialized;

    Dx9Context() : device(nullptr), originalEndScene(nullptr), originalReset(nullptr),
        originalEndSceneAddress(nullptr), originalResetAddress(nullptr), isInitialized(false) {}
};

// Global variables
extern Dx9Context g_dx9Context;
extern HWND g_parentWindowHandle;

// Hook functions
HRESULT WINAPI HookedEndScene(LPDIRECT3DDEVICE9 device);
HRESULT WINAPI HookedReset(LPDIRECT3DDEVICE9 device, D3DPRESENT_PARAMETERS* presentationParameters);
LRESULT CALLBACK HookedWindowProcedure(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);

// Hook installation
bool InstallDirectX9Hooks();
void InstallWindowProcedureHook();

// ImGui window control
void ToggleImGuiWindow();
bool IsImGuiWindowOpen();
