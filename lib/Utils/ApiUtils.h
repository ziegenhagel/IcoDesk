/*
  ApiUtils.h - Utility functions to fetch JSONs from a server.
  Created by Till Michels, February 23, 2023.
*/

#ifndef ApiUtils_h
#define ApiUtils_h

#include "Arduino.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

class ApiUtils
{
  public:
    static String httpGETRequest(const char *url);
    static void getJsonFromServer(JsonDocument *doc, const char *url);
  private:
};

#endif