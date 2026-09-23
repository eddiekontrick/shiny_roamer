#include "SerialHandler.h"

namespace SerialHandler{

bool ConnectSerial(HANDLE& serialHandle){
    serialHandle = CreateFileA(
        kPortName,
        GENERIC_READ | GENERIC_WRITE,
        0,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if (serialHandle == INVALID_HANDLE_VALUE){
        std::cerr << "Error opening serial port, error code: " << GetLastError() << std::endl;
        return false;
    }

    DCB serialParams = {0};
    serialParams.DCBlength = sizeof(serialParams);

    if(GetCommState(serialHandle, &serialParams) == 0){
        std::cerr << "Error getting comm state, error code: " << GetLastError() << std::endl;
        return false;
    }

    serialParams.BaudRate = CBR_9600;
    serialParams.ByteSize = 8;
    serialParams.StopBits = ONESTOPBIT;
    serialParams.Parity = NOPARITY;

    if (SetCommState(serialHandle, &serialParams) == 0){
        std::cerr << "Error setting comm state, error code: " << GetLastError() << std::endl;
        CloseHandle(serialHandle);
        return false;
    }

    COMMTIMEOUTS timeouts = {};
    timeouts.ReadIntervalTimeout = MAXDWORD;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(serialHandle, &timeouts)){
        std::cerr << "Error setting comm timeouts, error code: " << GetLastError() << std::endl;
        CloseHandle(serialHandle);
        return false;
    }

    std::cout << "Serial port successfully configured.\n" << std::endl;

    // Clear previous buffers
    PurgeComm(serialHandle, PURGE_RXCLEAR | PURGE_TXCLEAR);
    return true;
}

bool ReadSerialLine(HANDLE serialHandle, std::string& line){
    char ch = 0;
    std::string buffer;

    while (true){
        DWORD bytesRead = 0;
        // if read failure, return false
        if (!ReadFile(serialHandle, &ch, 1, &bytesRead, nullptr)){
            return false;
        }
        // if no response in 50ms, return false
        if (bytesRead == 0){
            return false;
        }

        // once terminal char reached
        if (ch == '\r' || ch == '\n'){
            // check if there is anything else in buffer
            // this prevents cases where two terminal chars are sent 
            // back to back
            if (!buffer.empty()){
                // store the contents of the buffer to passed line (not including terminal)
                line = buffer;
                return true;
            }
            // if buffer is empty, don't add this char
            continue;
        }

        // add char to buffer
        buffer += ch;
    }
}

bool WriteSerialChar(HANDLE serialHandle, char ch){
    DWORD bytesWritten = 0;

    if (!WriteFile(serialHandle, &ch, 1, &bytesWritten, nullptr)){
        std::cerr << "Error writing to serial port, error code: " << GetLastError() << std::endl;
        return false;
    }

    if (bytesWritten != 1){
        std::cerr << "Incomplete write to serial port." << std::endl;
        return false;
    }

    return true;
}

}