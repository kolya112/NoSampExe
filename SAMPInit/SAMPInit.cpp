#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <tchar.h>

BOOL Inject(DWORD pId, LPSTR dllName)
{
    HANDLE h = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pId);
    if (h)
    {
        LPVOID LoadLibAddr = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
        LPVOID dereercomp = VirtualAllocEx(h, NULL, strlen(dllName), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        WriteProcessMemory(h, dereercomp, dllName, strlen(dllName), NULL);
        HANDLE asdc = CreateRemoteThread(h, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibAddr, dereercomp, 0, NULL);
        WaitForSingleObject(asdc, INFINITE);
        VirtualFreeEx(h, dereercomp, strlen(dllName), MEM_RELEASE);
        CloseHandle(asdc);
        CloseHandle(h);
        return TRUE;
    }
    return FALSE;
}

int main()
{
    Sleep(5000);
    STARTUPINFO cif;
    ZeroMemory(&cif, sizeof(STARTUPINFO));
    PROCESS_INFORMATION pi;
    TCHAR czCommandLine[] = "-c -n Sharp_Macalister -h 176.32.39.129 -p 7777";
    if (CreateProcess("gta_sa.exe", czCommandLine, NULL, NULL, FALSE, DETACHED_PROCESS | CREATE_SUSPENDED, NULL, NULL, &cif, &pi))
    {
        if (pi.hProcess != NULL)
        {
            TCHAR dllname[] = "samp.dll";
            if (!Inject(pi.dwProcessId, dllname))
            {
                TerminateProcess(pi.hProcess, 0);
                ExitProcess(0);
            }
            ResumeThread(pi.hThread);
        }
    }
    else MessageBox(NULL, "Failed to Create Process", "Error", MB_ICONERROR);
}