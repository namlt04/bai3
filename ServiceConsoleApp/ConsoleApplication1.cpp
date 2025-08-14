// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <tchar.h>
#include <Windows.h>
#include <stdio.h>
#include <time.h>
#include <atlstr.h>
#include <sddl.h>
#include <Wtsapi32.h>      // WTSQueryUserToken
#include <Userenv.h>       // CreateEnvironmentBlock (nếu cần)
#include <Securitybaseapi.h> // DuplicateTokenEx
#define SERVICE_NAME L"myservice"
#pragma comment(lib, "Wtsapi32.lib")
#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "Userenv.lib")

SERVICE_STATUS g_ServiceStatus = {}; 
SERVICE_STATUS_HANDLE g_StatusHandle = nullptr; 
CStringW g_PathLog = L"D:\\test.log";
HANDLE g_ServiceStopEvent = INVALID_HANDLE_VALUE;
HANDLE g_WorkerThread = nullptr; 
HANDLE g_hPipe = nullptr;
void WINAPI ServiceHandle(DWORD ctrl)
{
    switch (ctrl)
    {
    case SERVICE_CONTROL_STOP: 
        g_ServiceStatus.dwControlsAccepted = 0; 
        g_ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
        SetServiceStatus(g_StatusHandle, &g_ServiceStatus); 
        SetEvent(g_ServiceStopEvent); 
        break; 
    default : 
        break;
    }
}
BOOL WriteLog(CString msg)
{
    HANDLE hFile = CreateFile(g_PathLog, FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }
    DWORD byteWritten = 0;
    WriteFile(hFile, msg.GetBuffer(), msg.GetLength() * sizeof(wchar_t), &byteWritten, NULL);

    CloseHandle(hFile);
    return TRUE;




}
BOOL RunAppAsCurrentUser(LPCWSTR appPath)
{
    // Logic thực hiện 
    // Lấy Token của người dùng hiện tại 
    // Clone token của người dùng hiện tại, không sử dụng trực tiếp token đó
    // Tạo  khối môi trường cho token vừa clone được 
    // Tạo tiến trình mới như user mở nó 
    DWORD sessionId = WTSGetActiveConsoleSessionId(); 
    // WTSGetActiveConsoleSessionId(), lấy id hiện tại của người dùng 
    HANDLE hUserToken = NULL;
    if (!WTSQueryUserToken(sessionId, &hUserToken))
    {
        // WTSQueryUserToken(2 tham số sessionId, và lpHanlde để lấy kết quả trả về ) , truy vấn usertoken của 1 session ID 

        WriteLog(L"WTSQueryUserToken failed\n");
        return FALSE;
    }
    // Giúp xác định người dùng hiện tại, lấy userToken của người dùng hiện tại 

    HANDLE hPrimaryToken = NULL;
    if (!DuplicateTokenEx(hUserToken, TOKEN_ALL_ACCESS, NULL, SecurityImpersonation, TokenPrimary, &hPrimaryToken))
    {
        // DuplicateTokenEx, sao chép token vừa lấy được phía trên, không tương tác trực tiếp với token vừa lấy được
        // Chỉ nên thực hiện trên token sao chép 
        WriteLog(L"DuplicateTokenEx failed\n");
        CloseHandle(hUserToken);
        return FALSE;
    }

    // Lấy environment block
    LPVOID lpEnv = NULL;
    if (!CreateEnvironmentBlock(&lpEnv, hPrimaryToken, FALSE))
    {
        // Tạo khối môi trường cho token
        WriteLog(L"CreateEnvironmentBlock failed\n");
        lpEnv = NULL;
    }

    STARTUPINFO si = { sizeof(si) }; // Cấu trúc startupinfo, chứa các thông tin khởi động cho tiến trình mới
    si.lpDesktop = (LPWSTR)L"winsta0\\default";
    PROCESS_INFORMATION pi = {};

    // Tạo buffer có thể modify cho command line
    size_t pathLen = wcslen(appPath);
    LPWSTR commandLine = new WCHAR[pathLen + 3]; // +3 cho quotes và null terminator
    swprintf_s(commandLine, pathLen + 3, L"\"%s\"", appPath); // Bao bọc bằng quotes

    BOOL result = CreateProcessAsUser( // Tạo tiến trình  mới giống như user đang mở 
        hPrimaryToken,
        appPath,                    // <<<< Application name (có thể NULL)
        commandLine,               // <<<< Command line (phải có thể modify)
        NULL,
        NULL,
        FALSE,
        CREATE_UNICODE_ENVIRONMENT | CREATE_NEW_CONSOLE,
        lpEnv, // truyền vào con trỏ khối môi trường 
        NULL,
        &si, // cấu trúc startupinfo
        &pi
    );

    if (!result)
    {
        DWORD err = GetLastError();
        CString msg;
        msg.Format(L"CreateProcessAsUser failed. Error: %lu\n", err);
        WriteLog(msg);
    }
    else
    {
        WriteLog(L"Process created successfully\n");
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }

    // Cleanup
    delete[] commandLine;
    if (lpEnv)
        DestroyEnvironmentBlock(lpEnv);
    CloseHandle(hPrimaryToken);
    CloseHandle(hUserToken);
    return result;
}


