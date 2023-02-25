/*
  RandomUtils.cpp - A few utility functions to do different things.
  Created by Till Michels, February 24, 2023.
*/

#include "Arduino.h"
#include "RandomUtils.h"

unsigned int RandomUtils::convertHexStringTo16BitColor(const char *hexStr)
{
  long color = strtoul(hexStr + 1, 0, 16);
  return (((color & 0xf80000) >> 8) | ((color & 0xfc00) >> 5) | ((color & 0xf8) >> 3));
}