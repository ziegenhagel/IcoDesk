/*
  IcoMod_StartScreen.cpp - IcoDesk Module to display the animated Logo.
  Created by Till Michels, February 2, 2023.
*/

#ifndef IcoMod_StartScreen_h
#define IcoMod_StartScreen_h

#include "Arduino.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

class IcoMod_StartScreen
{
  public:
    IcoMod_StartScreen(Adafruit_ST7735* tft, uint16_t color);
    void onClick();
    void initialize();
    void refresh();
  private:
    Adafruit_ST7735* _tft;
    int _color;
    bool _animate;
    int _polygons[10][6];
    unsigned long _animTime;
};

#endif