BOOL SetBackground(CStringW path)
{
    RunAppAsCurrentUser(L"C:\\Windows\\System32\\notepad.exe");
    return TRUE;
}
DWORD WINAPI WorkerThread(PVOID pParam)
{

    while ((WaitForSingleObject(g_ServiceStopEvent, 0)) != WAIT_OBJECT_0)
    {
        // Tuong duong while(true)
        
        BOOL connected = ConnectNamedPipe(g_hPipe, NULL); 
        if (connected)
        {
            // neu Ket noi thanh cong thi ::
            wchar_t buffer[512]; 
            DWORD byteRead = 0;
            if (ReadFile(g_hPipe, buffer, sizeof(buffer), &byteRead, NULL))
            {
                // Neu nhu doc duoc 
                buffer[byteRead / sizeof(wchar_t)] = L'\0';
                CStringW msg = CStringW(buffer);
                int splitPos = msg.Find(L"|");
                CStringW header = msg.Left(splitPos);
                CStringW body = msg.Mid(splitPos + 1);
                CStringW res = L""; 
                if (header == L"SEND_LOG")
                    WriteLog(body) ? res = L"Ghi Log thanh cong" :  res = L"Ghi log that bai"; 
                else 
                {
                    SetBackground(body) ? res= L"Thay doi hinh nen thanh cong" : res = L"Thay doi hinh nen that bai"; 
                   
                }
                DWORD byteWri = 0;
                WriteFile(g_hPipe, res.GetBuffer(), res.GetLength() * sizeof(wchar_t), &byteWri, NULL );
            }
            DisconnectNamedPipe(g_hPipe); 
        }
    }
    return 0; 
}


void WINAPI ServiceMain(DWORD argc, LPWSTR* argv) // Đọc argv để lấy link log 

{
    int nParam = 0; 
    LPWSTR* lParam = CommandLineToArgvW(GetCommandLineW(), &nParam);
    if (lParam && nParam > 1)
    {
        g_PathLog = CStringW(lParam[1]);
    }
    LocalFree(lParam);

    g_StatusHandle = RegisterServiceCtrlHandler(SERVICE_NAME, ServiceHandle);

    g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS; 
    g_ServiceStatus.dwControlsAccepted = 0; 
    g_ServiceStatus.dwCurrentState = SERVICE_START_PENDING; 

    SetServiceStatus(g_StatusHandle, &g_ServiceStatus);
    
    g_ServiceStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL); 

    g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP; 
    g_ServiceStatus.dwCurrentState = SERVICE_RUNNING; 
    
    SetServiceStatus(g_StatusHandle, &g_ServiceStatus);
    
    // ===== Khoi tao NamedPipe

    SECURITY_ATTRIBUTES sa = { 0 };
    PSECURITY_DESCRIPTOR pSD = NULL;

    // DACL cho phép mọi người truy cập: D:(A;OICI;GRGW;;;WD)
    LPCWSTR szSD = L"D:(A;OICI;GRGW;;;WD)";

    if (!ConvertStringSecurityDescriptorToSecurityDescriptorW(szSD, SDDL_REVISION_1, &pSD, NULL)) {
        return;
    }

    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = pSD;
    sa.bInheritHandle = FALSE;
    g_hPipe = CreateNamedPipeW(
        L"\\\\.\\pipe\\aatest1pipe",
        PIPE_ACCESS_DUPLEX, 
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT ,
        PIPE_UNLIMITED_INSTANCES, 
        4096,
        4096, 
        0 ,
         &sa 
    );
 
    g_WorkerThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WorkerThread, NULL, 0 , nullptr); 

    WaitForSingleObject(g_ServiceStopEvent, INFINITE); 

    if (g_WorkerThread)
    {
        WaitForSingleObject(g_WorkerThread, INFINITE); 
        CloseHandle(g_WorkerThread); 
    }
    CloseHandle(g_ServiceStopEvent); 

    g_ServiceStatus.dwControlsAccepted = 0; 
    g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
    SetServiceStatus(g_StatusHandle, &g_ServiceStatus); 





}
int _tmain(int argc, TCHAR* argv[])
{
   
    SERVICE_TABLE_ENTRY ServiceTable[] =
    {
        {(LPWSTR)SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION)ServiceMain },
        {nullptr, nullptr}
    }; 
    StartServiceCtrlDispatcher(ServiceTable);
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
