// hook_manager.cpp - Hook Manager Implementation

#include "hook_manager.h"
#include "../dx9/dx9_hook.h"

extern HWND g_parentWindowHandle;

namespace HookManager 
{
    uintptr_t FindMemoryPattern(uintptr_t startAddress, size_t searchLength, const std::vector<int>& pattern) 
    {
        const uint8_t* memoryData = reinterpret_cast<const uint8_t*>(startAddress);
        size_t patternLength = pattern.size();

        for (size_t i = 0; i <= searchLength - patternLength; ++i) 
        {
            bool patternMatches = true;
            
            for (size_t j = 0; j < patternLength; ++j) {
                if (pattern[j] != -1 && pattern[j] != memoryData[i + j]) 
                {
                    patternMatches = false;
                    break;
                }
            }

            if (patternMatches) 
                return startAddress + i;
        }
        return 0;
    }

    HWND FindGameWindowHandle() 
    {
        HWND parentWindowHandle = NULL;

        auto enumWindowsCallback = [](HWND windowHandle, LPARAM lParam) -> BOOL 
        {
            DWORD processId = 0;
            GetWindowThreadProcessId(windowHandle, &processId);

            char windowTitle[256] = { 0 };
            GetWindowTextA(windowHandle, windowTitle, sizeof(windowTitle));

            char className[256] = { 0 };
            GetClassNameA(windowHandle, className, sizeof(className));

            if (GetCurrentProcessId() == processId &&
                (strstr(windowTitle, "Conquer") != nullptr || 
                 strstr(className, "Afx:00400000:0:000100") != nullptr))
            {
                *reinterpret_cast<HWND*>(lParam) = windowHandle;
                return FALSE;
            }
            return TRUE;
        };

        EnumWindows(enumWindowsCallback, reinterpret_cast<LPARAM>(&parentWindowHandle));

        if (parentWindowHandle == NULL) 
            return NULL;

        ::g_parentWindowHandle = parentWindowHandle;

        return FindWindowExA(parentWindowHandle, NULL, "#32770", NULL);
    }
}
