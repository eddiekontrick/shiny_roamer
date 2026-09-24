#include "InputController.h"
#include <Arduino.h>

// Pin definitions
const int buttonStartPin = 2;
const int buttonSelectPin = 3;
const int buttonAPin = A0;
const int buttonBPin = 12;
const int buttonLBPin = 4;
const int buttonRBPin = 5;
const int buttonLeftPin = 8;
const int buttonRightPin = 9;
const int buttonUpPin = 10;
const int buttonDownPin = 11;
const int buttonYPin = 7;
const int buttonXPin = 6;

void initButtons() {
    pinMode(buttonAPin, OUTPUT);      digitalWrite(buttonAPin, HIGH);
    pinMode(buttonBPin, OUTPUT);      digitalWrite(buttonBPin, HIGH);
    pinMode(buttonXPin, OUTPUT);      digitalWrite(buttonXPin, HIGH);
    pinMode(buttonYPin, OUTPUT);      digitalWrite(buttonYPin, HIGH);
    pinMode(buttonLBPin, OUTPUT);     digitalWrite(buttonLBPin, HIGH);
    pinMode(buttonRBPin, OUTPUT);     digitalWrite(buttonRBPin, HIGH);
    pinMode(buttonStartPin, OUTPUT);  digitalWrite(buttonStartPin, HIGH);
    pinMode(buttonSelectPin, OUTPUT); digitalWrite(buttonSelectPin, HIGH);
    pinMode(buttonUpPin, OUTPUT);     digitalWrite(buttonUpPin, HIGH);
    pinMode(buttonDownPin, OUTPUT);   digitalWrite(buttonDownPin, HIGH);
    pinMode(buttonLeftPin, OUTPUT);   digitalWrite(buttonLeftPin, HIGH);
    pinMode(buttonRightPin, OUTPUT);  digitalWrite(buttonRightPin, HIGH);
}

void pressA(unsigned int duration)      { digitalWrite(buttonAPin, LOW); delay(duration); digitalWrite(buttonAPin, HIGH); }
void pressB(unsigned int duration)      { digitalWrite(buttonBPin, LOW); delay(duration); digitalWrite(buttonBPin, HIGH); }
void pressX(unsigned int duration)      { digitalWrite(buttonXPin, LOW); delay(duration); digitalWrite(buttonXPin, HIGH); }
void pressY(unsigned int duration)      { digitalWrite(buttonYPin, LOW); delay(duration); digitalWrite(buttonYPin, HIGH); }
void pressLB(unsigned int duration)     { digitalWrite(buttonLBPin, LOW); delay(duration); digitalWrite(buttonLBPin, HIGH); }
void pressRB(unsigned int duration)     { digitalWrite(buttonRBPin, LOW); delay(duration); digitalWrite(buttonRBPin, HIGH); }
void pressStart(unsigned int duration)  { digitalWrite(buttonStartPin, LOW); delay(duration); digitalWrite(buttonStartPin, HIGH); }
void pressSelect(unsigned int duration) { digitalWrite(buttonSelectPin, LOW); delay(duration); digitalWrite(buttonSelectPin, HIGH); }
void pressUp(unsigned int duration)     { digitalWrite(buttonUpPin, LOW); delay(duration); digitalWrite(buttonUpPin, HIGH); }
void pressDown(unsigned int duration)   { digitalWrite(buttonDownPin, LOW); delay(duration); digitalWrite(buttonDownPin, HIGH); }
void pressLeft(unsigned int duration)   { digitalWrite(buttonLeftPin, LOW); delay(duration); digitalWrite(buttonLeftPin, HIGH); }
void pressRight(unsigned int duration)  { digitalWrite(buttonRightPin, LOW); delay(duration); digitalWrite(buttonRightPin, HIGH); }

void pressReset(unsigned int duration) {
    digitalWrite(buttonAPin, LOW);
    digitalWrite(buttonBPin, LOW);
    digitalWrite(buttonStartPin, LOW);
    digitalWrite(buttonSelectPin, LOW);
    delay(duration);
    digitalWrite(buttonAPin, HIGH);
    digitalWrite(buttonBPin, HIGH);
    digitalWrite(buttonStartPin, HIGH);
    digitalWrite(buttonSelectPin, HIGH);
}

void pressResetDS(unsigned int duration) {
    digitalWrite(buttonRBPin, LOW);
    digitalWrite(buttonLBPin, LOW);
    digitalWrite(buttonStartPin, LOW);
    digitalWrite(buttonSelectPin, LOW);
    delay(duration);
    digitalWrite(buttonRBPin, HIGH);
    digitalWrite(buttonLBPin, HIGH);
    digitalWrite(buttonStartPin, HIGH);
    digitalWrite(buttonSelectPin, HIGH);
}


// HOLD DOWN BUTTON FUNCTIONS
// BUTTONS

void holdA()      { digitalWrite(buttonAPin, LOW); }
void releaseA()   { digitalWrite(buttonAPin, HIGH); }
void holdB()      { digitalWrite(buttonBPin, LOW); }
void releaseB()   { digitalWrite(buttonBPin, HIGH); }
void holdX()      { digitalWrite(buttonXPin, LOW); }
void releaseX()   { digitalWrite(buttonXPin, HIGH); }
void holdY()      { digitalWrite(buttonYPin, LOW); }
void releaseY()   { digitalWrite(buttonYPin, HIGH); }

// DIRECTIONAL
void holdUp()      { digitalWrite(buttonUpPin, LOW); }
void releaseUp()   { digitalWrite(buttonUpPin, HIGH); }
void holdDown()      { digitalWrite(buttonDownPin, LOW); }
void releaseDown()   { digitalWrite(buttonDownPin, HIGH); }
void holdLeft()      { digitalWrite(buttonLeftPin, LOW); }
void releaseLeft()   { digitalWrite(buttonLeftPin, HIGH); }
void holdRight()      { digitalWrite(buttonRightPin, LOW); }
void releaseRight()   { digitalWrite(buttonRightPin, HIGH); }

// 
