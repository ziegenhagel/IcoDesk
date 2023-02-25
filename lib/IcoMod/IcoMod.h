/*
  IcoMod.cpp - IcoDesk Module Parent Class.
  All IcoDesk Modules have to inherit from this class.
  Created by Till Michels, February 3, 2023.
*/

#ifndef IcoMod_h
#define IcoMod_h

#include "Arduino.h"
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

class IcoMod
{
  public:
    virtual void onClick() = 0;
    virtual void initialize() = 0;
    virtual void refresh() = 0;
  protected:
    Adafruit_ST7735* _tft;
    unsigned int* _colors;
};

#endif