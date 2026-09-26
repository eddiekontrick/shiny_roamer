#include "Detection.h"

#include <filesystem>
#include <stdexcept>

namespace Detection{

namespace {
cv::Mat tree_reference;
cv::Mat roamer_reference;
cv::Mat raikou_normal;
cv::Mat raikou_shiny;
cv::Mat entei_normal;
cv::Mat entei_shiny;

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
    const auto imageDirectory = GetExecutableDirectory();
    const auto loadImage = [&imageDirectory](cv::Mat& image,
                                             const std::string& filename) {
        const auto imagePath = imageDirectory / filename;
        image = cv::imread(imagePath.string(), cv::IMREAD_GRAYSCALE);
        if (image.empty()) {
            std::cerr << "Could not load image: " << imagePath.string()
                      << std::endl;
            return false;
        }
        return true;
    };

    if (!loadImage(tree_reference, "tree_reference.png") ||
        !loadImage(roamer_reference, "roamer_reference.png") ||
        !loadImage(raikou_normal, "raikou_normal.png") ||
        !loadImage(raikou_shiny, "raikou_shiny.png") ||
        !loadImage(entei_normal, "entei_normal.png") ||
        !loadImage(entei_shiny, "entei_shiny.png")) {
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

    std::cout << "Color detected: " << roiAvg << "\nDifference from normal: " << diff << std::endl;

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
        std::cout << "Repel interrupt detected, Repel diffs"
            << repelDiff[0] << repelDiff[1] << repelDiff[2];
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
    int distThreshold = 5;

    cv::Rect thresholdRoi(0, 0, 150, 200);
    cv::Mat imgPartition = img(thresholdRoi);

    // convert to binary threshold
    cv::Mat gray;
    cv::cvtColor(imgPartition, gray, cv::COLOR_BGR2GRAY);

    cv::Mat thresholded;
    cv::threshold(gray, thresholded, 90, 255, cv::THRESH_BINARY);

    // begin template matching 
    cv::Mat result;
    cv::matchTemplate(thresholded, tree_reference, result, cv::TM_CCOEFF_NORMED);

    // Localize best match coordinates
    double minVal; double maxVal;
    cv::Point minLoc; cv::Point maxLoc;
    cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());

    cv::Point matchLoc = maxLoc;

    cv::Point bottomRight(matchLoc.x + tree_reference.cols, matchLoc.y + tree_reference.rows);
    cv::rectangle(thresholded, matchLoc, bottomRight, cv::Scalar(0, 255, 0), 2);
    cv::imshow("BINARY", thresholded); 

    // formula for telling distance from gate (to reset route)
    int distance = 90 - bottomRight.y + 58;
    std::cout << "Distance: " << distance << std::endl;

    int tile = -1;
    if (FindTileWithPadding(distance, 3, tile)) {
        std::cout << "In tile: " << tile << std::endl;
    } else {
        std::cout << "value not found" << std::endl;
    }

