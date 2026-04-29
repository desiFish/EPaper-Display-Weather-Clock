/*
epdWeatherClockV1.ino
Copyright (C) 2024 desiFish

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
//=============== HEADER SECTION ===============

// E-paper weather clock v1 - Main code
// Uses GxEPD2 library for e-paper display control
// Using Huge App partition (3MB NO OTA/1MB SPIFFS)

//=============== CONFIGURATION ===============
// Enable/disable GxEPD2_GFX base class - uses ~1.2k more code
#define ENABLE_GxEPD2_GFX 0

#include <GxEPD2_3C.h> // 3-color e-paper display
#include <Fonts/FreeMonoBold9pt7b.h>
#include <U8g2_for_Adafruit_GFX.h> // Include U8g2 fonts
#include <Wire.h>                  // Used to establish serial communication on the I2C bus
#include <SparkFun_TMP117.h>       // TMP117 temperature sensor library
#include <Adafruit_Sensor.h>       // Adafruit sensor library
#include "Adafruit_BME680.h"       // BME680 environmental sensor library
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "RTClib.h" // RTC library
#include "image.h"  //for sleep icon
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <BH1750.h>  // Light sensor library
#include <TimeLib.h> // for time functions
#include "icons.h"   // for weather icons

#include <Arduino.h>
#include <ESPAsyncWebServer.h> // for web server
#include <AsyncTCP.h>          // for tcp connection
#include <Preferences.h>       // for storing data in flash memory
#include <esp_wifi.h>          // for wifi functions

//=============== GLOBAL OBJECTS =================
Preferences pref;
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// your wifi name and password
String ssid;
String password;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800); // 19800 is offset of India, pool.ntp.org automatically selects nearest pool for you

// save number of boots
RTC_DATA_ATTR int bootCount = 0; // Persistent boot counter stored in RTC memory
const byte ghostProtek = 5;      // ghost protection, 5 means for every 5 boots, 1 boot will be in dark mode

// openWeatherMap Api Key from your profile in account section
String openWeatherMapApiKey = ""; // add your profile key here when running for the first time

// personal custom Api Key from your server
String customApiKey = ""; // add your api key here when running for the first time

// Replace with your lat and lon
String lat = "22.5895515";
String lon = "88.2876455";

RTC_DS3231 rtc; // Initalize rtc

TMP117 sensor;           // Initalize temperature sensor
Adafruit_BME680 bme;     // Initalize environmental sensor
BH1750 lightMeter(0x23); // Initalize light sensor

// Initalize display for 400x300, UC8276
GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> display(GxEPD2_420c_Z21(/*CS=5*/ /* SS*/ D7, /*DC=*/D1, /*RST=*/D2, /*BUSY=*/D3)); // universal declaration for XIAO series
#define BATPIN A0                                                                                                                      // Battery voltage divider pin (1M Ohm with 104 Capacitor)
#define DEBUG_PIN D6                                                                                                                   // Debug mode toggle pin

U8G2_FOR_ADAFRUIT_GFX u8g2Fonts; // u8g2 fonts

//=============== GLOBAL CONSTANTS ===============
#define BATTERY_LEVEL_SAMPLING 4 // BATTERY_LEVEL_SAMPLING: Number of samples to average for battery reading
#define battType 3.6             // battType: Battery nominal voltage (ICR: 4.2V, LFP: 3.6V) (Change accordingly)
#define battChangeThreshold 0.06 // battChangeThreshold: Minimum voltage change to update battery level
#define battHigh 3.3             // battHigh: Healthy battery threshold voltage (Change accordingly)
#define battLow 2.9              // battLow: Low battery warning threshold (Change accordingly)
#define critBattPercent 30       // critBattPercent: Critical battery percentage threshold
// #define SHOW_BATTERY_VOLT false  // SHOW_BATTERY_VOLT: Set to true to display battery voltage, false to display percentage

/**
 * @brief Sleep configuration
 * uS_TO_S_FACTOR: Microseconds to seconds conversion
 * TIME_TO_SLEEP: Sleep duration in seconds (default 15 mins)
 */
#define uS_TO_S_FACTOR 1000000
int TIME_TO_SLEEP = 900; // 15 minutes

//=============== GLOBAL VARIABLES ===============
// State variables
RTC_DATA_ATTR byte nightFlag = 0;            // Night mode state preserved across sleep
RTC_DATA_ATTR float hTemp = 0.0;             // Highest temperature recorded
RTC_DATA_ATTR float lTemp = 60.0;            // Lowest temperature recorded
RTC_DATA_ATTR float battLevel = battType;    // Battery level
RTC_DATA_ATTR bool BATTERY_CRITICAL = false; // Critical battery state
bool DEBUG_MODE = false;                     // Debug mode state
bool RTC_READY = false;                      // RTC hardware state
bool TMP117_READY = false;                   // TMP117 hardware state
bool BME680_READY = false;                   // BME680 hardware state

String jsonBuffer;              // for storing json data from api
String systemAlertMessage = ""; // Hardware/runtime alerts shown in the alert line

