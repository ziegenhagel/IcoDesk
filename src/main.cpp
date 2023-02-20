#include <header.h>

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

int _currentModule = 0;
IcoMod* modules[3] =
{
  new IcoMod_Logo(&tft, STARTSCREEN_COLOR),
  new IcoMod_Weather(&tft, WEATHER_CITY, WEATHER_API_KEY, WEATHER_UPDATE_INTERVAL),
  new IcoMod_DateTime(&tft, DATETIME_GMT_OFFSET_SEC, DATETIME_DAYLIGHT_OFFSET_SEC)
};

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

      int numberOfModules = sizeof(modules) / sizeof(IcoMod*);
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
  // dont print anything unless value changed
  //  if (rotaryEncoder.encoderChanged())
  //  {
  //  	Serial.print("Value: ");
  //  	Serial.println(rotaryEncoder.readEncoder());
  //  }
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
/*                                    WiFi                                    */
/* -------------------------------------------------------------------------- */

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, WIFI_KEY);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

/* -------------------------------------------------------------------------- */
/*                               SETUP and LOOP                               */
/* -------------------------------------------------------------------------- */

void setup()
{
  Serial.begin(9600);
  Serial.println("IcoDesk is starting...");

  // Connect to WiFi
  initWiFi();

  // Display
  tft.initR(INITR_BLACKTAB); // Init ST7735S chip, black tab (128x160 px)
  tft.setRotation(2);

  // Encoder
  rotaryEncoder.begin();
  rotaryEncoder.setup(readEncoderISR);

  bool circleValues = true;
  rotaryEncoder.setBoundaries(-1000, 1000, circleValues); // minValue, maxValue, circleValues true|false (when max go to min and vice versa)
  rotaryEncoder.disableAcceleration();

  // Initialize first module
  modules[_currentModule]->initialize();
}

void loop()
{
  rotary_loop();

  // Update current module
  modules[_currentModule]->refresh();
}