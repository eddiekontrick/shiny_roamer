#include "InputController.h"
#include "GameSequence.h"
#include "Detection.h"

const int ledPin = 13;
bool shinyFound = false;
int counter = 0;


void setLed(bool on) {
  digitalWrite(ledPin, on ? HIGH : LOW);
}

void setup() {
  pinMode(ledPin, OUTPUT);
  setLed(false);

  initButtons();
  Serial.begin(9600);
  Serial.println(F("READY"));

  delay(9000);
  pressA(200);
  delay(1000);
  pressA(200); Serial.println("Open Game");
  delay(200);
}

void loop() {
  int repelCount = 0;
  bool raikouSeen = false;
  bool enteiSeen = false;

  roamer_setup_sequence();

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
          pressResetDS(100);
          break;
        }
        pressB(100);
        delay(600);
        use_repel_hgss();
        returnToEcruteak();
        Serial.println("RESTART_INTERRUPT");
        delay(300);
      }
      else if (message == "ENCOUNTER"){
        // find the correct time to delay so that 
        // no matter how long we have to wait for the last reset 
        // route iteration, we still observe the same pixels
        // placeholder:
        delay(10000);
        Serial.println("START_DETECTION");
        bool decisionReceived = roamer_detection(shinyFound, enteiSeen, raikouSeen);
        counter++;
        Serial.println("Encounters: ");
        Serial.println(counter);
        
        if (shinyFound){
          setLed(true);
          Serial.println("SHINY_FOUND");
          while(true){
            delay(1000);
          }
        }

        if (!decisionReceived) {
          // Detection never resolved
          Serial.println("END_INTERRUPT");
          pressResetDS(100);
          break;
        }

        if (enteiSeen && raikouSeen){
          Serial.println("END_INTERRUPT");
          pressResetDS(100);
          break;
        }

        // implement use attack to knock out current roamer
        knock_out_entei();

        delay(5000);

        // Note, a repel could run out on the way back to ecruteak. I think this should be okay
        // because the program goes right back into interrupt mode anyways, so it'll notice it
        // right away. I'll probably have to put this block above the repel detection one so that it'll notice
        // should I make it just if and not else if to avoid this issue?
        returnToEcruteak();
        Serial.println("RESTART_INTERRUPT");
      }
      
    }
    route_reset_walk();
  }
}