char daysOfTheWeek[7][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
char monthName[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

const char fullDaysOfTheWeek[7][10] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
const char fullMonthName[12][10] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

int httpResponseCode; // for storing http response code

const int16_t TOP_STATUS_HEIGHT = 14;
const int16_t TOP_STATUS_PADDING_X = 4;
const int16_t TOP_STATUS_TEXT_Y = 11;
const int16_t TOP_STATUS_ICON_Y = -1;
const int16_t TOP_STATUS_BATTERY_Y = 4;
const int16_t TOP_STATUS_BATTERY_WIDTH = 16;
const int16_t TOP_STATUS_WIFI_WIDTH = 12;
const int16_t TOP_STATUS_WIFI_ERROR_WIDTH = 13;
const int16_t TOP_STATUS_GAP = 6;

const byte TOP_STATUS_WIFI_CONNECTED = 0;
const byte TOP_STATUS_WIFI_OFF = 1;
const byte TOP_STATUS_WIFI_ERROR = 2;

// DEBUG_MODE update frequency
unsigned long lastTime1 = 0;    // Last light sensor update
const long timerDelay1 = 60000; // Light sensor update interval (60 seconds)

// Define base URLs as const char arrays
const char OPEN_WEATHER_BASE_URL[] = "http://api.openweathermap.org/data/3.0/onecall?lat=";
const char OPEN_WEATHER_PARAMS[] = "&exclude=hourly,minutely&units=metric&appid=";
const char CUSTOM_WEATHER_BASE_URL[] = "http://iotthings.pythonanywhere.com/api/weatherStation/serve?api_key=";

//=============== HTML CODE =================
const char index_html[] PROGMEM = R"rawliteral(
 <!DOCTYPE html><html><head><title>WiFi Setup</title><meta name="viewport" content="width=device-width,initial-scale=1"><style>body{font-family:Arial;text-align:center;margin:20px}input{margin:10px;padding:5px}form{background:#f0f0f0;padding:20px;display:inline-block}</style></head><body><h1>Weather Station Setup</h1><form action="/" method="POST"><label for="ssid">SSID</label><br><input type="text" id="ssid" name="ssid"><br><label for="pass">Password</label><br><input type="text" id="pass" name="pass"><br><input type="submit" value="Connect"></form></body></html>
 )rawliteral";

// Search for parameter in HTTP POST request
const char *PARAM_INPUT_1 = "ssid";
const char *PARAM_INPUT_2 = "pass";

//=============== HELPER FUNCTIONS ===============

/**
 * @brief Measures battery voltage with averaging
 * @return float Averaged battery voltage in volts
 * @note Uses voltage divider with 1MΩ resistor and 104 capacitor
 */
float batteryLevel()
{
  uint32_t Vbatt = 0;
  for (int i = 0; i < BATTERY_LEVEL_SAMPLING; i++)
  {
    Vbatt = Vbatt + analogReadMilliVolts(BATPIN); // ADC with correction
    delay(10);
  }
  float Vbattf = 2 * Vbatt / BATTERY_LEVEL_SAMPLING / 1000.0; // attenuation ratio 1/2, mV --> V
  // if (DEBUG_MODE) Serial.println(Vbattf);
  return (Vbattf);
}

/**
 * @brief Adds one hardware/runtime alert to the shared top alert line
 */
void addSystemAlert(const char *msg)
{
  if (systemAlertMessage.indexOf(msg) >= 0)
    return;

  if (systemAlertMessage.length() > 0)
    systemAlertMessage += " | ";
  systemAlertMessage += msg;
}

/**
 * @brief Builds the current alert line, including dynamic battery state
 */
String currentAlertText()
{
  String text = systemAlertMessage;
  if (BATTERY_CRITICAL)
  {
    if (text.length() > 0)
      text += " | ";
    text += "BATTERY CRITICAL";
  }
  return text;
}

/**
 * @brief Prints an alert in the same compact "Alerts:" style used for weather alerts
 * @param msg Alert text to print
 * @param invert Clears the alert strip with the current screen background color
 */
void printAlertLine(const String &msg, bool invert)
{
  uint16_t bg = invert ? GxEPD_BLACK : GxEPD_WHITE;
  display.fillRect(0, TOP_STATUS_HEIGHT, display.width(), 16, bg);

  if (msg.length() == 0)
    return;

  String text = "Alerts: " + msg;
  u8g2Fonts.setFont(u8g2_font_luRS08_tf);
  while (u8g2Fonts.getUTF8Width(text.c_str()) > display.width() && text.length() > 4)
    text = text.substring(0, text.length() - 4) + "...";

  uint16_t textWidth = u8g2Fonts.getUTF8Width(text.c_str());
  int16_t x = 0;
  if (textWidth < display.width())
    x = (display.width() - textWidth) / 2;

  u8g2Fonts.setCursor(x, TOP_STATUS_HEIGHT + 11);
  u8g2Fonts.print(text);
}

void drawTopWifiIcon(byte wifiState, int16_t x, int16_t y, bool invert)
{
  uint16_t fg = invert ? GxEPD_WHITE : GxEPD_BLACK;
  if (wifiState == TOP_STATUS_WIFI_OFF)
    display.drawBitmap(x, y, wifiOff, 12, 12, fg);
  else if (wifiState == TOP_STATUS_WIFI_ERROR)
    display.drawBitmap(x, y, wifiError, 13, 13, fg);
  else if (WiFi.RSSI() >= -60)
    display.drawBitmap(x, y, wifiOn, 12, 12, fg);
  else
    display.drawBitmap(x, y, wifiAvg, 12, 12, fg);
}

int16_t topStatusWifiX(byte wifiState)
{
  u8g2Fonts.setFont(u8g2_font_luRS08_tf);
  uint16_t timeWidth = u8g2Fonts.getUTF8Width("00:00");
  int16_t wifiWidth = (wifiState == TOP_STATUS_WIFI_ERROR) ? TOP_STATUS_WIFI_ERROR_WIDTH : TOP_STATUS_WIFI_WIDTH;
  return display.width() - TOP_STATUS_PADDING_X - timeWidth - TOP_STATUS_GAP - wifiWidth;
}

void drawTopStatusBar(float voltage, int percent, const char *timeText, bool invert, byte wifiState, const char *centerText)
{
  uint16_t bg = invert ? GxEPD_BLACK : GxEPD_WHITE;
  uint16_t fg = invert ? GxEPD_WHITE : GxEPD_BLACK;
  display.fillRect(0, 0, display.width(), TOP_STATUS_HEIGHT, bg);

  u8g2Fonts.setFont(u8g2_font_luRS08_tf);
  u8g2Fonts.setForegroundColor(fg);
  u8g2Fonts.setBackgroundColor(bg);

  char voltageText[8];
  char powerText[18];
  dtostrf(voltage, 0, 2, voltageText);
  if (voltage < 4)
    snprintf(powerText, sizeof(powerText), "%sV %d%%", voltageText, percent);
  else
    snprintf(powerText, sizeof(powerText), "USB");

  int16_t leftX = TOP_STATUS_PADDING_X;
  iconBattery(display, percent, invert, leftX, TOP_STATUS_BATTERY_Y);
  leftX += TOP_STATUS_BATTERY_WIDTH + TOP_STATUS_GAP;
  u8g2Fonts.setCursor(leftX, TOP_STATUS_TEXT_Y);
  u8g2Fonts.print(powerText);
  leftX += u8g2Fonts.getUTF8Width(powerText);

  uint16_t timeWidth = (timeText && timeText[0]) ? u8g2Fonts.getUTF8Width(timeText) : 0;
  int16_t wifiWidth = (wifiState == TOP_STATUS_WIFI_ERROR) ? TOP_STATUS_WIFI_ERROR_WIDTH : TOP_STATUS_WIFI_WIDTH;
  int16_t timeX = display.width() - TOP_STATUS_PADDING_X - timeWidth;
  int16_t wifiX = timeX - TOP_STATUS_GAP - wifiWidth;

  drawTopWifiIcon(wifiState, wifiX, TOP_STATUS_ICON_Y, invert);
  if (timeWidth > 0)
  {
    u8g2Fonts.setCursor(timeX, TOP_STATUS_TEXT_Y);
    u8g2Fonts.print(timeText);
  }

  if (centerText && centerText[0])
  {
    uint16_t centerWidth = u8g2Fonts.getUTF8Width(centerText);
    int16_t centerX = (display.width() - centerWidth) / 2;
    int16_t minCenterX = leftX + TOP_STATUS_GAP;
    int16_t maxCenterX = wifiX - TOP_STATUS_GAP - centerWidth;
    if (centerX < minCenterX)
      centerX = minCenterX;
    if (centerX <= maxCenterX)
    {
      u8g2Fonts.setCursor(centerX, TOP_STATUS_TEXT_Y);
      u8g2Fonts.print(centerText);
    }
  }
}

/**
 * @brief Disables WiFi and lowers CPU frequency
 * @param extreme If true, uses the lowest CPU frequency for critical-battery mode
 * @note Reduces CPU frequency and disables unused peripherals
 */
void turnOffWifi(bool extreme = false)
{
  // Disable WiFi
  WiFi.disconnect(true); // Disconnect and clear credentials
  WiFi.mode(WIFI_OFF);   // Set WiFi mode to off
  esp_wifi_stop();       // Stop WiFi
  // Additional power savings
  btStop(); // Disable Bluetooth - more compatible than esp_bt_controller_disable()
  // Reduce CPU frequency last
  if (extreme)
    setCpuFrequencyMhz(10); // Set CPU to 10MHz
  else
    setCpuFrequencyMhz(20); // Set CPU to 20MHz
  delay(5);                 // wait for 5ms
  if (DEBUG_MODE)
  {
    if (extreme)
      Serial.println("Critical-battery power saver: WiFi off, CPU reduced");
    else
      Serial.println("WiFi off, CPU reduced after network activity");
    Serial.print("CPU frequency MHz: ");
    Serial.println(getCpuFrequencyMhz());
  }
}

/**
 * @brief Updates RTC time from NTP server if necessary
 * If an update is needed and WiFi is connected, it fetches the current time
 * from an NTP server and updates the RTC.
 *
 * @return bool Returns true if the time was successfully updated, false otherwise
 * @note Requires an active WiFi connection to function
 */
bool autoTimeUpdate()
{
  if (!RTC_READY)
    return false;

  if (WiFi.status() == WL_CONNECTED)
  {
    timeClient.begin();
    if (timeClient.update() && timeClient.isTimeSet())
    {
      time_t rawtime = timeClient.getEpochTime();
      struct tm *ti = localtime(&rawtime);

      uint16_t year = ti->tm_year + 1900;
      uint8_t month = ti->tm_mon + 1;
      uint8_t day = ti->tm_mday;

      rtc.adjust(DateTime(year, month, day,
                          timeClient.getHours(),
                          timeClient.getMinutes(),
                          timeClient.getSeconds()));

      if (DEBUG_MODE)
      {
        Serial.print("RTC updated: ");
        Serial.print(year);
        Serial.print("-");
        Serial.print(month);
        Serial.print("-");
        Serial.println(day);
      }
      return true;
    }
    else
      return false;
  }
  else
    return false;
}

/**
 * @brief Prints temperature and environmental data
 * @param offset Vertical offset for display positioning (default: 0)
 * @param invert Inverts colors for ghost protection (default: false)
 */
void onlineTimePrint(bool invert = false);

/**
 * @brief Displays network debugging information
 * @param msg Message to display in debug info
 */
void networkInfo(const String &msg = "");

/**
 * @brief Prints a compact alert line without replacing the full screen
 * @param msg Alert text to print
 * @param invert Clears the alert strip with the current screen background color
 */
void printAlertLine(const String &msg, bool invert);

/**
 * @brief Fetches and displays weather data
 * @param invert Inverts display colors for ghost protection
 * @note Requires active WiFi connection and valid API keys
 */
void weatherPrint(bool invert = false);

//=============== MAIN SETUP AND LOOP ===============

/**
 * @brief Initialize and configure all hardware and software components
 *
 * This function performs the following initializations:
 * 1. CPU and Debug Configuration
 *    - Sets CPU frequency to power-saving mode (20MHz)
 *    - Initializes serial communication if in debug mode
 *    - Configures debug pin and mode
 *
 * 2. Power Management
 *    - Initializes battery monitoring
 *    - Manages critical battery state
 *    - Configures WiFi power state based on battery level
 *
 * 3. Hardware Initialization
 *    - Configures I2C communication
 *    - Initializes e-paper display
 *    - Sets up environmental sensors (TMP117, BME680)
 *    - Configures light sensor (BH1750)
 *
 * 4. State Management
 *    - Handles night mode transitions
 *    - Manages data persistence with preferences
 *    - Updates high/low temperature records
 *
 * 5. Network Configuration
 *    - Handles WiFi setup and connection
 *    - Configures NTP time synchronization
 *    - Sets up weather API access
 *
 * 6. Display Functions
 *    - Updates screen based on current state
 *    - Handles ghost protection display rotation
 *    - Shows status information and sensor data
 *
 * @note Enters deep sleep mode after completion unless in debug mode
 * @note Some features are disabled when battery is critical
 */

void setup()
{
  setCpuFrequencyMhz(20); // Set CPU to 20MHz
  pinMode(BATPIN, INPUT);
  pinMode(DEBUG_PIN, INPUT);
  if (digitalRead(DEBUG_PIN) == 1) // Check if debug mode is enabled
    DEBUG_MODE = true;
  if (DEBUG_MODE)
  {
    Serial.begin(115200);
    Serial.println("Setup");
    Serial.println(getCpuFrequencyMhz());
  }
  pref.begin("database", false); // Open the preferences "database"

  if (BATTERY_CRITICAL)
    turnOffWifi(true); // turn off wifi to save power when battery is critical

  Wire.begin();                         // Start the I2C communication
  Wire.setClock(400000);                // Set clock speed to be the fastest for better communication (fast mode)
  analogReadResolution(12);             // Set ADC resolution to 12-bit
  display.init(115200, true, 2, false); // USE THIS for Waveshare boards with "clever" reset circuit, 2ms reset pulse

  u8g2Fonts.begin(display); // connect u8g2 procedures to Adafruit GFX

  /*if (lightMeter.begin(BH1750::ONE_TIME_HIGH_RES_MODE))
  {
    if (DEBUG_MODE)
      Serial.println(F("BH1750 Advanced begin"));
  }
  else
  {
    if (DEBUG_MODE)
      Serial.println(F("Error initialising BH1750"));
    addSystemAlert("BH1750 ERROR");
  }
  float lux = 0; // Light level in lux
  while (!lightMeter.measurementReady(true))
  {
    yield(); // Wait for the measurement to be ready
  }*/
  float lux = 50; // lightMeter.readLightLevel(); // Get Lux value from sensor
  if (DEBUG_MODE)
  {
    Serial.print("Light: ");
    Serial.print(lux);
    Serial.println(" lx");
  }

  // if battery is critical, then no need to check wifi and weather api
  if ((!BATTERY_CRITICAL && lux != 0) || DEBUG_MODE == true)
  {
    if (!pref.isKey("ssid"))
    { // create key:value pairs
      pref.putString("ssid", "");
      pref.putString("password", "");
    }

    ssid = pref.getString("ssid", "");
    password = pref.getString("password", "");

    if (ssid == "" || password == "")
    {
      setCpuFrequencyMhz(80); // Set CPU to 80MHz for wifi manager
      // if no ssid or password saved, then start the wifi manager
      if (DEBUG_MODE)
        Serial.println("No values saved for ssid or password");
      // Connect to Wi-Fi network with SSID and password
      if (DEBUG_MODE)
        Serial.println("Setting AP (Access Point)");
      // NULL sets an open Access Point
      WiFi.softAP("WCLOCK-WIFI-MANAGER", NULL);

      IPAddress IP = WiFi.softAPIP();
      if (DEBUG_MODE)
      {
        Serial.print("AP IP address: ");
        Serial.println(IP);
      }

      debugPrinter("Connect to 'WCLOCK-WIFI-MANAGER' \nfrom your phone or computer (Wifi).\n\nThen go to " + IP.toString() + "\nfrom your browser.");

      // Web Server Root URL
      server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
                { request->send(200, "text/html", index_html); });

      server.on("/", HTTP_POST, [](AsyncWebServerRequest *request)
                {
        int params = request->params();
        for (int i = 0; i < params; i++) {
          const AsyncWebParameter *p = request->getParam(i);
          if (p->isPost()) {
            // HTTP POST ssid value
            if (p->name() == PARAM_INPUT_1) {
              ssid = p->value();
              if (DEBUG_MODE) {
                Serial.print("SSID set to: ");
                Serial.println(ssid);
              }
              ssid.trim();
              pref.putString("ssid", ssid);
            }
            // HTTP POST pass value
            if (p->name() == PARAM_INPUT_2) {
              password = p->value();
              if (DEBUG_MODE) {
                Serial.print("Password set to: ");
                Serial.println(password);
              }
              password.trim();  // remove leading and trailing spaces
              pref.putString("password", password);
            }
            //if (DEBUG_MODE) Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
          }
        }
        request->send(200, "text/html", "<h2>Done. Weather Station will now restart</h2>");
        delay(2000);
        ESP.restart(); });
      server.begin();
      while (true)
        yield(); // Runs forever
    }
  }

  // if lux is 0, then the device is in dark mode and no need to initialize sensors
  if (lux != 0 || DEBUG_MODE == true)
  {
    RTC_READY = rtc.begin();
    if (RTC_READY)
    {
      if (DEBUG_MODE)
        Serial.println("RTC Ready");

      DateTime now = rtc.now();
      if ((now.hour() == 0) && (now.minute() >= 0 && now.minute() < = 15))
      { // reset high low at midnight
        hTemp = 0.0;
        lTemp = 60.0;
      }
    }
    else
    {
      if (DEBUG_MODE)
        Serial.println("RTC unavailable; using 00:00 and placeholder date");
      addSystemAlert("RTC ERROR");
    }

    TMP117_READY = sensor.begin();
    if (TMP117_READY) // Function to check if the TMP117 will correctly self-identify with the proper Device ID/Address
    {
      if (DEBUG_MODE)
        Serial.println("TMP117 Begin");
    }
    else
    {
      if (DEBUG_MODE)
        Serial.println("TMP117 unavailable; displaying -- for indoor temperature");
      addSystemAlert("TMP117 ERROR");
    }

    BME680_READY = bme.begin();
    if (BME680_READY)
    {
      if (DEBUG_MODE)
        Serial.println("BME Ready");

      // Set up oversampling and filter initialization for accurate readings
      bme.setTemperatureOversampling(BME680_OS_2X);
      bme.setHumidityOversampling(BME680_OS_16X);
      bme.setPressureOversampling(BME680_OS_16X);
      bme.setIIRFilterSize(BME680_FILTER_SIZE_7);
      bme.setGasHeater(0, 0); // 320°C for 150 ms
    }
    else
    {
      if (DEBUG_MODE)
        Serial.println(F("BME680 unavailable; displaying -- for humidity and pressure"));
      addSystemAlert("BME680 ERROR");
    }

    if (!BATTERY_CRITICAL) // Connect to Wi-Fi network with SSID and password if battery is not critical
    {
      setCpuFrequencyMhz(80); // Set CPU to 80MHz for wifi
      delay(2);
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid.c_str(), password.c_str());
      if (WiFi.waitForConnectResult() != WL_CONNECTED)
      {
        if (DEBUG_MODE)
          Serial.println("WiFi connection failed");
      }

      if (WiFi.status() == WL_CONNECTED) // if wifi is connected
      {
        if (DEBUG_MODE)
        {
          Serial.println("IP Address: ");
          Serial.println(WiFi.localIP());
        }

        if (RTC_READY)
        {
          // Get the current day
          if (!pref.isKey("timeNeedsUpdate")) // create key:value pairs
            pref.putBool("timeNeedsUpdate", true);
          bool timeNeedsUpdate = pref.getBool("timeNeedsUpdate", false);

          DateTime now = rtc.now();
          if ((now.year() == 1970) || rtc.lostPower()) // if RTC lost power or not set
            timeNeedsUpdate = true;

          // Get the current day
          byte currentDay = now.day();

          // Check if we need to update time (every 15 days)
          if (!pref.isKey("lastCheckedDay")) // create key:value pairs
            pref.putUChar("lastCheckedDay", 0);
          byte lastCheckedDay = pref.getUChar("lastCheckedDay", 0);
          byte daysPassed = (currentDay - lastCheckedDay + 31) % 31;

          if ((daysPassed >= 15) || timeNeedsUpdate) // check if 15 days passed or force update
          {
            Serial.println("RTC sync needed; updating time from NTP server");
            if (autoTimeUpdate()) // Update time from NTP server
            {
              if (DEBUG_MODE)
                Serial.println("RTC sync succeeded");
              timeNeedsUpdate = false;
            }
            else
            {
              if (DEBUG_MODE)
                Serial.println("RTC sync failed");
            }
            pref.putBool("timeNeedsUpdate", timeNeedsUpdate);
            pref.putUChar("lastCheckedDay", currentDay); // Update last checked day
          }
          else
            Serial.println("RTC sync not required");
        }
        else if (DEBUG_MODE)
        {
          Serial.println("RTC unavailable, skipping RTC time sync");
        }

        // Check if the API keys are saved in the preferences
        if (!pref.isKey("api")) // create key:value pairs
          pref.putString("api", openWeatherMapApiKey);
        openWeatherMapApiKey = pref.getString("api", "");

        if (!pref.isKey("apiCustom")) // create key:value pairs
          pref.putString("apiCustom", customApiKey);
        customApiKey = pref.getString("apiCustom", "");
      }
      else
        turnOffWifi(); // turn off wifi to save power when wifi is not connected
    }
  }

  if (DEBUG_MODE)
    Serial.println("Setup done");

  if (lux == 0)
  {
    TIME_TO_SLEEP = 300; // 5 min sleep time in dark mode
    if (nightFlag == 0)  // prevents unnecessary redrawing of same thing in dark mode
    {
      nightFlag = 1;
      display.setRotation(0);
      display.setFullWindow();
      display.firstPage();
      do
      {
        display.fillScreen(GxEPD_WHITE);
        display.drawInvertedBitmap(0, 0, nightMode, 400, 300, GxEPD_BLACK); // display sleep icon
      } while (display.nextPage());
    }
    display.hibernate();
    display.powerOff();
  }
  else // if lux is not 0, then the device is in normal mode
  {
    nightFlag = 0;
    display.setRotation(0);
    display.setFullWindow();
    display.firstPage();
    do
    {
      if (WiFi.status() == WL_CONNECTED) // if wifi is connected, then fetch weather data
      {
        ++bootCount; // increment the boot counter
        if (DEBUG_MODE)
          Serial.println("Drawing online time and weather screen");
        if (bootCount == ghostProtek)
        {
          display.fillScreen(GxEPD_BLACK);
          onlineTimePrint(true); // prints temperature and battery level
          weatherPrint(true);    // prints weather data
        }
        else // if not ghost protection, then normal display
        {
          display.fillScreen(GxEPD_WHITE);
          onlineTimePrint(); // prints temperature and battery level
          weatherPrint();    // prints weather data
        }
        if (bootCount == ghostProtek) // reset boot counter after ghost protection
          bootCount = 0;
        if (DEBUG_MODE)
          Serial.println("Online time and weather screen drawn");
        // Turn off WiFi as soon as possible
        turnOffWifi();
      }
      else // if wifi is not connected, then only display time
      {
        display.fillScreen(GxEPD_WHITE);
        if (DEBUG_MODE)
          Serial.println("Drawing offline time screen");
        offlineTimePrint();
        if (DEBUG_MODE)
          Serial.println("Offline time screen drawn");
        if (BATTERY_CRITICAL)
          TIME_TO_SLEEP = 1800; // 30 min sleep time when battery is critical (POWER SAVER MODE)
      }
    } while (display.nextPage());
    display.hibernate();
    display.powerOff();
  }

  if (DEBUG_MODE)
    Serial.println("Closing preferences and I2C");

  pref.end(); // Close the preferences
  Wire.end(); // End I2C communication

  if (DEBUG_MODE)
  {
    Serial.println("Preferences closed; I2C stopped");
    Serial.print("Configured sleep interval: ");
    Serial.print(TIME_TO_SLEEP / 60);
    Serial.println(" min");
    Serial.flush(); // Flush the serial buffer
    delay(5);
  }

  if (!DEBUG_MODE) // if debug mode is off, then go to deep sleep
  {
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR); // Set the sleep time
    esp_deep_sleep_start();                                        // Enter deep sleep
  }
  else
    Serial.println("DEBUG_MODE active: staying awake and entering loop");
}

