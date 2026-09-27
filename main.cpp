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
        // GET THE COMMAND FIRST
        std::string command = "INTERRUPT";
        if (SerialHandler::MatchCommand(command, serialConnected, serialHandle)){
            interrupt = true;
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
                    while (!SerialHandler::MatchCommand("GET_DIST", serialConnected, serialHandle)){
                        if (cv::waitKey(1) == 'q') break;
                    }

                    // Send return to ecruteak message
                    std::string directions = Detection::getReturnToEcruteakMessage(img);
                    DWORD written2 = 0;
                    WriteFile(serialHandle, directions.c_str(), static_cast<DWORD>(directions.size()), &written2, nullptr);
                    std::cout << "Message sent: " << directions << std::endl;

                    // wait for arduino to signal it's ready for interrupt detection again
                    while (!SerialHandler::MatchCommand("RESTART_INTERRUPT", serialConnected, serialHandle)){
                        if (cv::waitKey(1) == 'q') break;
                    }
                }
                break;
            case Detection::EVENTS::ENCOUNTER:
                std::cout << "Encounter Starting! " << std::endl;

                if (serialConnected){
                    const std::string response = "ENCOUNTER\n";
                    DWORD written = 0;
                    WriteFile(serialHandle, response.c_str(), static_cast<DWORD>(response.size()), &written, nullptr);
                    std::cout << "Sent reapply repel message, waiting on response." << std::endl;

                    // wait for command to start detection
                    std::string detectReady = "START_DETECTION";

                    // First - Get the roamer and store it's name
                    Detection::ROAMER currRoamer = Detection::identifyRoamer(img);
                    std::string currRoamerName = currRoamer == Detection::ROAMER::RAIKOU ? "RAIKOU" : currRoamer == Detection::ROAMER::ENTEI ? "ENTEI" : "ROAMER_ERROR";
                    
                    // Second - Decide if it is shiny
                    bool isShiny = Detection::DetectShinyRoamer(currRoamer, img);
                    std::string shinyStatus = isShiny ? "SHINY\n" : "NORMAL\n";


                    const std::string decision = currRoamerName + "_" + shinyStatus;
                    DWORD written = 0;
                    WriteFile(serialHandle, decision.c_str(), static_cast<DWORD>(response.size()), &written, nullptr);
                    std::cout << "Sent reapply repel message, waiting on response." << std::endl;

                }

                break;
            case Detection::EVENTS::NO_ACTION:
                break;
        }

        if (cv::waitKey(1) == 'q') {  // 27 = Esc key, gives you a clean exit
            break;
        }
        
    }

}


