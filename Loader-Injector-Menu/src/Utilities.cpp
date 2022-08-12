#include "Utilities.h"
#include <vector>

bool Utilities::InjectDll(HANDLE processHandle, const char* dllPath) {
    // Clear Up Memory For DLL At Address
    void* address = VirtualAllocEx(processHandle, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    // Check If The Address Is Valid
    if (address == 0)
        return FALSE;

    // Write The Dll To The Memory With The Size of THe Dll
    WriteProcessMemory(processHandle, address, dllPath, 1 + strlen(dllPath), NULL);

    // Then Load The Dll At The Freed Up Space At The Address
    HANDLE hThread = CreateRemoteThread(processHandle, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, address, 0, 0);

    // Check If The Handle Is Valid
    if (hThread == NULL) {
        // If Thread Is Not Valid Then Patch The Allocated Memory And Return False
        VirtualFreeEx(processHandle, address, 0, MEM_RELEASE);

        return FALSE;
    }

    // Close Handle And Return True After Injecting
    CloseHandle(hThread);
    return TRUE;
}

HANDLE Utilities::GetProcessByName(const char* procname)
{
    // Process ID
    DWORD pid = 0;

    // Create toolhelp snapshot.
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 process;
    process.dwSize = sizeof(process);

    // Walkthrough all processes.
    if (Process32First(snapshot, &process))
    {
        do
        {
            // Compare process.szExeFile based on format of name, i.e., trim file path
            // trim .exe if necessary, etc.
            if (strcmp(procname, process.szExeFile) == 0)
            {
                pid = process.th32ProcessID;
                break;
            }
        } while (Process32Next(snapshot, &process));
    }

    CloseHandle(snapshot);

    // Checks if the process exists
    if (pid != 0)
    {
        // Returns the processes handle if it exists
        return OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    }

    return NULL;
}