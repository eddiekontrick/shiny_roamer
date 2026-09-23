#pragma once
#include <Windows.h>
#include <string>
#include <opencv2/opencv.hpp>

namespace WindowHandler{

const std::string kWindowName = "DS Capture";

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);

bool GetWindow(HWND hwnd, cv::Mat& img);

}