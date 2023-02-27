/*
  IcoMod_Config.cpp - Mandatory IcoDesk Module to open the online Configurator for the device.
  Created by tillmii, February 27, 2023.
*/

#include "Arduino.h"
#include "IcoMod_Config.h"
#include "icons.h"
#include "ArduinoJson.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

IcoMod_Config::IcoMod_Config(Adafruit_ST7735* tft, unsigned int colors[], JsonObject &config)
{
  _tft = tft;
  _colors = colors;

  // _qrCode = config["qrCode"];
  // memcpy(&_polygons, &polygons, sizeof(&polygons[0]) * 10 * 6);

  _showQrCode = false;
}

void IcoMod_Config::onClick()
{
  _showQrCode = !_showQrCode;
}

void IcoMod_Config::initialize()
{
  _tft->fillScreen(_colors[0]);

  // Draw icon
  _tft->drawXBitmap(0, 16, _configIcon, 128, 128, _colors[2]);
}

void IcoMod_Config::refresh()
{
  
}