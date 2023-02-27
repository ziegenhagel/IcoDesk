#include <header.h>

#include "ApiUtils.h"
#include "RandomUtils.h"
#include "TextUtils.h"

#include "WiFiManager.h"

/* -------------------------------------------------------------------------- */
/*                               Initializations                              */
/* -------------------------------------------------------------------------- */

// Helper function
#define sign(x) ({ __typeof__(x) _x = (x); _x < 0 ? -1 : _x ? 1 : 0; })

// Display
#define TFT_CS 5
#define TFT_RST 4 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC 2

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

float p = 3.1415926;

// Encoder
#define ROTARY_ENCODER_A_PIN 32
#define ROTARY_ENCODER_B_PIN 16
#define ROTARY_ENCODER_BUTTON_PIN 25
#define ROTARY_ENCODER_VCC_PIN 12 /* 27 put -1 of Rotary encoder Vcc is connected directly to 3,3V; else you can use declared output pin for powering rotary encoder */

#define ROTARY_ENCODER_STEPS 4 // depending on your encoder - try 1,2 or 4 to get expected behaviour

AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS);

/* -------------------------------------------------------------------------- */
/*                      Modules - Initialize modules here                     */
/* -------------------------------------------------------------------------- */

StaticJsonDocument<1024> config;
unsigned int colors[4];

uint8_t _currentModule = 0;
uint8_t numberOfModules = 0;
IcoMod* modules[16];

/* -------------------------------------------------------------------------- */
/*                               ROTARY ENCODER                               */
/* -------------------------------------------------------------------------- */

void rotary_onButtonClick()
{
  static unsigned long lastTimePressed = 0;
  // ignore multiple press in that time milliseconds
  if (millis() - lastTimePressed < 500)
  {
    return;
  }
  lastTimePressed = millis();
  Serial.print("button pressed ");
  Serial.print(millis());
  Serial.println(" milliseconds after restart");

  modules[_currentModule]->onClick();
}

void checkRotation()
{
  static int _lastValue = 0;
  static int _currentValue = 0;
  static int _startValue = 0;
  static bool _isRotating = false;
  static unsigned long _startedRotating = 0;
  static int _steps = 0;

  _currentValue = rotaryEncoder.readEncoder();

  // Start rotating
  if (_lastValue != _currentValue && !_isRotating)
  {
    _isRotating = true;
    _startedRotating = millis();
    _startValue = _lastValue;
    // Serial.print("Value: ");
    // Serial.println(_currentValue);
  }

  // Check steps when rotating
  if (_isRotating)
  {
    _steps = _startValue - _currentValue;
    if (abs(_steps) >= 3)
    {
      int dir = sign(_steps);

      _currentModule = (_currentModule + dir + numberOfModules) % numberOfModules;
      modules[_currentModule]->initialize();

      Serial.print("Module");
      Serial.println(_currentModule);

      _startValue = _currentValue;
    }

    if (millis() - _startedRotating > 500)
    {
      _isRotating = false;
    }
  }

  _lastValue = _currentValue;
}

void rotary_loop()
{
  checkRotation();
  
  if (rotaryEncoder.isEncoderButtonClicked())
  {
    rotary_onButtonClick();
  }
}

void IRAM_ATTR readEncoderISR()
{
  rotaryEncoder.readEncoder_ISR();
}

/* -------------------------------------------------------------------------- */
/*                                 Setup WiFi                                 */
/* -------------------------------------------------------------------------- */

void drawInitializationScreen()
{
  tft.fillScreen(ST77XX_BLACK);

  // const char* text = "Welcome to your new IcoDesk!\nTo configure the device, please connect to the network \"IcoDeskWiFi\" and follow the instructions.";
  String text = "Welcome to your new IcoDesk! To configure the device, please connect to the network \"IcoDeskWiFi\" and follow the instructions.";
  TextUtils::printLinesCentered(&tft, text, 10, 8, 10, 1, ST77XX_WHITE);
}

/* -------------------------------------------------------------------------- */
/*                                CONFIGURATION                               */
/* -------------------------------------------------------------------------- */

