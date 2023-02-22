/*
  TextUtils.h - Utility functions to print text on a TFT display.
  Created by Till Michels, February 21, 2023.
*/

#ifndef TextUtils_h
#define TextUtils_h

#include "Arduino.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

class TextUtils
{
  public:
    // TextUtils();
    static bool fitsOnScreen(Adafruit_ST7735* tft, String str, uint8_t x, uint8_t textSize);
    static void printCentered(Adafruit_ST7735* tft, String str, uint8_t y, uint8_t textSize, uint16_t textColor);
    static void printLinesCentered(Adafruit_ST7735* tft, String str, uint8_t spacingX, uint8_t spacingY, uint8_t y, uint8_t textSize, uint16_t textColor);
    static void printRightAligned(Adafruit_ST7735* tft, String str, uint8_t spacing, uint8_t y, uint8_t textSize, uint16_t textColor);
  private:
};

#endif