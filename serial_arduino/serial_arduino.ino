#include "InputController.h"
#include "GameSequence.h"
#include "Detection.h"

const int ledPin = 13;
bool shinyFound = false;
int counter = 0;


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
  returnToEcruteak();
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
