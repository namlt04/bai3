// ConsoleApplication2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include <Windows.h>
#include <tchar.h>

int main()
{
    // Đường dẫn đầy đủ tới ảnh nền (nên dùng ảnh .bmp để tương thích tốt)
    LPCTSTR imagePath = _T("C:\\Users\\admin\\Downloads\\pexels-suissounet-2101187.jpg");

    // Gọi hàm thay đổi ảnh nền
    BOOL result = SystemParametersInfo(
        SPI_SETDESKWALLPAPER,        // Thay đổi wallpaper
        0,                           // Không dùng tham số phụ
        (PVOID)imagePath,            // Đường dẫn tới ảnh
        SPIF_UPDATEINIFILE | SPIF_SENDWININICHANGE // Cập nhật registry và broadcast
    );

    if (result) {
        MessageBox(NULL, _T("Đã thay đổi ảnh nền thành công!"), _T("Thông báo"), MB_OK);
    }
    else {
        MessageBox(NULL, _T("Không thể thay đổi ảnh nền!"), _T("Lỗi"), MB_OK | MB_ICONERROR);
    }

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
