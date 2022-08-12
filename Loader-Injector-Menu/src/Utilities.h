#include <cstdio>
#include <windows.h>
#include <tlhelp32.h>

namespace Utilities {
    HANDLE GetProcessByName(const char* procname);
    bool InjectDll(HANDLE processHandle, const char* dllPath);
}