    return tile;
}

bool getFacingDirection(cv::Mat img){
    int tolerance = 30;
    cv::Rect roi(126, 85, 1, 1);
    cv::Mat dirRoi = img(roi);
    cv::Scalar roiMean = cv::mean(dirRoi);

    cv::Vec3i roiColor(
        static_cast<int>(roiMean[0]),
        static_cast<int>(roiMean[1]),
        static_cast<int>(roiMean[2])
    );

    cv::Vec3i forwardColor(32, 113, 178);

    cv::Vec3i dirResult(
        cv::abs(roiColor[0] - forwardColor[0]),
        cv::abs(roiColor[1] - forwardColor[1]),
        cv::abs(roiColor[2] - forwardColor[2])
    );
    std::cout << "Current color: " << roiColor << std::endl;
    std::cout << dirResult << std::endl;
    cv::rectangle(img, roi, (255, 255, 255, 0), 1);
    cv::imshow("Get Facing Direction", img);

    return dirResult[0] < tolerance || dirResult[1] < tolerance || dirResult[2] < tolerance;
}

std::string getReturnToEcruteakMessage(cv::Mat img){
    int tile = Detection::getDistanceFromEcruteak(img);
    std::string direction = Detection::getFacingDirection(img) ? "BACKWARD" : "FORWARD";

    return direction + "_" + std::to_string(tile);
}

// make sure to remember to pass image 
ROAMER identifyRoamer(cv::Mat img){
    // Threshold the template
    cv::Mat gray_ref;
    if (roamer_reference.channels() == 1) {
        gray_ref = roamer_reference;
    } else if (roamer_reference.channels() == 3) {
        cv::cvtColor(roamer_reference, gray_ref, cv::COLOR_BGR2GRAY);
    } else if (roamer_reference.channels() == 4) {
        cv::cvtColor(roamer_reference, gray_ref, cv::COLOR_BGRA2GRAY);
    } else {
        std::cerr << "Unsupported roamer reference channel count: "
                  << roamer_reference.channels() << std::endl;
        return ROAMER_ERROR;
    }

    cv::Mat thresh_roamer_ref;
    cv::threshold(gray_ref, thresh_roamer_ref, 110, 255, cv::THRESH_BINARY);

    // Threshold the passed image
    cv::Mat gray_img;
    if (img.channels() == 1) {
        gray_img = img;
    } else if (img.channels() == 3) {
        cv::cvtColor(img, gray_img, cv::COLOR_BGR2GRAY);
    } else if (img.channels() == 4) {
        cv::cvtColor(img, gray_img, cv::COLOR_BGRA2GRAY);
    } else {
        std::cerr << "Unsupported input image channel count: "
                  << img.channels() << std::endl;
        return ROAMER_ERROR;
    }

    cv::Mat thresh_img;
    cv::threshold(gray_img, thresh_img, 100, 255, cv::THRESH_BINARY);

    cv::Mat result;
    cv::matchTemplate(thresh_img, thresh_roamer_ref, result, cv::TM_CCOEFF_NORMED);

    double minVal; double maxVal;
    cv::Point minLoc; cv::Point maxLoc;
    cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());

    cv::Point matchLoc = maxLoc;
    cv::Point bottomRight(matchLoc.x + roamer_reference.cols, matchLoc.y + roamer_reference.rows);
    
    cv::rectangle(thresh_img, matchLoc, bottomRight, cv::Scalar(0, 255, 0), 2);
    cv::imshow("BINARY", thresh_img); 

    const double matchThreshold = 0.7;

    std::string message = maxVal > matchThreshold ? "Raikou Detected." : "Entei Detected";
    std::cout << message << std::endl;
    return maxVal > matchThreshold ? RAIKOU : ENTEI;
}

bool DetectShinyRoamer(ROAMER roamer, cv::Mat img){
    switch(roamer){
        case(RAIKOU): {
            cv::Rect raikouRoi(193, 33, 4, 4);
            cv::Vec3i raikouNormalColor(142, 101, 174);
            return DetectShiny(img, raikouRoi, raikouNormalColor, 5);
        } case(ENTEI): {
            cv::Rect enteiRoi(179, 40, 2, 2);
            cv::Vec3i enteiNormalColor(0, 0, 219);
            return DetectShiny(img, enteiRoi, enteiNormalColor, 5);
        } case(ROAMER_ERROR):
            std::cout << "Roamer error!" << std::endl;
            return false;
        default:
            return false;
    }
}

// Looks up `distance` in `tiles`, tolerating +/- padding pixels of drift.
// Returns true and sets outTile to the matched tile if found.
bool FindTileWithPadding(int distance, int padding, int& outTile) {
    int bestOffset = padding + 1; // sentinel: worse than any valid match
    bool found = false;

    for (int offset = -padding; offset <= padding; ++offset) {
        auto it = tiles.find(distance + offset);
        if (it != tiles.end() && std::abs(offset) < bestOffset) {
            bestOffset = std::abs(offset);
            outTile = it->second;
            found = true;
        }
    }
    return found;
}

}