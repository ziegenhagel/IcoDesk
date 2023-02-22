/*
  TextUtils.cpp - Utility functions to print text on a TFT display.
  Created by Till Michels, February 21, 2023.
*/

#include "Arduino.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include "TextUtils.h"

// TextUtils::TextUtils(Adafruit_ST7735 *tft)
// {
//   tft = tft;
//   tft->setTextWrap(false);
// }

bool TextUtils::fitsOnScreen(Adafruit_ST7735 *tft, String str, uint8_t x, uint8_t textSize)
{
  tft->setTextSize(textSize);

  int16_t x1, y1;

  uint16_t size[2] = {0, 0};
  uint16_t comparisonSize[2] = {0, 0};

  tft->getTextBounds("A", 0, tft->height() / 2, &x1, &y1, &comparisonSize[0], &comparisonSize[1]);
  tft->getTextBounds(str, x, tft->height() / 2, &x1, &y1, &size[0], &size[1]);

  return size[1] == comparisonSize[1];
}

void TextUtils::printCentered(Adafruit_ST7735 *tft, String str, uint8_t y, uint8_t textSize, uint16_t textColor)
{
  tft->setTextSize(textSize);
  tft->setTextColor(textColor);

  int16_t x1, y1;
  uint16_t size[2] = {0, 0};
  
  tft->getTextBounds(str, 0, 0, &x1, &y1, &size[0], &size[1]); // calc width of new string
  tft->setCursor(tft->width() / 2 - size[0] / 2, y);
  tft->print(str);
}

void TextUtils::printLinesCentered(Adafruit_ST7735 *tft, String str, uint8_t spacingX, uint8_t spacingY, uint8_t y, uint8_t textSize, uint16_t textColor)
{
  tft->setTextSize(textSize);
  tft->setTextColor(textColor);
  
  // Check if whole string fits
  if (fitsOnScreen(tft, str, spacingX * 2, textSize))
  {
    // Serial.print("The String: ");
    // Serial.print(str);
    // Serial.println(" fits on screen.");
    printCentered(tft, str, y, textSize, textColor);
    return;
  }

  int16_t x1, y1;
  uint16_t comparisonSize[2] = {0, 0};
  tft->getTextBounds("A", 0, tft->height() / 2, &x1, &y1, &comparisonSize[0], &comparisonSize[1]);

  String words[30];
  int wordCount = 0;

  // Split the string into substrings
  while (str.length() > 0)
  {
    int index = str.indexOf(' ');
    if (index == -1) // No space found
    {
      words[wordCount++] = str;
      break;
    }
    else
    {
      words[wordCount++] = str.substring(0, index);
      str = str.substring(index + 1);
    }
  }

  String line = "";
  for (int i = 0; i < wordCount - 1; i++)
  {
    line += words[i] + " ";

    // Check if next line would fit
    if (!fitsOnScreen(tft, line + words[i + 1], spacingX * 2, textSize))
    {
      printCentered(tft, line.substring(0, line.length() - 1), y, textSize, textColor);

      String tail = "";
      for (int j = i + 1; j < wordCount; j++)
      {
        tail += words[j] + " ";
      }
      printLinesCentered(tft, tail.substring(0, tail.length() - 1), spacingX, spacingY, y + comparisonSize[1] + spacingY, textSize, textColor);
      return;
    }
  }
}

void TextUtils::printRightAligned(Adafruit_ST7735 *tft, String str, uint8_t spacing, uint8_t y, uint8_t textSize, uint16_t textColor)
{
  tft->setTextSize(textSize);
  tft->setTextColor(textColor);

  int16_t x1, y1;
  uint16_t size[2] = {0, 0};

  tft->getTextBounds(str, 0, tft->height() / 2, &x1, &y1, &size[0], &size[1]);

  int strSpace = size[0] + spacing;
  tft->setCursor(tft->width() - strSpace, y);
  tft->print(str);
}