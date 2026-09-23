#include "WindowHandler.h"

#include <iostream>

namespace WindowHandler{

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam){
    char title[256] = {};
    GetWindowTextA(hwnd, title, sizeof(title));

    if (std::string(title).find(kWindowName) != std::string::npos){
        // change lParam back into HWND pointer
        auto* result = reinterpret_cast<HWND*>(lParam);
        // dereference to store the pointer
        *result = hwnd;
        std::cout << "Found window: " << title << " , returning handle" << std::endl;
        return FALSE;
    }
    return TRUE;
}

bool GetWindow(HWND hwnd, cv::Mat& img){
    // Get dimensions of DS Capture window
    RECT rect{};
    GetWindowRect(hwnd, &rect);

    const int width = rect.right - rect.left;
    const int height = rect.bottom - rect.top;

    // Get device handle for DS Capture window
    HDC hdcWindow = GetWindowDC(hwnd);

    // Create a compatible device context and bitmap to store pixel data from 
    // the actual window's device context, and stitch them together w/ SelectObject
    HDC hdcMem = CreateCompatibleDC(hdcWindow);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcWindow, width, height);
    SelectObject(hdcMem, hBitmap);

    // copy data from hdc window into bitmap through connected DC (hdcMem)
    BitBlt(hdcMem, 0, 0, width, height, hdcWindow, 0, 0, SRCCOPY);

    // Create a BITMAPINFOHEADER to format output from GetDIBits
    BITMAPINFOHEADER bmi = {0};
    bmi.biSize = sizeof(BITMAPINFOHEADER);
    bmi.biWidth = width;
    bmi.biHeight = -height;                 // set flag to for correct draw order
    bmi.biPlanes = 1;
    bmi.biBitCount = 32;
    bmi.biCompression = BI_RGB;

    cv::Mat result(height, width, CV_8UC4);

    // copy data into the cv::Mat instance's data
    if (GetDIBits(hdcMem, hBitmap, 0, height, result.data,
                  reinterpret_cast<BITMAPINFO*>(&bmi), DIB_RGB_COLORS) == 0) {
        std::cout << "Failed to transfer data." << std::endl;
        DeleteObject(hBitmap);
        DeleteDC(hdcMem);
        ReleaseDC(hwnd, hdcWindow);
        return false;
    }

    // release used HDC and HBITMAPS
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(hwnd, hdcWindow);

    // std::cout << "Window gathered successfully!" << std::endl;
    img = result;
    return true;
}

}
