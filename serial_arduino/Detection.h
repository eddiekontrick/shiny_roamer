#ifndef DETECTION_H
#define DETECTION_H

#include <Arduino.h>
#include "GameSequence.h"

typedef struct Tiles {
  int tileNum;
  int holdDurForward;
  int holdDurBackward;
} Tiles;

extern Tiles tiles[13];

void detection_ready(bool &shinyFound);
void returnToEcruteak();
bool roamer_detection(bool &shinyFound, bool& enteiSeen, bool& raikouSeen);

#endif
