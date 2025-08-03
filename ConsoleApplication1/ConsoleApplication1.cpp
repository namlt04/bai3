// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <tchar.h>
#include <Windows.h>
#include <stdio.h>
#include <time.h>
#include <atlstr.h>
#define SERVICE_NAME _T("myservice")

SERVICE_STATUS g_ServiceStatus = {}; 
SERVICE_STATUS_HANDLE g_StatusHandle = nullptr; 
CStringW g_PathLog = _T("D:\\test.log");
HANDLE g_ServiceStopEvent = INVALID_HANDLE_VALUE;
HANDLE g_WorkerThread = nullptr; 
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
void WriteLog(const char* message)
{
    CString logPath = g_PathLog;
    HANDLE hFile = CreateFileW(
        logPath,
        FILE_APPEND_DATA,           // Ghi nối tiếp cuối file
        FILE_SHARE_READ,            // Cho phép đọc đồng thời
        NULL,
        OPEN_ALWAYS,                // Tạo nếu chưa có
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        OutputDebugString(_T("Cannot open log file.\n"));
        return;
    }

    // Di chuyển con trỏ file tới cuối (chắc chắn nối tiếp)
    SetFilePointer(hFile, 0, NULL, FILE_END);

    // Format thời gian
    time_t now = time(NULL);
    struct tm t;
    localtime_s(&t, &now);

    char buffer[1024] = {};
    snprintf(buffer, sizeof(buffer),
        "[%02d-%02d-%04d %02d:%02d:%02d] %s\r\n",
        t.tm_mday, t.tm_mon + 1, t.tm_year + 1900,
        t.tm_hour, t.tm_min, t.tm_sec, message);

    DWORD bytesWritten = 0;
    WriteFile(hFile, buffer, (DWORD)strlen(buffer), &bytesWritten, NULL);

    CloseHandle(hFile);
}
DWORD WINAPI WorkerThread(PVOID pParam)
{

    while ((WaitForSingleObject(g_ServiceStopEvent, 0)) != WAIT_OBJECT_0)
    {
        WriteLog("Log in: ");
        Sleep(1000); 
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
