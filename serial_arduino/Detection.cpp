#include "Detection.h"
#include "InputController.h"

Tiles tiles[13] = {
  {0, 0, 0},
  {1, 100, 200},
  {2, 200, 300},
  {3, 320, 450},
  {4, 500, 700},
  {5, 700, 830},
  {6, 860, 950},
  {7, 980, 1000},
  {8, 1120, 1230},
  {9, 1210, 1310},
  {10, 1350, 1450},
  {11, 1500, 1600},
  {12, 1650, 1800}
};

void detection_ready(bool &shinyFound) {
  delay(7000);
  Serial.println("START_DETECTION");
  

  unsigned long waitStart = millis();
  bool decisionReceived = false;

  while (!decisionReceived && millis() - waitStart < 15000) {
    if (Serial.available()) {
      String msg = Serial.readStringUntil('\n');
      msg.trim();
      msg.toUpperCase();

      if (msg == "SHINY") {
        shinyFound = true;
        decisionReceived = true;
        Serial.println("ARDUINO: SHINY RECEIVED");
      } else if (msg == "NOT_SHINY" || msg == "NORMAL") {
        shinyFound = false;
        decisionReceived = true;
        Serial.println("ARDUINO: NOT SHINY RECEIVED");
      }
    }
  }
}

void returnToEcruteak(){
  Serial.println("GET_DIST");
  unsigned long startTime = millis();
  const unsigned long responseTimeout = 1500; // adjust based on how long PC processing takes

  while (millis() - startTime < responseTimeout) {
    if (Serial.available() > 0) {
      String message = Serial.readStringUntil('\n');
      message.trim();
      message.toUpperCase();

      int sepIndex = message.indexOf('_');
      if (sepIndex == -1) break;

      String direction = message.substring(0, sepIndex);
      String tileNumString = message.substring(sepIndex + 1);
      int currTile = tileNumString.toInt();

      for (Tiles tile : tiles) {
        if (tile.tileNum == currTile) {
          if (direction == "FORWARD") {
            pressUp(tile.holdDurForward);
          } else if (direction == "BACKWARD") {
            pressUp(tile.holdDurBackward);
          }
        }
      }
      delay(200);
      pressDown(50);
      break; // got and handled the response, stop waiting
    }
  }
}

void roamer_detection(bool &shinyFound, bool& enteiSeen, bool& raikouSeen) {
  delay(7000);  

  unsigned long waitStart = millis();
  bool decisionReceived = false;

  while (!decisionReceived && millis() - waitStart < 15000) {
    if (Serial.available()) {
      String msg = Serial.readStringUntil('\n');
      msg.trim();
      msg.toUpperCase();

      int sepIndex = msg.indexOf('_');
      if (sepIndex == -1) break;

      String roamer = msg.substring(0, sepIndex);
      String shinyStatus = msg.substring(sepIndex + 1);

      if (shinyStatus == "SHINY") {

        shinyFound = true;
        decisionReceived = true;

        if (roamer == "RAIKOU") raikouSeen = true;
        if (roamer == "ENTEI") enteiSeen = true;

        Serial.println("ARDUINO: SHINY RECEIVED");
      } else if (shinyStatus == "NORMAL") {

        shinyFound = false;
        decisionReceived = true;

        if (roamer == "RAIKOU") raikouSeen = true;
        if (roamer == "ENTEI") enteiSeen = true;

        Serial.println("ARDUINO: NOT SHINY RECEIVED");
      }
    }
  }
}
