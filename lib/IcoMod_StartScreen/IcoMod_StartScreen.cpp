/*
  IcoMod_StartScreen.cpp - IcoDesk Module to display the animated Logo.
  Created by Till Michels, February 2, 2023.
*/

#include "Arduino.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include "IcoMod_StartScreen.h"

IcoMod_StartScreen::IcoMod_StartScreen(Adafruit_ST7735 *tft, uint16_t color)
{
  _tft = tft;
  _color = color;
  _animate = false;

  int polygons[10][6] = {{42, 26, 22, 25, 22, 46}, {22, 50, 7, 64, 22, 78}, {22, 81, 7, 95, 22, 109}, {22, 114, 22, 135, 42, 134}, {50, 135, 64, 149, 78, 135}, {106, 114, 106, 135, 86, 134}, {106, 81, 121, 95, 106, 109}, {106, 50, 121, 64, 106, 78}, {86, 26, 106, 25, 106, 46}, {78, 25, 64, 11, 50, 25}};
  memcpy(&_polygons, &polygons, sizeof(&polygons[0]) * 10 * 6);

  _animTime = 300;
}

void IcoMod_StartScreen::onClick()
{
  _animate = !_animate;
}

void IcoMod_StartScreen::initialize()
{
  _tft->fillScreen(ST77XX_BLACK);

  // Middle Part of Logo
  _tft->fillTriangle(44, 109, 44, 50, 36, 118, _color);
  _tft->fillTriangle(36, 41, 36, 118, 44, 50, _color);
  _tft->fillTriangle(36, 118, 36, 41, 27, 109, _color);
  _tft->fillTriangle(27, 50, 27, 109, 36, 41, _color);
  _tft->fillTriangle(46, 30, 37, 38, 81, 30, _color);
  _tft->fillTriangle(91, 38, 81, 30, 37, 38, _color);
  _tft->fillTriangle(82, 47, 91, 38, 46, 47, _color);
  _tft->fillTriangle(37, 38, 46, 47, 91, 38, _color);
  _tft->fillTriangle(92, 119, 101, 110, 92, 42, _color);
  _tft->fillTriangle(101, 51, 92, 42, 101, 110, _color);
  _tft->fillTriangle(83, 110, 92, 119, 83, 51, _color);
  _tft->fillTriangle(92, 42, 83, 51, 92, 119, _color);
  _tft->fillTriangle(37, 121, 46, 130, 91, 121, _color);
  _tft->fillTriangle(82, 130, 91, 121, 46, 130, _color);
  _tft->fillTriangle(46, 112, 37, 121, 81, 112, _color);
  _tft->fillTriangle(91, 121, 81, 112, 37, 121, _color);

  // Rays
  for (int i = 0; i < 10; i++)
  {
    _tft->fillTriangle(_polygons[i][0], _polygons[i][1], _polygons[i][2], _polygons[i][3], _polygons[i][4], _polygons[i][5], _color);
  }
}

void IcoMod_StartScreen::refresh()
{
  static unsigned long animTimer = _animTime;
  static int ray = 0;

  if (!_animate)
  {
    _tft->fillTriangle(_polygons[ray][0], _polygons[ray][1], _polygons[ray][2], _polygons[ray][3], _polygons[ray][4], _polygons[ray][5], _color);
    animTimer = millis();
    return;
  }

  if (millis() >= animTimer)
  {
    _tft->fillTriangle(_polygons[ray][0], _polygons[ray][1], _polygons[ray][2], _polygons[ray][3], _polygons[ray][4], _polygons[ray][5], _color);
    ray = (ray - 1 + 10) % 10;
    _tft->fillTriangle(_polygons[ray][0], _polygons[ray][1], _polygons[ray][2], _polygons[ray][3], _polygons[ray][4], _polygons[ray][5], ST77XX_BLACK);
    animTimer += _animTime;
  }
}