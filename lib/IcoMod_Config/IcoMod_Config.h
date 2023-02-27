/*
  IcoMod_Config.h - Mandatory IcoDesk Module to open the online Configurator for the device.
  Created by tillmii, February 27, 2023.
*/

#ifndef IcoMod_Config_h
#define IcoMod_Config_h

#include "Arduino.h"
#include "IcoMod.h"
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

class IcoMod_Config : public IcoMod
{
  public:
    IcoMod_Config(Adafruit_ST7735* tft, unsigned int colors[], JsonObject &config);
    void onClick();
    void initialize();
    void refresh();
  private:
    bool _showQrCode;
    // unsigned char _qrCode[];
};

#endif