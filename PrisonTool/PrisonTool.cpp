#include <windows.h>
#include <iostream>
#include <vector>
#include <D3D9Types.h>
#include <TlHelp32.h>

using namespace std;

uintptr_t FindDMAAddy(HANDLE hProcess, uintptr_t ptr, std::vector<unsigned int> healthoffsets)
{
    uintptr_t addr = ptr;
    for (unsigned int i = 0; i < healthoffsets.size(); ++i)
    {
        ReadProcessMemory(hProcess, (LPVOID)addr, &addr, sizeof(addr), NULL);
        addr += healthoffsets[i];
    }
    return addr;
}

uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName)
{
    uintptr_t modBaseAddr = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
    if (hSnap != INVALID_HANDLE_VALUE)
    {
        MODULEENTRY32 modEntry;
        modEntry.dwSize = sizeof(modEntry);
        if (Module32First(hSnap, &modEntry))
        {
            do
            {
                if (!_wcsicmp(modEntry.szModule, modName))
                {
                    modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
                    break;
                }
            } while (Module32Next(hSnap, &modEntry));
        }
    }
    CloseHandle(hSnap);
    return modBaseAddr;
}

int main()
{
    HWND hwnd = FindWindowA(0, "Ancient Warfare 3");
    
    if (hwnd == NULL)
    {
        cout << "Error" << endl;
    }
    else

    {
        FreeConsole();


        DWORD procID;

        GetWindowThreadProcessId(hwnd, &procID);

        HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);

        uintptr_t modulebase = GetModuleBaseAddress(procID, L"GameAssembly.dll");

        uintptr_t healthptrbaseaddr = modulebase + 0x0273DB08;
        uintptr_t ammoptrbaseaddr = modulebase + 0x0274CC80;

        std::vector<unsigned int> healthoffsets = { 0xB8, 0x48, 0x1A0, 0x10, 0x148, 0x60, 0x18 };
        std::vector<unsigned int> ammooffsets = { 0xB8, 0x0, 0x50, 0x28, 0x78, 0x10, 0x104 };

        float PHealth = 99999;
        int PAmmo = 99999;

        //Health
        uintptr_t healthaddr = FindDMAAddy(hProcess, healthptrbaseaddr, healthoffsets);
        WriteProcessMemory(hProcess, (LPVOID)healthaddr, &PHealth, sizeof(PHealth), NULL);
        std::cout << PHealth << " <-- pulled to -->  " << (LPVOID)healthaddr << endl;

        // Ammo
        uintptr_t ammoaddr = FindDMAAddy(hProcess, ammoptrbaseaddr, ammooffsets);
        WriteProcessMemory(hProcess, (LPVOID)ammoaddr, &PAmmo, sizeof(PAmmo), NULL);
        std::cout << PAmmo << " <-- pulled to -->  " << (LPVOID)ammoaddr << endl;

        exit(-1);
    }

    return 0;

}