#include "InputController.h"
#include "GameSequence.h"
#include "Detection.h"

const int ledPin = 13;
bool shinyFound = false;
int counter = 0;

typedef struct Tiles {
  int tileNum;
  int holdDurForward;
  int holdDurBackward;
};

Tiles tiles[13] = {
  {0, 0},
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

void setLed(bool on) {
  digitalWrite(ledPin, on ? HIGH : LOW);
}

void handleSerialCommand(const String& message) {
  String msg = message;
  msg.trim();
  msg.toUpperCase();

  if (msg == "SHINY") {
    shinyFound = true;
    setLed(true);
    return;
  }

  if (msg == "NOT_SHINY" || msg == "NORMAL") {
    shinyFound = false;
    setLed(false);
    return;
  }
}

void setup() {
  pinMode(ledPin, OUTPUT);
  setLed(false);

  initButtons();
  Serial.begin(9600);
  Serial.println(F("READY"));

  delay(5000);
  pressA(200);
  delay(1000);
  pressA(200);
  delay(200);
}

void loop() {
  delay(6000);
  Serial.println("GET_DISTANCE");

  unsigned long startTime = millis();
  const unsigned long responseTimeout = 500; // adjust based on how long PC processing takes

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
      break; // got and handled the response, stop waiting
    }
  }
}
  /*
  while (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n');
    handleSerialCommand(cmd);
  }

  counter++;

  if (!shinyFound) {
    diamond_title_sequence();
    delay(1500);
    encounter_giratina();
    detection_ready(shinyFound);

    if (!shinyFound) {
      pressResetDS(200);
    }
  }

  Serial.print("Number of encounters: ");
  Serial.println(counter);

  if (shinyFound) {
    while (true) {
      setLed(true);
      delay(250);
      setLed(false);
      delay(250);
    }
  }
  */
// }
