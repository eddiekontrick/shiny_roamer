#pragma once

#include <opencv2/opencv.hpp>
#include <Windows.h>
#include <unordered_map>
#include <string>
#include <iostream>

namespace Detection {

enum EVENTS {
    NO_ACTION = -1,
    REPEL, 
    ENCOUNTER
};

enum ROAMER {
    ROAMER_ERROR,
    RAIKOU,
    ENTEI
};

inline const std::unordered_map<int, int> tiles = {
    {0, 0},
    {13, 1},
    {26, 2},
    {39, 3},
    {51, 4},
    {63, 5},
    {74, 6},
    {86, 7},
    {97, 8},
    {107, 9},
    {117, 10},
    {127, 11},
    {137, 12}
};

bool Initialize();
int getDistanceFromEcruteak(cv::Mat img);
bool DetectShiny(cv::Mat img, cv::Rect roi, cv::Vec3i normalColor, int tolerance);
Detection::EVENTS DetectInterrupt(cv::Mat img);
bool getFacingDirection(cv::Mat img);
std::string getReturnToEcruteakMessage(cv::Mat img);
ROAMER identifyRoamer(cv::Mat img);
bool DetectShinyRoamer(ROAMER roamer, cv::Mat img);
bool FindTileWithPadding(int distance, int padding, int& outTile);

}