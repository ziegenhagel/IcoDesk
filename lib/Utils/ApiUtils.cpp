/*
  ApiUtils.cpp - Utility functions to fetch JSONs from a server.
  Created by Till Michels, February 23, 2023.
*/

#include "Arduino.h"
#include "ApiUtils.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

String ApiUtils::httpGETRequest(const char *url)
{
  WiFiClient client;
  HTTPClient http;

  // Your Domain name with URL path or IP address with path
  http.begin(client, url);

  // Send HTTP POST request
  int httpResponseCode = http.GET();

  String payload = "{}";

  if (httpResponseCode > 0)
  {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else
  {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  // Free resources
  http.end();

  return payload;
}

void ApiUtils::getJsonFromServer(JsonDocument *doc, const char *url)
{
  String jsonStr = httpGETRequest(url);

  DeserializationError err = deserializeJson(*doc, jsonStr);

  if (err) 
  {
    Serial.print("ERROR: ");
    Serial.println(err. c_str());
    return;
  }
}