void getConfig()
{
  const char* configUrl = "http://icode.sk/api/config/69";
  
  ApiUtils::getJsonFromServer(&config, configUrl);

  JsonObject colorsObject = config["colors"];

  colors[0] = RandomUtils::convertHexStringTo16BitColor(colorsObject["background"]);
  colors[1] = RandomUtils::convertHexStringTo16BitColor(colorsObject["foreground"]);
  colors[2] = RandomUtils::convertHexStringTo16BitColor(colorsObject["accent"]);
  colors[3] = RandomUtils::convertHexStringTo16BitColor(colorsObject["danger"]);

  const char* version = config["version"]; // "1.0.0"
  Serial.print("IcoDesk Version ");
  Serial.println(version);
}

IcoMod* createModuleInstance(JsonObject &moduleConfig)
{
  String moduleName = moduleConfig["name"];

  Serial.print("Create instance of ");
  Serial.print(moduleName);
  Serial.println(" module");

  JsonObject params = moduleConfig["config"];

  if (moduleName == "Logo")
  {
    return new IcoMod_Logo(&tft, colors, params);
  }
  if (moduleName == "Weather")
  {
    return new IcoMod_Weather(&tft, colors, params);
  }
  if (moduleName == "DateTime")
  {
    return new IcoMod_DateTime(&tft, colors, params);
  }

  return nullptr;
}

void initializeModules()
{
  if (config.isNull())
  {
    Serial.println("Error: No config found");
    return;
  }

  JsonArray modulesArray = config["modules"].as<JsonArray>();

  numberOfModules = modulesArray.size() + 1;

  Serial.print("Number of modules: ");
  Serial.println(numberOfModules);

  // Add (mandatory) config module
  JsonObject obj = modulesArray[0];
  modules[0] = new IcoMod_Config(&tft, colors, obj);

  for (int i = 0; i < numberOfModules; i++)
  {
    JsonObject module = modulesArray[i];
    modules[i + 1] = createModuleInstance(module);
  }
}

/* -------------------------------------------------------------------------- */
/*                               SETUP and LOOP                               */
/* -------------------------------------------------------------------------- */

void setup()
{
  Serial.begin(9600);
  Serial.println("IcoDesk is starting...");

  // Display
  tft.initR(INITR_BLACKTAB); // Init ST7735S chip, black tab (128x160 px)
  tft.setRotation(2);

  // Encoder
  rotaryEncoder.begin();
  rotaryEncoder.setup(readEncoderISR);

  bool circleValues = true;
  rotaryEncoder.setBoundaries(-1000, 1000, circleValues); // minValue, maxValue, circleValues true|false (when max go to min and vice versa)
  rotaryEncoder.disableAcceleration();

  // Draw instructions to connect to WiFi
  drawInitializationScreen();

  // WiFiManager
  // Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  // Uncomment and run it once, if you want to erase all the stored information
  // wifiManager.resetSettings();

  // wifiManager.setDebugOutput(false);

  wifiManager.setAPStaticIPConfig(IPAddress(8,8,8,8), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

  const char* headText = "<h1>IcoDesk</h1><p>Congratulations! You are the owner of a brand new <b>IcoDesk</b>!</p><p>In order to connect your <b>IcoDesk</b> to the Internet, please enter the password of your Wi-Fi network.</p>";
  wifiManager.setCustomHeadElement(headText);
  wifiManager.setTitle("");

  std::vector<const char *> menu  = { "wifi" };
  wifiManager.setShowInfoUpdate(false);
  wifiManager.setShowInfoErase(false);
  wifiManager.setMenu(menu);
  
  wifiManager.autoConnect("IcoDeskWiFi");
  Serial.println("Connected.");

  // Connect to WiFi
  // initWiFi();

  getConfig();
  initializeModules();

  // Initialize first module
  Serial.println("Initialize first module...");
  modules[_currentModule]->initialize();

  Serial.println("Done.");
}

void loop()
{
  rotary_loop();

  // Update current module
  modules[_currentModule]->refresh();
}