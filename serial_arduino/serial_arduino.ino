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
  int repelCount = 0;
  bool raikouSeen = false;
  bool enteiSeen = false;

  // roamer_setup_sequence();

  // Enter interrupt detection mode
  Serial.println("INTERRUPT");
  while (true){

    if (Serial.available()){
      String message = Serial.readStringUntil('\n');
      message.trim();
      message.toUpperCase();
      // This won't work because it needs to go back to route_reset_walk instead of rest
      // They must be in the same while loop
      if (message == "REPEL"){
        repelCount++;
        if (repelCount >= 25){
          Serial.println("END_INTERRUPT");
          break;
        }
        pressB(100);
        delay(300);
        use_repel_hgss();
        returnToEcruteak();
        Serial.println("RESTART_INTERRUPT");
      }
      else if (message == "ENCOUNTER"){
        // find the correct time to delay so that 
        // no matter how long we have to wait for the last reset 
        // route iteration, we still observe the same pixels
        // placeholder:
        delay(5000);
        Serial.println("START_DETECTION");
        roamer_detection(shinyFound, enteiSeen, raikouSeen);

        if (!shinyFound && (enteiSeen && raikouSeen)) pressResetDS(100);

        if (!shinyFound && (!enteiSeen || !raikouSeen)){
          // implement use attack to knock out current roamer
          // knock_out_roamer();

          // Note, a repel could run out on the way back to ecruteak. I think this should be okay
          // because the program goes right back into interrupt mode anyways, so it'll notice it 
          // right away. I'll probably have to put this block above the repel detection one so that it'll notice
          // should I make it just if and not else if to avoid this issue?
          returnToEcruteak();
          Serial.println("RESTART_INTERRUPT");
        }
      }
      
    }
    route_reset_walk();
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
