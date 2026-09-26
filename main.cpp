#include <iostream>
#include <opencv2/opencv.hpp>
#include <../WindowHandler/WindowHandler.h>
#include <../SerialHandler/SerialHandler.h>
#include <../Detection/Detection.h>

int main(){
     // HANDLE WINDOW FUNCTIONALITY
    HWND hwnd = nullptr;
    EnumWindows(WindowHandler::EnumWindowsProc, reinterpret_cast<LPARAM>(&hwnd));

    if (!hwnd){ 
        std::cerr << "Window " << WindowHandler::kWindowName << " could not be found." << std::endl;
        return 1;
    }

    cv::Mat img;
    if (!Detection::Initialize()) {
        return 1;
    }

    // HANDLE SERIAL
    HANDLE serialHandle = nullptr;
    // store serial connection status through ConnectSerial
    const bool serialConnected = SerialHandler::ConnectSerial(serialHandle);

    if (serialConnected){
        PurgeComm(serialHandle, PURGE_RXCLEAR | PURGE_TXCLEAR);
    }

    bool interrupt = false;

    while (true){
        WindowHandler::GetWindow(hwnd, img);
        /*
        int dist = Detection::getDistanceFromEcruteak(img);
        std::cout << "Distance: " << dist << std::endl;
        */
        /*
        Detection::ROAMER currRoamer = Detection::identifyRoamer(img);
        bool isShiny = Detection::DetectShinyRoamer(currRoamer, img);

        std::string msg = isShiny ? "Shiny!" : "Not shiny.";

        std::cout << msg << std::endl;
        if (cv::waitKey(1) == 'q') {  // 27 = Esc key, gives you a clean exit
            break;
        }*/
        
        // GET THE COMMAND FIRST
        std::string command;
        if (serialConnected){
            if (!SerialHandler::ReadSerialLine(serialHandle, command)){
                if (cv::waitKey(1) == 'q') break;
                continue;
            }

            std::string clean = command;
            // trim leading and trailing whitespace
            clean.erase(clean.begin(), std::find_if(clean.begin(), clean.end(), [&](unsigned char ch) { return !std::isspace(ch); }));
            clean.erase(std::find_if(clean.rbegin(), clean.rend(), [&](unsigned char ch) { return !std::isspace(ch); }).base(), clean.end());

            std::cout << "Received: " << clean << std::endl;
            if (clean == "INTERRUPT") {
                interrupt = true;
            }
        }

        // SCAN FOR AN INTERRUPT
        if (!interrupt) continue;

        switch(Detection::DetectInterrupt(img)) {
            case Detection::EVENTS::REPEL:
                // Send repel message to arduino
                if (serialConnected){
                    const std::string response = "REPEL\n";
                    DWORD written = 0;
                    WriteFile(serialHandle, response.c_str(), static_cast<DWORD>(response.size()), &written, nullptr);
                    std::cout << "Sent reapply repel message, waiting on response." << std::endl;

                    // Similar function to while serial.available()
                    // this waits for the response from arduino saying GET_DIST
                    // which will trigger the getDistanceFromEcruteak function
                    std::string command;
                    while (true){
                        if (!SerialHandler::ReadSerialLine(serialHandle, command)){
                            if (cv::waitKey(1) == 'q') break;
                            continue;
                        }
                        std::string clean = command;
                        // trim leading and trailing whitespace
                        clean.erase(clean.begin(), std::find_if(clean.begin(), clean.end(), [&](unsigned char ch) { return !std::isspace(ch); }));
                        clean.erase(std::find_if(clean.rbegin(), clean.rend(), [&](unsigned char ch) { return !std::isspace(ch); }).base(), clean.end());

                        std::cout << "Received: " << clean << std::endl;
                        if (clean == "GET_DIST") {
                            std::string directions = Detection::getReturnToEcruteakMessage(img);
                            DWORD written = 0;
                            WriteFile(serialHandle, directions.c_str(), static_cast<DWORD>(directions.size()), &written, nullptr);
                            std::cout << "Message sent: " << directions << std::endl;
                        }
                    }
                }
                break;
            case Detection::EVENTS::ENCOUNTER:
                std::cout << "Encounter Starting! " << std::endl;
                break;
            case Detection::EVENTS::NO_ACTION:
                break;
        }
        

        // get the tile distance from resetting route        
        
        if (serialConnected){
            std::string directions = Detection::getReturnToEcruteakMessage(img);
            DWORD written = 0;
            WriteFile(serialHandle, directions.c_str(), static_cast<DWORD>(directions.size()), &written, nullptr);
            std::cout << "Message sent: " << directions << std::endl;
        } else {
            std::cout << "Serial not connected";
        }
        

        if (cv::waitKey(1) == 'q') {  // 27 = Esc key, gives you a clean exit
            break;
        }
        
    }

}
