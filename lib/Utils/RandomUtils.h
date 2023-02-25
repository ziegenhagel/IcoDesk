/*
  RandomUtils.h - A few utility functions to do different things.
  Created by Till Michels, February 24, 2023.
*/

#ifndef RandomUtils_h
#define RandomUtils_h

#include "Arduino.h"

class RandomUtils
{
  public:
    static unsigned int convertHexStringTo16BitColor(const char *hexStr);
  private:
};

#endif