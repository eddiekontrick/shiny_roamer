#pragma once

#include <Windows.h>
#include <string>
#include <iostream>

inline constexpr char kPortName[] = "COM3";

namespace SerialHandler{

bool ConnectSerial(HANDLE& serialHandle);

bool ReadSerialLine(HANDLE serialHandle, std::string& line);

bool WriteSerialChar(HANDLE serialHandle, char ch);

bool MatchCommand(const std::string& expected, bool serialConnected, HANDLE serialHandle);

std::string ReadCleanLine(bool serialConnected, HANDLE serialHandle);

}