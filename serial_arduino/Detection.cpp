#include "Detection.h"

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
