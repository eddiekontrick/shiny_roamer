#include "Detection.h"

#include <filesystem>
#include <stdexcept>

namespace Detection{

namespace {
cv::Mat reference;

std::filesystem::path GetExecutableDirectory() {
    std::wstring buffer(MAX_PATH, L'\0');
    DWORD length = 0;

    do {
        buffer.resize(buffer.size() * 2);
        length = GetModuleFileNameW(nullptr, buffer.data(),
                                    static_cast<DWORD>(buffer.size()));
    } while (length == buffer.size());

    if (length == 0) {
        return {};
    }

    buffer.resize(length);
    return std::filesystem::path(buffer).parent_path();
}
}

bool Initialize() {
    const auto referencePath = GetExecutableDirectory() / "tree_reference.png";
    reference = cv::imread(referencePath.string(), cv::IMREAD_GRAYSCALE);
    if (reference.empty()) {
        std::cerr << "Could not load tree reference image: "
                  << referencePath.string() << std::endl;
        return false;
    }

    return true;
}

bool DetectShiny(cv::Mat img, cv::Rect roiRect, cv::Vec3i normalColor, int tolerance){
    cv::Mat roi = img(roiRect);
    cv::Scalar meanColor = cv::mean(roi);

    cv::Vec3i roiAvg(
        static_cast<int>(meanColor[0]),
        static_cast<int>(meanColor[1]),
        static_cast<int>(meanColor[2])
    );

    cv::Vec3i diff(
        std::abs(roiAvg[0] - normalColor[0]),
        std::abs(roiAvg[1] - normalColor[1]),
        std::abs(roiAvg[2] - normalColor[2])
    );

    std::cout << "Color detected: " << roiAvg << "\nDifference from shiny: " << diff << std::endl;

    cv::rectangle(img, roiRect, cv::Scalar(0,255,0), 1);
    cv::imshow("Detection window", img);
    
    return diff[0] > tolerance || diff[1] > tolerance || diff[2] > tolerance;
}

Detection::EVENTS DetectInterrupt(cv::Mat img){
    cv::Rect encounterDimensions(120, 360, 10, 10);
    cv::Rect repelDimensions(244, 162, 5, 5);

    cv::Vec3i encounterColor(255, 255, 255);
    cv::Vec3i repelColor(251, 251, 203);

    cv::Mat repelRoi = img(repelDimensions);
    cv::Mat encounterRoi = img(encounterDimensions);

    // Get average color in each roi
    cv::Scalar repelMean = cv::mean(repelRoi);
    cv::Scalar encounterMean = cv::mean(encounterRoi);

    // Convert to Vector
    cv::Vec3i repelAvg(
        static_cast<int>(repelMean[0]),
        static_cast<int>(repelMean[1]),
        static_cast<int>(repelMean[2])
    );

    cv::Vec3i encounterAvg(
        static_cast<int>(encounterMean[0]),
        static_cast<int>(encounterMean[1]),
        static_cast<int>(encounterMean[2])
    );

    // calculate differences from flag color
    cv::Vec3i repelDiff(
        std::abs(repelAvg[0] - repelColor[0]),
        std::abs(repelAvg[1] - repelColor[1]),
        std::abs(repelAvg[2] - repelColor[2])
    );

    cv::Vec3i encounterDiff(
        std::abs(encounterAvg[0] - encounterColor[0]),
        std::abs(encounterAvg[1] - encounterColor[1]),
        std::abs(encounterAvg[2] - encounterColor[2])
    );

    cv::rectangle(img, encounterDimensions, (0, 0, 255), 2);
    cv::rectangle(img, repelDimensions, (0, 0, 255), 2);

    cv::imshow("Detect Interrupt", img);

    if (repelDiff[0] == 0 && repelDiff[1] == 0 && repelDiff[2] == 0){
        return Detection::EVENTS::REPEL;
    }

    if (encounterDiff[0] <= 5 && encounterDiff[1] <= 5 && encounterDiff[2] <= 5){
        return Detection::EVENTS::ENCOUNTER;
    }

    return Detection::EVENTS::NO_ACTION;
}

int getDistanceFromEcruteak(cv::Mat img){
    // Partition out the other trees on the right side of the screen:
    // we are using the treeline on the left as a reference to where we
    // are in relation to the gate
    cv::Rect thresholdRoi(0, 0, 150, 200);
    cv::Mat imgPartition = img(thresholdRoi);

    // convert to binary threshold
    cv::Mat gray;
    cv::cvtColor(imgPartition, gray, cv::COLOR_BGR2GRAY);

    cv::Mat thresholded;
    cv::threshold(gray, thresholded, 90, 255, cv::THRESH_BINARY);

    // begin template matching 
    cv::Mat result;
    cv::matchTemplate(thresholded, reference, result, cv::TM_CCOEFF_NORMED);

    // Localize best match coordinates
    double minVal; double maxVal;
    cv::Point minLoc; cv::Point maxLoc;
    cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());

    cv::Point matchLoc = maxLoc;

    cv::Point bottomRight(matchLoc.x + reference.cols, matchLoc.y + reference.rows);
    cv::rectangle(thresholded, matchLoc, bottomRight, cv::Scalar(0, 255, 0), 2);
    cv::imshow("BINARY", thresholded); 

    // formula for telling distance from gate (to reset route)
    int distance = 90 - bottomRight.y + 58;
    auto it = tiles.find(distance);
    std::cout << "Distance: " << distance << std::endl;
    if (it != tiles.end())
        std::cout << "In tile: " << it->second << std::endl;
    else
        std::cout << "value not found " << std::endl;

    return it->second;
}

bool getFacingDirection(cv::Mat img){
    int tolerance = 30;
    cv::Rect roi(128, 89, 1, 1);
    cv::Mat dirRoi = img(roi);
    cv::Scalar roiMean = cv::mean(dirRoi);

    cv::Vec3i roiColor(
        static_cast<int>(roiMean[0]),
        static_cast<int>(roiMean[1]),
        static_cast<int>(roiMean[2])
    );

    cv::Vec3i forwardColor(190, 207, 239);

    cv::Vec3i dirResult(
        cv::abs(roiColor[0] - forwardColor[0]),
        cv::abs(roiColor[1] - forwardColor[1]),
        cv::abs(roiColor[2] - forwardColor[2])
    );

    std::cout << dirResult << std::endl;
    cv::rectangle(img, roi, (0, 0, 0, 0), 4);
    cv::imshow("Get Facing Direction", img);

    return dirResult[0] < tolerance || dirResult[1] < tolerance || dirResult[2] < tolerance;
}

std::string getReturnToEcruteakMessage(cv::Mat img){
    int tile = Detection::getDistanceFromEcruteak(img);
    std::string direction = Detection::getFacingDirection(img) ? "BACKWARD" : "FORWARD";

    return direction + "_" + std::to_string(tile);
}

}