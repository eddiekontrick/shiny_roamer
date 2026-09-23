#ifndef INPUTCONTROLLER_H
#define INPUTCONTROLLER_H

#include <Arduino.h>

// Initialization
void initButtons();

// Main buttons
void pressA(unsigned int duration);
void pressB(unsigned int duration);
void pressX(unsigned int duration);
void pressY(unsigned int duration);
void pressLB(unsigned int duration);
void pressRB(unsigned int duration);
void pressStart(unsigned int duration);
void pressSelect(unsigned int duration);
void pressUp(unsigned int duration);
void pressDown(unsigned int duration);
void pressLeft(unsigned int duration);
void pressRight(unsigned int duration);
void pressReset(unsigned int duration = 400);
void pressResetDS(unsigned int duration);

void holdUp();
void releaseUp();
void holdDown();
void releaseDown();
void holdLeft();
void releaseLeft();
void holdRight();
void releaseRight();
void holdA();
void releaseA();
void holdB();
void releaseB();
void holdX();
void releaseX();
void holdY();
void releaseY();  

#endif
