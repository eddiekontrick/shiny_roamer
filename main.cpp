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
    const bool serialConnected = SerialHandler::ConnectSerial(serialHandle);

    if (serialConnected){
        PurgeComm(serialHandle, PURGE_RXCLEAR | PURGE_TXCLEAR);
    }

    bool interrupt = false;

    while (true){
        WindowHandler::GetWindow(hwnd, img);

        // GET THE COMMAND FIRST
        if (SerialHandler::MatchCommand("INTERRUPT", serialConnected, serialHandle)){
            interrupt = true;
        }

        // SCAN FOR AN INTERRUPT
        if (!interrupt) continue;

        switch(Detection::DetectInterrupt(img)) {
            case Detection::EVENTS::REPEL: {
                if (serialConnected){
                    const std::string response = "REPEL\n";
                    DWORD written = 0;
                    WriteFile(serialHandle, response.c_str(), static_cast<DWORD>(response.size()), &written, nullptr);
                    std::cout << "Sent repel message, waiting on response." << std::endl;

                    bool resetSignalled = false;
                    while (true){
                        std::string line = SerialHandler::ReadCleanLine(serialConnected, serialHandle);

                        if (line == "GET_DIST") {
                            std::string directions = Detection::getReturnToEcruteakMessage(img);
                            DWORD written2 = 0;
                            WriteFile(serialHandle, directions.c_str(), static_cast<DWORD>(directions.size()), &written2, nullptr);
                            std::cout << "Message sent: " << directions << std::endl;
                            break;
                        }
                        if (line == "END_INTERRUPT"){
                            resetSignalled = true;
                            break;
                        }
                        if (cv::waitKey(1) == 'q') break;
                    }

                    if (resetSignalled) {
                        std::cout << "All repels used, standing by" << std::endl;
                        interrupt = false;
                        continue;
                    }

                    while (!SerialHandler::MatchCommand("RESTART_INTERRUPT", serialConnected, serialHandle)) {
                        if (cv::waitKey(1) == 'q') break;
                    }
                    WindowHandler::GetWindow(hwnd, img);
                    cv::imwrite("last_repel_result.png", img);
                }
                break;
            }
            case Detection::EVENTS::ENCOUNTER: {
                std::cout << "Encounter Starting! " << std::endl;

                if (serialConnected){
                    const std::string response = "ENCOUNTER\n";
                    DWORD written = 0;
                    WriteFile(serialHandle, response.c_str(), static_cast<DWORD>(response.size()), &written, nullptr);
                    std::cout << "Sent encounter message, waiting for start-detection signal." << std::endl;

                    // wait for command to start detection
                    while (!SerialHandler::MatchCommand("START_DETECTION", serialConnected, serialHandle)) {
                        if (cv::waitKey(1) == 'q') break;
                    }

                    // Recapture: img at the top of the outer loop is stale by now
                    WindowHandler::GetWindow(hwnd, img);

                    // First - Get the roamer and store its name
                    Detection::ROAMER currRoamer = Detection::identifyRoamer(img);
                    std::string currRoamerName = currRoamer == Detection::ROAMER::RAIKOU ? "RAIKOU" :
                                                  currRoamer == Detection::ROAMER::ENTEI ? "ENTEI" : "ROAMER_ERROR";

                    // Second - Decide if it is shiny
                    bool isShiny = Detection::DetectShinyRoamer(currRoamer, img);
                    std::string shinyStatus = isShiny ? "SHINY\n" : "NORMAL\n";

                    const std::string decision = currRoamerName + "_" + shinyStatus;
                    DWORD written2 = 0;
                    WriteFile(serialHandle, decision.c_str(), static_cast<DWORD>(decision.size()), &written2, nullptr);
                    std::cout << "Sent shiny decision, waiting on response." << std::endl;

                    bool resetSignalled = false;
                    bool shinyFound = false;
                    while (true){
                        if (!IsWindow(hwnd)) {
                            std::cerr << "Window " << WindowHandler::kWindowName << " lost. Waiting to reacquire..." << std::endl;
                            hwnd = nullptr;

                            while (!hwnd) {
                                EnumWindows(WindowHandler::EnumWindowsProc, reinterpret_cast<LPARAM>(&hwnd));
                                if (!hwnd) {
                                    if (cv::waitKey(500) == 'q') {
                                        if (serialConnected) CloseHandle(serialHandle);
                                        return 0;
                                    }
                                }
                            }

                            std::cout << "Window reacquired." << std::endl;
                        }

                        WindowHandler::GetWindow(hwnd, img);
                        std::string line = SerialHandler::ReadCleanLine(serialConnected, serialHandle);

                        if (line == "GET_DIST") {
                            std::string directions = Detection::getReturnToEcruteakMessage(img);
                            DWORD written3 = 0;
                            WriteFile(serialHandle, directions.c_str(), static_cast<DWORD>(directions.size()), &written3, nullptr);
                            std::cout << "Message sent: " << directions << std::endl;
                            break;
                        }
                        // CHECK FOR REPEL IMMEDIATELY AFTER THIS
                        if (line == "END_INTERRUPT"){
                            resetSignalled = true;
                            break;
                        }
                        if (line == "SHINY_FOUND"){
                            shinyFound = true;
                            break;
                        }
                        if (cv::waitKey(1) == 'q') break;
                    }

                    if (shinyFound) {
                        std::cout << "SHINY FOUND! Stopping." << std::endl;
                        return 0;
                    }

                    if (resetSignalled) {
                        interrupt = false;
                        continue;
                    }

                    while (!SerialHandler::MatchCommand("RESTART_INTERRUPT", serialConnected, serialHandle)) {
                        if (cv::waitKey(1) == 'q') break;
                    }
                }
                break;
            }
            case Detection::EVENTS::NO_ACTION:
                break;
        }

        if (cv::waitKey(1) == 'q') {  // 27 = Esc key, gives you a clean exit
            break;
        }
    }

    if (serialConnected) {
        CloseHandle(serialHandle);
    }

    return 0;
}