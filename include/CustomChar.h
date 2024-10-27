#pragma once

#include <Arduino.h>

const byte blockUp[8] PROGMEM = {
  B11111,
  B11111,
  B11111,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};

const byte blockDown[8] PROGMEM = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
  B11111
};

const byte blockCenter[8] PROGMEM = {
  B00000,
  B00000,
  B11111,
  B11111,
  B11111,
  B00000,
  B00000,
  B00000
};

const byte blockAll[8] PROGMEM = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};