/**
 * @brief Main loop function that runs continuously in debug mode
 *
 * This function only executes when DEBUG_MODE is true. It provides
 * continuous monitoring and debugging capabilities by:
 * 1. Checking timing conditions every timerDelay1 interval
 * 2. Displaying debug messages on the e-paper display
 * 3. Allowing for interactive testing and monitoring
 *
 * Future debug functionality can be added within the timer check.
 * The function uses non-blocking delays via millis() to maintain
 * responsiveness.
 *
 * @note This loop is skipped during normal operation (DEBUG_MODE = false)
 * @note Uses timerDelay1 (60s) to prevent excessive display updates
 */
void loop()
{
  if ((millis() - lastTime1) > timerDelay1)
  {
    Serial.println("In LOOP");
    // addSystemAlert("DEBUG MODE"); // Display debug message
    //  Additional debug functions can be added here
    lastTime1 = millis();
  }
  yield();
}

//=============== WEATHER AND DISPLAY FUNCTIONS ===============

/**
 * @brief Fetches weather data from API endpoint
 * @param serverName URL of the weather API endpoint
 * @return String JSON response from server
 */
String weatherDataAPI(const char *serverName)
{
  WiFiClient client;
  HTTPClient http;

  // Your Domain name with URL path or IP address with path
  http.begin(client, serverName);
  http.setTimeout(8000);

  // Send HTTP POST request
  httpResponseCode = http.GET();

  String payload = "{}";

  if (httpResponseCode > 0)
  {
    if (DEBUG_MODE)
    {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    }
    payload = http.getString();
  }
  else
  {
    if (DEBUG_MODE)
    {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
  }
  // Free resources
  http.end();

  return payload;
}

/**
 * @brief Checks HTTP response code and displays error if needed
 * @param source String identifying the API source for debug messages
 * @return bool Returns true if response code is 200, false otherwise
 */
bool checkHttpResponse(const char *source)
{
  if (httpResponseCode != 200)
  {
    if (DEBUG_MODE)
    {
      Serial.print(source);
      Serial.print(" API request failed with code: ");
      Serial.println(httpResponseCode);
    }
    if (httpResponseCode == -1 || httpResponseCode == -11)
      addSystemAlert("WEATHER API ERROR");
    networkInfo(source);
    return false;
  }
  return true;
}

/**
 * @brief Prints temperature and environmental data when WiFi is connected
 * @param invert Inverts colors for ghost protection
 *
 * Layout groups:
 * 1. Sensor/cache refresh
 * 2. Header: battery, status, update time
 * 3. Main panel: date and indoor temperature
 * 4. Footer strip: humidity, pressure, high/low temperature
 */
void onlineTimePrint(bool invert)
{
  if (DEBUG_MODE)
    Serial.println("Online Time Print");

  //=============== 1. SENSOR AND STATE REFRESH ===============
  float tempC = 0;
  bool tempReady = TMP117_READY && sensor.dataReady();
  if (tempReady)
  {
    tempC = sensor.readTempC();
    hTemp = max(hTemp, tempC);
    lTemp = min(lTemp, tempC);
  }

  float newBattLevel = batteryLevel();
  bool acceptBatteryRise = ((newBattLevel - battLevel) >= battChangeThreshold) || (newBattLevel > battHigh);
  if (newBattLevel < battLevel || acceptBatteryRise)
    battLevel = newBattLevel;

  int percent = constrain(((battLevel - battLow) / (battHigh - battLow)) * 100, 0, 100);
  BATTERY_CRITICAL = percent < critBattPercent;

  // Start BME680 sampling early so display drawing hides part of the wait.
  bool bmeStarted = BME680_READY && bme.beginReading();

  //=============== 2. DISPLAY STYLE ===============
  uint16_t bg = invert ? GxEPD_BLACK : GxEPD_WHITE;
  uint16_t fg = invert ? GxEPD_WHITE : GxEPD_BLACK;
  uint16_t lineColor = (BATTERY_CRITICAL || invert) ? GxEPD_WHITE : GxEPD_RED;

  u8g2Fonts.setFontMode(1);
  u8g2Fonts.setFontDirection(0);
  u8g2Fonts.setForegroundColor(fg);
  u8g2Fonts.setBackgroundColor(bg);

  //=============== 3. HEADER: POWER, WIFI, UPDATE TIME ===============
  byte currentHour = 0;
  byte currentMinute = 0;
  byte currentDay = 0;
  byte currentMonth = 0;
  byte currentDayOfWeek = 0;

  if (RTC_READY)
  {
    DateTime now = rtc.now();
    currentHour = now.hour();
    currentMinute = now.minute();
    currentDay = now.day();
    currentMonth = now.month();
    currentDayOfWeek = now.dayOfTheWeek();
  }

  char timeStr[6] = "--:--";
  if (RTC_READY)
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d", currentHour, currentMinute);

  drawTopStatusBar(battLevel, percent, timeStr, invert, TOP_STATUS_WIFI_CONNECTED, invert ? "GHOSTING PROTECTION" : "");

  //=============== 4. MAIN PANEL: DATE AND INDOOR TEMP ===============
  u8g2Fonts.setFont(u8g2_font_logisoso20_tf);
  u8g2Fonts.setCursor(10, 75);
  if (RTC_READY)
  {
    if (currentDay < 10)
      u8g2Fonts.print("0");
    u8g2Fonts.print(currentDay);
    u8g2Fonts.print(", ");
    u8g2Fonts.print(monthName[currentMonth - 1]);
  }
  else
    u8g2Fonts.print("--, ---");

  u8g2Fonts.setCursor(10, 105);
  u8g2Fonts.print(RTC_READY ? daysOfTheWeek[currentDayOfWeek] : "---");

  u8g2Fonts.setFont(u8g2_font_inb19_mf);
  u8g2Fonts.setCursor(320, 60);
  u8g2Fonts.print("o");

  u8g2Fonts.setFont(u8g2_font_logisoso58_tf);
  u8g2Fonts.setCursor(150, 110);
  if (tempReady)
    u8g2Fonts.print(tempC);
  else
    u8g2Fonts.print("--");
  u8g2Fonts.setCursor(330, 110);
  u8g2Fonts.print("C");

  display.fillRect(0, 121, 400, 2, lineColor);
  display.fillRect(0, 154, 400, 2, lineColor);

  //=============== 5. FOOTER STRIP: ENVIRONMENT AND HIGH/LOW ===============
  bool bmeReady = bmeStarted && bme.endReading();
  if (BME680_READY && !bmeReady)
  {
    if (DEBUG_MODE)
      Serial.println("BME READING ERROR");
    addSystemAlert("BME680 READ");
  }

  u8g2Fonts.setFont(u8g2_font_logisoso20_tf);
  u8g2Fonts.setCursor(2, 150);
  if (bmeReady)
    u8g2Fonts.print(bme.humidity);
  else
    u8g2Fonts.print("--");
  u8g2Fonts.print("%");

  u8g2Fonts.setCursor(264, 150);
  if (bmeReady)
    u8g2Fonts.print(bme.pressure / 100.0);
  else
    u8g2Fonts.print("--");
  u8g2Fonts.print("hPa");

  u8g2Fonts.setFont(u8g2_font_logisoso16_tf);
  const char *labels[] = {"H:", "L:"};
  float temps[] = {hTemp, lTemp};
  int positions[] = {85, 180};

  for (int i = 0; i < 2; i++)
  {
    u8g2Fonts.setCursor(positions[i], 148);
    u8g2Fonts.print(labels[i]);
    if (tempReady)
      u8g2Fonts.print(temps[i]);
    else
      u8g2Fonts.print("--");
    u8g2Fonts.setFont(u8g2_font_fub11_tf);
    u8g2Fonts.setCursor(positions[i] + 63, 138);
    u8g2Fonts.print("o");
    u8g2Fonts.setFont(u8g2_font_logisoso16_tf);
    u8g2Fonts.setCursor(positions[i] + 73, 148);
    u8g2Fonts.print("C");
  }

  printAlertLine(currentAlertText(), invert);

  if (DEBUG_MODE)
    Serial.println("Online Time Print Done");
}

/**
 * @brief Prints only time and temperature data (no weather)
 * @note Used when WiFi is not connected or in offline mode
 */
void offlineTimePrint()
{
  // Temperature reading
  float tempC = 0;
  bool tempReady = TMP117_READY && sensor.dataReady();
  if (tempReady)
  {
    tempC = sensor.readTempC();
    hTemp = max(hTemp, tempC);
    lTemp = min(lTemp, tempC);
  }

  // Battery level handling
  float newBattLevel = batteryLevel();
  battLevel = (newBattLevel < battLevel) ? newBattLevel : ((newBattLevel - battLevel) >= battChangeThreshold || newBattLevel > battHigh) ? newBattLevel
                                                                                                                                         : battLevel;

  int percent = constrain(((battLevel - battLow) / (battHigh - battLow)) * 100, 0, 100);
  BATTERY_CRITICAL = percent < critBattPercent;

  // Start BME680 sampling early so display drawing hides part of the wait.
  bool bmeStarted = BME680_READY && bme.beginReading();

  // Configure fonts and colors once at the start
  uint16_t bg = GxEPD_WHITE;
  uint16_t fg = GxEPD_BLACK;
  uint16_t lineColor = GxEPD_RED;

  u8g2Fonts.setFontMode(1);
  u8g2Fonts.setFontDirection(0);
  u8g2Fonts.setForegroundColor(fg);
  u8g2Fonts.setBackgroundColor(bg);

  byte currentHour = 0;
  byte currentMinute = 0;
  byte currentDay = 0;
  byte currentMonth = 0;
  byte currentDayOfWeek = 0;
  uint16_t currentYear = 0;

  if (RTC_READY)
  {
    DateTime now = rtc.now();
    currentHour = now.hour();
    currentMinute = now.minute();
    currentDay = now.day();
    currentMonth = now.month();
    currentYear = now.year();
    currentDayOfWeek = now.dayOfTheWeek();
  }

  char timeStr[6] = "--:--";
  if (RTC_READY)
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d", currentHour, currentMinute);

  drawTopStatusBar(battLevel, percent, timeStr, false, TOP_STATUS_WIFI_OFF, "OFFLINE MODE");

  // Main temperature display
  u8g2Fonts.setFont(u8g2_font_logisoso58_tf);
  u8g2Fonts.setCursor(100, 110);
  if (tempReady)
    u8g2Fonts.print(tempC);
  else
    u8g2Fonts.print("--");
  u8g2Fonts.setCursor(280, 110);
  u8g2Fonts.print("C");
  u8g2Fonts.setFont(u8g2_font_inb19_mf);
  u8g2Fonts.setCursor(270, 60);
  u8g2Fonts.print("o");

  u8g2Fonts.setFont(u8g2_font_logisoso20_tf);
  u8g2Fonts.setCursor(10, 150);
  if (RTC_READY)
  {
    if (currentDay < 10)
      u8g2Fonts.print("0");
    u8g2Fonts.print(currentDay);
    u8g2Fonts.print("/");
    u8g2Fonts.print(fullMonthName[currentMonth - 1]);
    u8g2Fonts.print("/");
    u8g2Fonts.print(currentYear);
  }
  else
    u8g2Fonts.print("--/--/----");

  u8g2Fonts.setCursor(270, 150);
  u8g2Fonts.print(RTC_READY ? fullDaysOfTheWeek[currentDayOfWeek] : "---");

  if (!BATTERY_CRITICAL)
  {
    display.fillRect(0, 160, 400, 3, lineColor); // top line

    display.fillRect(0, 259, 400, 3, lineColor); // bottom line

    display.fillRect(0, 160, 3, 100, lineColor); // vertical line first

    display.fillRect(150, 160, 3, 100, lineColor); // vertical line second

    display.fillRect(300, 160, 3, 100, lineColor); // vertical line second

    display.fillRect(397, 160, 3, 100, lineColor); // vertical line last
  }
  // Environmental readings
  bool bmeReady = bmeStarted && bme.endReading();
  if (BME680_READY && !bmeReady)
  {
    if (DEBUG_MODE)
      Serial.println("BME READING ERROR");
    addSystemAlert("BME680 READ");
  }

  u8g2Fonts.setFont(u8g2_font_logisoso20_tf);
  float temps[] = {hTemp, lTemp};
  u8g2Fonts.setCursor(6, 200);
  u8g2Fonts.print("HIGH:");
  if (tempReady)
    u8g2Fonts.print(temps[0]);
  else
    u8g2Fonts.print("--");
  u8g2Fonts.setFont(u8g2_font_fub11_tf);
  u8g2Fonts.setCursor(124, 185);
  u8g2Fonts.print("o");
  u8g2Fonts.setFont(u8g2_font_logisoso20_tf);
  u8g2Fonts.setCursor(133, 200);
  u8g2Fonts.print("C");

  u8g2Fonts.setCursor(6, 240);
  u8g2Fonts.print("LOW:");
  if (tempReady)
    u8g2Fonts.print(temps[1]);
  else
    u8g2Fonts.print("--");
  u8g2Fonts.setFont(u8g2_font_fub11_tf);
  u8g2Fonts.setCursor(116, 225);
  u8g2Fonts.print("o");
  u8g2Fonts.setFont(u8g2_font_logisoso20_tf);
  u8g2Fonts.setCursor(124, 240);
  u8g2Fonts.print("C");

  // Display environmental data
  u8g2Fonts.setFont(u8g2_font_logisoso20_tf);

  display.drawBitmap(155, 174, humiIcon, 32, 32, fg);
  u8g2Fonts.setCursor(188, 200);
  if (bmeReady)
    u8g2Fonts.print(bme.humidity);
  else
    u8g2Fonts.print("--");
  u8g2Fonts.print("%");

  // Draw gauge icon for pressure
  display.drawBitmap(155, 214, gaugeIcon, 32, 32, fg);
  u8g2Fonts.setCursor(188, 240);
  if (bmeReady)
    u8g2Fonts.print(bme.pressure / 100.0);
  else
    u8g2Fonts.print("--");
  u8g2Fonts.setFont(u8g2_font_6x13_tf);
  u8g2Fonts.print("hPa");

  // Last update time
  u8g2Fonts.setFont(u8g2_font_8x13_tf);
  u8g2Fonts.setCursor(306, 190);
  u8g2Fonts.print("Last Update");
  u8g2Fonts.setFont(u8g2_font_logisoso20_tf);
  u8g2Fonts.setCursor(319, 240);
  u8g2Fonts.print(timeStr);

  printAlertLine(currentAlertText(), false);
}

/**
 * @brief Draws the OpenWeather icon code on the weather panel
 * @param icon OpenWeather icon code
 * @param invert Inverts display colors for ghost protection
 */
void drawCurrentWeatherIcon(const char *icon, bool invert)
{
  if (strcmp(icon, "01d") == 0)
    iconSun(display, 361, 189, 15, invert);
  else if (strcmp(icon, "01n") == 0)
    iconMoon(display, 361, 189, 15, invert);
  else if (strcmp(icon, "02d") == 0)
    iconCloudyDay(display, 330, 160, 60, invert);
  else if (strcmp(icon, "02n") == 0)
    iconCloudyNight(display, 330, 160, 60, invert);
  else if (strcmp(icon, "03d") == 0 || strcmp(icon, "03n") == 0)
    iconCloud(display, 361, 189, 15, invert);
  else if (strcmp(icon, "04d") == 0 || strcmp(icon, "04n") == 0)
    iconCloudy(display, 330, 160, 60, invert);
  else if (strcmp(icon, "09d") == 0 || strcmp(icon, "09n") == 0)
    iconSleet(display, 330, 160, 60, invert);
  else if (strcmp(icon, "10d") == 0 || strcmp(icon, "10n") == 0)
    iconRain(display, 330, 160, 60, invert);
  else if (strcmp(icon, "11d") == 0 || strcmp(icon, "11n") == 0)
    iconThunderstorm(display, 330, 160, 60, invert);
  else if (strcmp(icon, "13d") == 0 || strcmp(icon, "13n") == 0)
    iconSnow(display, 330, 160, 60, invert);
  else if (strcmp(icon, "50d") == 0 || strcmp(icon, "50n") == 0)
    iconFog(display, 330, 160, 60, invert);
}

void printUvRisk(float uv)
{
  if (uv < 2)
    u8g2Fonts.print(" Low");
  else if (uv < 5)
    u8g2Fonts.print(" Medium");
  else if (uv <= 7)
    u8g2Fonts.print(" High");
  else
    u8g2Fonts.print(" Danger");
}

/**
 * @brief Fetches and displays weather data
 * @param invert Inverts display colors for ghost protection
 * @note Requires active WiFi connection and valid API keys
 */
void weatherPrint(bool invert)
{
  uint16_t bg = invert ? GxEPD_BLACK : GxEPD_WHITE;
  uint16_t fg = invert ? GxEPD_WHITE : GxEPD_BLACK;
  uint16_t red = invert ? GxEPD_WHITE : GxEPD_RED;

  char serverPath[256];
  int urlLength = snprintf(serverPath, sizeof(serverPath), "%s%s&lon=%s%s%s",
                           OPEN_WEATHER_BASE_URL,
                           lat.c_str(),
                           lon.c_str(),
                           OPEN_WEATHER_PARAMS,
                           openWeatherMapApiKey.c_str());
  if (urlLength < 0 || urlLength >= int(sizeof(serverPath)))
  {
    if (DEBUG_MODE)
      Serial.println("OpenWeather URL too long");
    networkInfo("OpenWeather URL");
    return;
  }

  jsonBuffer = weatherDataAPI(serverPath);
  if (!checkHttpResponse("OpenWeather"))
    return;
  if (DEBUG_MODE)
    Serial.println(jsonBuffer);

  JsonDocument myObject;
  DeserializationError error = deserializeJson(myObject, jsonBuffer);

  if (error)
  {
    if (DEBUG_MODE)
    {
      Serial.print("OpenWeather JSON parse failed: ");
      Serial.println(error.c_str());
    }
    addSystemAlert("WEATHER JSON ERROR");
    networkInfo("OpenWeather JSON");
    return;
  }

  urlLength = snprintf(serverPath, sizeof(serverPath), "%s%s",
                       CUSTOM_WEATHER_BASE_URL,
                       customApiKey.c_str());
  if (urlLength < 0 || urlLength >= int(sizeof(serverPath)))
  {
    if (DEBUG_MODE)
      Serial.println("Custom weather URL too long");
    networkInfo("Custom URL");
    return;
  }

  jsonBuffer = weatherDataAPI(serverPath);
  if (!checkHttpResponse("Custom"))
    return;
  if (DEBUG_MODE)
    Serial.println(jsonBuffer);

  JsonDocument customObject;
  error = deserializeJson(customObject, jsonBuffer);

  if (error)
  {
    if (DEBUG_MODE)
    {
      Serial.print("Custom weather JSON parse failed: ");
      Serial.println(error.c_str());
    }
    addSystemAlert("WEATHER JSON ERROR");
    networkInfo("Custom JSON");
    return;
  }

  if (myObject["current"]["temp"].isNull() || customObject["data"]["temp"].isNull())
  {
    networkInfo();
    return;
  }

  u8g2Fonts.setFontMode(1);
  u8g2Fonts.setFontDirection(0);
  u8g2Fonts.setForegroundColor(fg);
  u8g2Fonts.setBackgroundColor(bg);

  u8g2Fonts.setFont(u8g2_font_helvB10_tf);
  u8g2Fonts.setCursor(29, 170);
  u8g2Fonts.print("OUTDOOR");

  float outdoorTemp = customObject["data"]["temp"].as<float>();
  float feelsLike = myObject["current"]["feels_like"].as<float>();
  char outdoorTempText[12];
  char feelsLikeValue[12];
  char feelsLikeText[24];
  dtostrf(outdoorTemp, 0, 2, outdoorTempText);
  dtostrf(feelsLike, 0, 2, feelsLikeValue);
  snprintf(feelsLikeText, sizeof(feelsLikeText), "Real Feel:%s", feelsLikeValue);

  u8g2Fonts.setFont(u8g2_font_fub20_tf);
  uint16_t width = u8g2Fonts.getUTF8Width(outdoorTempText);
  u8g2Fonts.setCursor(20, 200);
  u8g2Fonts.print(outdoorTempText);
  u8g2Fonts.setCursor(30 + width, 200);
  u8g2Fonts.print("C");
  u8g2Fonts.setFont(u8g2_font_fub11_tf);
  u8g2Fonts.setCursor(22 + width, 185);
  u8g2Fonts.print("o");

  u8g2Fonts.setFont(u8g2_font_fur11_tf);
  width = u8g2Fonts.getUTF8Width(feelsLikeText);
  u8g2Fonts.setCursor(5, 220);
  u8g2Fonts.print("Real Feel:");
  u8g2Fonts.setCursor(75, 220);
  u8g2Fonts.print(feelsLikeValue);
  u8g2Fonts.setCursor(width + 16, 220);
  u8g2Fonts.print("C");
  u8g2Fonts.setFont(u8g2_font_baby_tf);
  u8g2Fonts.setCursor(13 + width, 211);
  u8g2Fonts.print("o");

  u8g2Fonts.setFont(u8g2_font_fur14_tf);
  u8g2Fonts.setCursor(5, 245);
  u8g2Fonts.print(customObject["data"]["humidity"].as<float>());
  u8g2Fonts.print("%");

  u8g2Fonts.setCursor(5, 270);
  u8g2Fonts.print(customObject["data"]["pressure"].as<float>());
  u8g2Fonts.print("hPa");

  u8g2Fonts.setFont(u8g2_font_helvB10_tf);
  u8g2Fonts.setCursor(5, 294);
  u8g2Fonts.print("UVI: ");
  float uv = myObject["current"]["uvi"].as<float>();
  u8g2Fonts.print(uv);
  u8g2Fonts.setFont(u8g2_font_fur11_tf);
  printUvRisk(uv);

  display.fillRect(136, 155, 2, 144, red);

  char timeBuffer[6];
  for (int i = 0; i < 2; i++)
  {
    const char *key = (i == 0) ? "sunrise" : "sunset";
    time_t t = myObject["current"][key] | 0;
    if (t > 0)
    {
      setTime(t);
      adjustTime(19800);
      snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%02d", hour(), minute());
      iconSunRise(display, i == 0 ? 152 : 267, 170, i == 0, invert);
      u8g2Fonts.setCursor(i == 0 ? 166 : 281, 175);
      u8g2Fonts.print(timeBuffer);
    }
  }

  display.fillRect(320, 155, 2, 144, red);
  display.fillRect(320, 230, 80, 2, red);

  iconMoonPhase(display, 360, 260, 20, double(myObject["daily"][0]["moon_phase"].as<float>()), invert);
  u8g2Fonts.setFont(u8g2_font_luRS08_tf);
  u8g2Fonts.setCursor(330, 297);
  u8g2Fonts.print("Moon Phase");

  drawCurrentWeatherIcon(myObject["current"]["weather"][0]["icon"] | "", invert);

  u8g2Fonts.setFont(u8g2_font_luRS08_tf);
  u8g2Fonts.setCursor(330, 227);
  u8g2Fonts.print(myObject["current"]["weather"][0]["main"] | "");

  String alertText = currentAlertText();
  if (myObject.containsKey("alerts") && myObject["alerts"].size() > 0)
  {
    String weatherAlert = myObject["alerts"][0]["event"] | "";
    if (weatherAlert.length() > 0)
    {
      if (DEBUG_MODE)
        Serial.println("Alert: " + weatherAlert);

      if (alertText.length() > 0)
        alertText += " | ";
      alertText += weatherAlert;
    }
  }
  printAlertLine(alertText, invert);
}

//=============== UI HELPER FUNCTIONS ===============

/**
 * @brief Displays network debugging information
 * @note Shows WiFi status, signal strength, and HTTP response codes
 */
void networkInfo(const String &msg)
{
  bool connected = WiFi.status() == WL_CONNECTED;
  int rssi = WiFi.RSSI();

  drawTopWifiIcon(TOP_STATUS_WIFI_ERROR, topStatusWifiX(TOP_STATUS_WIFI_ERROR), TOP_STATUS_ICON_Y, false);
  display.drawBitmap(100, 160, net, 29, 28, GxEPD_BLACK);
  u8g2Fonts.setFont(u8g2_font_logisoso20_tf);
  u8g2Fonts.setCursor(145, 184); // start writing at this position
  u8g2Fonts.print("Network Debug");

  u8g2Fonts.setFont(u8g2_font_logisoso16_tf);
  u8g2Fonts.setCursor(5, 220); // start writing at this position
  u8g2Fonts.print("Connected: ");
  if (connected)
  {
    u8g2Fonts.print("Yes (");
    u8g2Fonts.print(WiFi.SSID());
    u8g2Fonts.print(")");
  }
  else
    u8g2Fonts.print("No");

  u8g2Fonts.setCursor(5, 245); // start writing at this position
  u8g2Fonts.print("HTTP Code: ");
  u8g2Fonts.print(httpResponseCode);

  u8g2Fonts.setCursor(5, 270); // start writing at this position
  u8g2Fonts.print("WiFi RSSI: ");
  u8g2Fonts.print(rssi);

  if (rssi > -50)
    u8g2Fonts.print(" Excellent");
  else if (rssi > -60)
    u8g2Fonts.print(" Good");
  else if (rssi > -70)
    u8g2Fonts.print(" Fair");
  else
    u8g2Fonts.print(" Poor");

  u8g2Fonts.setCursor(5, 295); // start writing at this position
  u8g2Fonts.print("Comments: ");
  u8g2Fonts.print(msg);
}

/**
 * @brief Displays WiFi signal strength indicator
 * @param invert Inverts icon colors for ghost protection
 */
void wifiStatus(bool invert)
{
  drawTopWifiIcon(TOP_STATUS_WIFI_CONNECTED, topStatusWifiX(TOP_STATUS_WIFI_CONNECTED), TOP_STATUS_ICON_Y, invert);
}

/**
 * @brief Adds an error to the compact alert line without stopping the clock
 * @note Kept as a compatibility wrapper for older call sites.
 */
void errMsg(String msg)
{
  addSystemAlert(msg.c_str());
}

/**
 * @brief Prints debug related msgs
 */
void debugPrinter(String msg)
{
  display.setRotation(0);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setFullWindow();
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(0, 20);
    display.print(msg);
  } while (display.nextPage());
}
