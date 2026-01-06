#pragma once
// hook_manager.h - Hook Manager Utilities

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <vector>

namespace HookManager 
{
    // Finds byte pattern in memory (used to locate DirectX 9 VMT)
    // pattern: byte sequence (-1 = wildcard)
    uintptr_t FindMemoryPattern(uintptr_t startAddress, size_t searchLength, const std::vector<int>& pattern);

    // Finds game window (searches for "Conquer" title or class "Afx:00400000:0:000100")
    HWND FindGameWindowHandle();
}
