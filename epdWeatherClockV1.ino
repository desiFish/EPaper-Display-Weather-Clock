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
#include <Arduino_JSON.h>
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

/*
GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> display(GxEPD2_420c_Z21(20, 3, 4, 5)); //for XIAO_ESP32C3 in case the above declaration gives error
#define BATPIN 2      // Battery voltage divider pin (1M Ohm with 104 Capacitor)
#define DEBUG_PIN 21  // Debug mode toggle pin
*/
/*
GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> display(GxEPD2_420c_Z21(17, 1, 2, 21)); //for XIAO_ESP32C6
#define BATPIN 0      // Battery voltage divider pin (1M Ohm with 104 Capacitor)
#define DEBUG_PIN 16  // Debug mode toggle pin
*/

U8G2_FOR_ADAFRUIT_GFX u8g2Fonts; // u8g2 fonts

//=============== GLOBAL CONSTANTS ===============
#define BATTERY_LEVEL_SAMPLING 4 // BATTERY_LEVEL_SAMPLING: Number of samples to average for battery reading
#define battType 3.6             // battType: Battery nominal voltage (ICR: 4.2V, LFP: 3.6V) (Change accordingly)
#define battChangeThreshold 0.06 // battChangeThreshold: Minimum voltage change to update battery level
#define battHigh 3.3             // battHigh: Healthy battery threshold voltage (Change accordingly)
#define battLow 2.9              // battLow: Low battery warning threshold (Change accordingly)
#define critBattPercent 30       // critBattPercent: Critical battery percentage threshold

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

String jsonBuffer; // for storing json data from api

char daysOfTheWeek[7][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
char monthName[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

int httpResponseCode; // for storing http response code

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
 * @brief Disables WiFi and enters power saving mode
 * @param extreme If true, reduces CPU frequency to 10MHz
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
    Serial.println("Power saving mode enabled");
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
        Serial.println("RTC updated: " + String(year) + "-" +
                       String(month) + "-" + String(day));
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
void tempPrint(byte offset = 0, bool invert = false);

/**
 * @brief Displays network debugging information
 * @param msg Message to display in debug info
 */
void networkInfo(String msg = "");

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

  if (lightMeter.begin(BH1750::ONE_TIME_HIGH_RES_MODE))
  {
    if (DEBUG_MODE)
      Serial.println(F("BH1750 Advanced begin"));
  }
  else
  {
    if (DEBUG_MODE)
      Serial.println(F("Error initialising BH1750"));
    errMsg("Error BH1750");
    while (1)
      yield(); // Runs forever
  }
  float lux = 0; // Light level in lux
  while (!lightMeter.measurementReady(true))
  {
    yield(); // Wait for the measurement to be ready
  }
  lux = lightMeter.readLightLevel(); // Get Lux value from sensor
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
              password.trim(); // remove leading and trailing spaces
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
    if (!rtc.begin())
    {
      if (DEBUG_MODE)
        Serial.println("Couldn't find RTC");
      errMsg("Error RTC");
      while (1)
        ; // Runs forever
    }
    if (DEBUG_MODE)
      Serial.println("RTC Ready");
    DateTime now = rtc.now();

    if ((now.hour() == 0) && (now.minute() >= 0 && now.minute() < 15))
    { // reset high low at midnight
      hTemp = 0.0;
      lTemp = 60.0;
    }

    if (sensor.begin() == true) // Function to check if the TMP117 will correctly self-identify with the proper Device ID/Address
    {
      if (DEBUG_MODE)
        Serial.println("TMP117 Begin");
    }
    else
    {
      if (DEBUG_MODE)
        Serial.println("Device failed to setup- Freezing code.");
      errMsg("Error TMP117");
      while (1)
        ; // Runs forever
    }

    if (!bme.begin())
    {
      if (DEBUG_MODE)
        Serial.println(F("Could not find a valid BME680 sensor, check wiring!"));
      errMsg("Error BME680");
      while (1)
        ; // Runs forever
    }

    if (DEBUG_MODE)
      Serial.println("BME Ready");

    // Set up oversampling and filter initialization
    bme.setTemperatureOversampling(BME680_OS_2X);
    bme.setHumidityOversampling(BME680_OS_16X);
    bme.setPressureOversampling(BME680_OS_16X);
    bme.setIIRFilterSize(BME680_FILTER_SIZE_7);
    bme.setGasHeater(0, 0); // 0*C for 0 ms

    if (!BATTERY_CRITICAL) // Connect to Wi-Fi network with SSID and password if battery is not critical
    {
      setCpuFrequencyMhz(80); // Set CPU to 80MHz for wifi
      delay(2);
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid.c_str(), password.c_str());
      while (WiFi.waitForConnectResult() != WL_CONNECTED)
      {
        if (DEBUG_MODE)
          Serial.println("Connection Failed");
        break;
      }

      if (WiFi.status() == WL_CONNECTED) // if wifi is connected
      {
        if (DEBUG_MODE)
        {
          Serial.println("IP Address: ");
          Serial.println(WiFi.localIP());
        }

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
          Serial.println("Updating time from NTP server");
          if (autoTimeUpdate()) // Update time from NTP server
          {
            if (DEBUG_MODE)
              Serial.println("Time Updated");
            timeNeedsUpdate = false;
          }
          else
          {
            if (DEBUG_MODE)
              Serial.println("Time Update Failed");
          }
          pref.putBool("timeNeedsUpdate", timeNeedsUpdate);
          pref.putUChar("lastCheckedDay", currentDay); // Update last checked day
        }
        else
          Serial.println("Time Update Not Required");

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
          Serial.println("Time And Weather");
        if (bootCount == ghostProtek)
        {
          display.fillScreen(GxEPD_BLACK);
          tempPrint(0, true); // prints temperature and battery level
          weatherPrint(true); // prints weather data
        }
        else // if not ghost protection, then normal display
        {
          display.fillScreen(GxEPD_WHITE);
          tempPrint();    // prints temperature and battery level
          weatherPrint(); // prints weather data
        }
        if (bootCount == ghostProtek) // reset boot counter after ghost protection
          bootCount = 0;
        if (DEBUG_MODE)
          Serial.println("Time And Weather Done");
        // Turn off WiFi as soon as possible
        turnOffWifi();
      }
      else // if wifi is not connected, then only display time
      {
        display.fillScreen(GxEPD_WHITE);
        if (DEBUG_MODE)
          Serial.println("Time Only");
        display.drawBitmap(270, 0, wifiOff, 12, 12, GxEPD_BLACK); // wifi off icon
        tempPrint(40);                                            // offset for wifi off which shifts the temperature display to the middle
        if (DEBUG_MODE)
          Serial.println("Time Done");
      }
    } while (display.nextPage());
    display.hibernate();
    display.powerOff();
  }

  if (DEBUG_MODE)
    Serial.println("Data Write");

  pref.end(); // Close the preferences
  Wire.end(); // End I2C communication

  if (DEBUG_MODE)
  {
    Serial.println("Data Write Done");
    Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP / 60) + " Mins");
    Serial.println("Going to sleep now");
    Serial.flush(); // Flush the serial buffer
    delay(5);
  }

  if (!DEBUG_MODE) // if debug mode is off, then go to deep sleep
  {
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR); // Set the sleep time
    esp_deep_sleep_start();                                        // Enter deep sleep
  }
  else
    Serial.println("Going to loop");
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
    errMsg("DEBUG MODE"); // Display debug message
    // Additional debug functions can be added here
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
  if (httpResponseCode == -1 || httpResponseCode == -11)
  {
    ESP.restart();
    return false;
  }
  else if (httpResponseCode != 200)
  {
    if (DEBUG_MODE)
      Serial.println(String(source) + " API request failed with code: " + String(httpResponseCode));
    networkInfo(source);
    return false;
  }
  return true;
}

/**
 * @brief Prints temperature and environmental data
 * @param offset Vertical offset for display positioning (default: 0)
 * @param invert Inverts colors for ghost protection (default: false)
 */
void tempPrint(byte offset, bool invert)
{
  // Configure fonts and colors once at the start
  uint16_t bg = invert ? GxEPD_BLACK : GxEPD_WHITE;
  uint16_t fg = invert ? GxEPD_WHITE : GxEPD_BLACK;
  uint16_t lineColor = (BATTERY_CRITICAL || invert) ? GxEPD_WHITE : GxEPD_RED;

  u8g2Fonts.setFontMode(1);
  u8g2Fonts.setFontDirection(0);
  u8g2Fonts.setForegroundColor(fg);
  u8g2Fonts.setBackgroundColor(bg);

  // Temperature reading
  float tempC = 0;
  if (sensor.dataReady())
  {
    tempC = sensor.readTempC();
    hTemp = max(hTemp, tempC);
    lTemp = min(lTemp, tempC);
  }

  // Battery level handling
  float newBattLevel = batteryLevel();
  battLevel = (newBattLevel < battLevel) ? newBattLevel : ((newBattLevel - battLevel) >= battChangeThreshold || newBattLevel > battHigh) ? newBattLevel
                                                                                                                                         : battLevel;

  // Battery display section
  u8g2Fonts.setFont(u8g2_font_luRS08_tf);
  u8g2Fonts.setCursor(28, 11);
  u8g2Fonts.print(battLevel, 2);
  u8g2Fonts.print("V");

  int percent = constrain(((battLevel - battLow) / (battHigh - battLow)) * 100, 0, 100);
  BATTERY_CRITICAL = percent < critBattPercent;

  u8g2Fonts.setCursor(63, 11);
  if (!BATTERY_CRITICAL)
  {
    u8g2Fonts.print(percent, 1);
    u8g2Fonts.print("%");
    if (invert)
    {
      u8g2Fonts.setCursor(123, 11);
      u8g2Fonts.print(" GHOSTING PROTECTION");
    }
  }
  else
  {
    u8g2Fonts.print("BATTERY CRITICAL, WIFI TURNED OFF");
  }
  iconBattery(display, percent, invert);

  // Time and date display
  DateTime now = rtc.now();
  char timeStr[6];
  sprintf(timeStr, "%02d:%02d", now.hour(), now.minute());

  u8g2Fonts.setCursor(295, 11);
  u8g2Fonts.print("Last Update: ");
  u8g2Fonts.print(timeStr);

  u8g2Fonts.setFont(u8g2_font_logisoso20_tf);
  u8g2Fonts.setCursor(10, 75 + offset);
  u8g2Fonts.print(now.day() < 10 ? "0" : "");
  u8g2Fonts.print(now.day());
  u8g2Fonts.print(", ");
  u8g2Fonts.print(monthName[now.month() - 1]);
  u8g2Fonts.setCursor(10, 105 + offset);
  u8g2Fonts.print(daysOfTheWeek[now.dayOfTheWeek()]);

  // Main temperature display
  u8g2Fonts.setFont(u8g2_font_inb19_mf);
  u8g2Fonts.setCursor(320, 60 + offset);
  u8g2Fonts.print("o");

  u8g2Fonts.setFont(u8g2_font_logisoso58_tf);
  u8g2Fonts.setCursor(150, 110 + offset);
  u8g2Fonts.print(String(tempC));
  u8g2Fonts.setCursor(330, 110 + offset);
  u8g2Fonts.print("C");

  // Draw separator lines
  for (int i = 0; i < 2; i++)
  {
    display.fillRect(0, 121 + offset + (i * 33), 400, 2, lineColor);
  }

  // Environmental readings
  if (!bme.beginReading() || !bme.endReading())
  {
    if (DEBUG_MODE)
      Serial.println("BME READING ERROR");
    return;
  }

  // Display environmental data
  u8g2Fonts.setFont(u8g2_font_logisoso20_tf);
  u8g2Fonts.setCursor(2, 150 + offset);
  u8g2Fonts.print(bme.humidity);
  u8g2Fonts.print("%");

  u8g2Fonts.setCursor(264, 150 + offset);
  u8g2Fonts.print(bme.pressure / 100.0);
  u8g2Fonts.print("hPa");

  // High/Low temperature display
  u8g2Fonts.setFont(u8g2_font_logisoso16_tf);
  const char *labels[] = {"H:", "L:"};
  float temps[] = {hTemp, lTemp};
  int positions[] = {85, 180};

  for (int i = 0; i < 2; i++)
  {
    u8g2Fonts.setCursor(positions[i], 148 + offset);
    u8g2Fonts.print(labels[i]);
    u8g2Fonts.print(temps[i]);
    u8g2Fonts.setFont(u8g2_font_fub11_tf);
    u8g2Fonts.setCursor(positions[i] + 63, 138 + offset);
    u8g2Fonts.print("o");
    u8g2Fonts.setFont(u8g2_font_logisoso16_tf);
    u8g2Fonts.setCursor(positions[i] + 73, 148 + offset);
    u8g2Fonts.print("C");
  }
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

  char serverPath[256]; // Buffer for API URL
  strcpy(serverPath, OPEN_WEATHER_BASE_URL);
  strcat(serverPath, lat.c_str());
  strcat(serverPath, "&lon=");
  strcat(serverPath, lon.c_str());
  strcat(serverPath, OPEN_WEATHER_PARAMS);
  strcat(serverPath, openWeatherMapApiKey.c_str());

  jsonBuffer = weatherDataAPI(serverPath);
  if (!checkHttpResponse("OpenWeather"))
    return;
  if (DEBUG_MODE)
    Serial.println(jsonBuffer);
  JSONVar myObject = JSON.parse(jsonBuffer);

  // JSON.typeof(jsonVar) can be used to get the type of the var
  if (JSON.typeof(myObject) == "undefined")
  {
    if (DEBUG_MODE)
      Serial.println("Parsing input failed!");
    ESP.restart();
    return;
  }

  // Override with custom weather URL
  strcpy(serverPath, CUSTOM_WEATHER_BASE_URL);
  strcat(serverPath, customApiKey.c_str());
  jsonBuffer = weatherDataAPI(serverPath);
  if (!checkHttpResponse("Custom"))
    return;
  if (DEBUG_MODE)
    Serial.println(jsonBuffer);
  JSONVar customObject = JSON.parse(jsonBuffer);

  // JSON.typeof(jsonVar) can be used to get the type of the var
  if (JSON.typeof(customObject) == "undefined")
  {
    if (DEBUG_MODE)
      Serial.println("Parsing input failed!");
    ESP.restart();
    return;
  }

  if (myObject["current"]["temp"] == null)
    networkInfo();
  else
  {
    wifiStatus(invert);
    u8g2Fonts.setFontMode(1);
    u8g2Fonts.setFontDirection(0);
    u8g2Fonts.setForegroundColor(fg);
    u8g2Fonts.setBackgroundColor(bg);

    u8g2Fonts.setFont(u8g2_font_helvB10_tf);
    u8g2Fonts.setCursor(29, 170);
    u8g2Fonts.print("OUTDOOR");
    u8g2Fonts.setFont(u8g2_font_fub20_tf); // u8g2_font_fub30_tf
    uint16_t width;
    width = u8g2Fonts.getUTF8Width(JSON.stringify(customObject["data"]["temp"]).c_str());
    u8g2Fonts.setCursor(20, 200); // start writing at this position
    u8g2Fonts.print(customObject["data"]["temp"]);
    u8g2Fonts.setCursor(30 + width, 200);
    u8g2Fonts.print("C");
    u8g2Fonts.setFont(u8g2_font_fub11_tf);
    u8g2Fonts.setCursor(22 + width, 185); // start writing at this position
    u8g2Fonts.print("o");

    u8g2Fonts.setFont(u8g2_font_fur11_tf); // u8g2_font_fur14_tf
    width = u8g2Fonts.getUTF8Width(("Real Feel:" + JSON.stringify(myObject["current"]["feels_like"])).c_str());
    u8g2Fonts.setCursor(5, 220); // start writing at this position
    u8g2Fonts.print("Real Feel:");
    u8g2Fonts.setCursor(75, 220);
    u8g2Fonts.print(myObject["current"]["feels_like"]);
    u8g2Fonts.setCursor(width + 16, 220);
    u8g2Fonts.print(String("C"));
    u8g2Fonts.setFont(u8g2_font_baby_tf); // u8g2_font_robot_de_niro_tf
    u8g2Fonts.setCursor(13 + width, 211); // start writing at this position
    u8g2Fonts.print("o");

    u8g2Fonts.setFont(u8g2_font_fur14_tf);
    u8g2Fonts.setCursor(5, 245); // start writing at this position
    u8g2Fonts.print(customObject["data"]["humidity"]);
    u8g2Fonts.print(String("%"));

    u8g2Fonts.setCursor(5, 270); // start writing at this position
    u8g2Fonts.print(customObject["data"]["pressure"]);
    u8g2Fonts.print(String("hPa"));
    u8g2Fonts.setFont(u8g2_font_helvB10_tf);
    u8g2Fonts.setCursor(5, 294); // start writing at this position
    u8g2Fonts.print("UVI: ");
    u8g2Fonts.print(myObject["current"]["uvi"]);
    u8g2Fonts.setFont(u8g2_font_fur11_tf);
    double uv = double(myObject["current"]["uvi"]);
    if (uv < 2)
      u8g2Fonts.print(" Low");
    else if (uv < 5)
      u8g2Fonts.print(" Medium");
    else if (uv <= 7)
      u8g2Fonts.print(" High");
    else if (uv > 7)
      u8g2Fonts.print(" Danger");

    // Draw vertical divider line
    display.fillRect(136, 155, 2, 144, red); // 144 = 299-155

    // Sunset sunrise print
    char timeBuffer[6];
    for (int i = 0; i < 2; i++)
    {
      time_t t = strtoll(JSON.stringify(myObject["current"][i == 0 ? "sunrise" : "sunset"]).c_str(), nullptr, 10);
      if (t > 0)
      {
        setTime(t);
        adjustTime(19800); // UTC+5:30 offset

        // Format time as HH:MM
        snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%02d", hour(), minute());

        // Draw icon and time
        iconSunRise(display, i == 0 ? 152 : 267, 170, i == 0, invert);
        u8g2Fonts.setCursor(i == 0 ? 166 : 281, 175);
        u8g2Fonts.print(timeBuffer);
      }
    }

    // Vertical divider line
    display.fillRect(320, 155, 2, 144, red); // 144 = 299-155
    // Horizontal divider line
    display.fillRect(320, 230, 80, 2, red); // 80 = 400-320

    iconMoonPhase(display, 360, 260, 20, double(myObject["daily"][0]["moon_phase"]), invert);
    u8g2Fonts.setFont(u8g2_font_luRS08_tf);
    u8g2Fonts.setCursor(330, 297);
    u8g2Fonts.print("Moon Phase");

    String s = JSON.stringify(myObject["current"]["weather"][0]["icon"]);
    int lastIndex = s.length() - 1;
    s.remove(lastIndex);
    s.remove(0, 1);

    if (s == "01d")
    { // Clear Day
      iconSun(display, 361, 189, 15, invert);
    }
    else if (s == "01n") // Clear Night
      iconMoon(display, 361, 189, 15, invert);
    else if (s == "02d") // few clouds
      iconCloudyDay(display, 330, 160, 60, invert);
    else if (s == "02n")
      iconCloudyNight(display, 330, 160, 60, invert);
    else if (s == "03d") // scattered clouds
      iconCloud(display, 361, 189, 15, invert);
    else if (s == "03n")
      iconCloud(display, 361, 189, 15, invert);
    else if (s == "04d") // broken clouds (two clouds)
      iconCloudy(display, 330, 160, 60, invert);
    else if (s == "04n")
      iconCloudy(display, 330, 160, 60, invert);
    else if (s == "09d") // shower rain
      iconSleet(display, 330, 160, 60, invert);
    else if (s == "09n")
      iconSleet(display, 330, 160, 60, invert);
    else if (s == "10d") // snow
      iconRain(display, 330, 160, 60, invert);
    else if (s == "10n")
      iconRain(display, 330, 160, 60, invert);
    else if (s == "11d") // thunderstorm
      iconThunderstorm(display, 330, 160, 60, invert);
    else if (s == "11n")
      iconThunderstorm(display, 330, 160, 60, invert);
    else if (s == "13d") // snow
      iconSnow(display, 330, 160, 60, invert);
    else if (s == "13n")
      iconSnow(display, 330, 160, 60, invert);
    else if (s == "50d") // mist
      iconFog(display, 330, 160, 60, invert);
    else if (s == "50n")
      iconFog(display, 330, 160, 60, invert);

    u8g2Fonts.setFont(u8g2_font_luRS08_tf); // u8g2_font_fur11_tf
    s = JSON.stringify(myObject["current"]["weather"][0]["main"]);
    lastIndex = s.length() - 1;
    s.remove(lastIndex);
    s.remove(0, 1);
    u8g2Fonts.setCursor(330, 227);
    u8g2Fonts.print(s);

    // u8g2Fonts.setCursor(186, 200);
    s = JSON.stringify(myObject["alerts"][0]["event"]);
    lastIndex = s.length() - 1;
    s.remove(lastIndex);
    s.remove(0, 1);
    if (s != "ul")
    {
      int16_t tbx, tby;
      uint16_t tbw, tbh;
      display.getTextBounds("Alerts: " + s, 0, 0, &tbx, &tby, &tbw, &tbh);
      // center the bounding box by transposition of the origin:
      uint16_t x = ((display.width() - tbw) / 2) - tbx;
      u8g2Fonts.setCursor(x, 25); // start writing at this position
      u8g2Fonts.print("Alerts: ");
      u8g2Fonts.print(s);
    }
  }
}

// UNCOMMENT BELOW FUNCTION AND REMOVE THE ABOVE FUNCTION WITH THE SAME NAME IF YOU WISH TO USE ONLY oPENwEATHERmAP API
/*
void weatherPrint()
{
  String serverPath = "http://api.openweathermap.org/data/3.0/onecall?lat=" + lat + "&lon=" + lon + "&exclude=hourly,minutely&units=metric&appid=" + openWeatherMapApiKey;

  jsonBuffer = weatherDataAPI(serverPath.c_str());
  if (httpResponseCode == -1 || httpResponseCode == -11)
    ESP.restart();
  if (DEBUG_MODE) Serial.println(jsonBuffer);
  JSONVar myObject = JSON.parse(jsonBuffer);

  // JSON.typeof(jsonVar) can be used to get the type of the var
  if (JSON.typeof(myObject) == "undefined")
  {
    if (DEBUG_MODE) Serial.println("Parsing input failed!");
    ESP.restart();
    return;
  }

  if (myObject["current"]["temp"] == null)
  {
    networkInfo();
  }
  else
  {
    wifiStatus();
    u8g2Fonts.setFont(u8g2_font_helvB10_tf);
    u8g2Fonts.setCursor(29, 170);
    u8g2Fonts.print("OUTDOOR");
    u8g2Fonts.setFont(u8g2_font_fub20_tf); // u8g2_font_fub30_tf
    uint16_t width;
    width = u8g2Fonts.getUTF8Width(JSON.stringify(myObject["current"]["temp"]).c_str());
    u8g2Fonts.setCursor(20, 200); // start writing at this position
    u8g2Fonts.print(myObject["current"]["temp"]);
    u8g2Fonts.setCursor(30 + width, 200);
    u8g2Fonts.print("C");
    u8g2Fonts.setFont(u8g2_font_fub11_tf);
    u8g2Fonts.setCursor(22 + width, 185); // start writing at this position
    u8g2Fonts.print("o");

    u8g2Fonts.setFont(u8g2_font_fur11_tf); // u8g2_font_fur14_tf
    width = u8g2Fonts.getUTF8Width(("Real Feel:" + JSON.stringify(myObject["current"]["feels_like"])).c_str());
    u8g2Fonts.setCursor(5, 220); // start writing at this position
    u8g2Fonts.print("Real Feel:");
    u8g2Fonts.setCursor(75, 220);
    u8g2Fonts.print(myObject["current"]["feels_like"]);
    u8g2Fonts.setCursor(width + 16, 220);
    u8g2Fonts.print(String("C"));
    u8g2Fonts.setFont(u8g2_font_baby_tf); // u8g2_font_robot_de_niro_tf
    u8g2Fonts.setCursor(13 + width, 211); // start writing at this position
    u8g2Fonts.print("o");

    u8g2Fonts.setFont(u8g2_font_fur14_tf);
    u8g2Fonts.setCursor(5, 245); // start writing at this position
    u8g2Fonts.print(myObject["current"]["humidity"]);
    u8g2Fonts.print(String("%"));

    u8g2Fonts.setCursor(5, 270); // start writing at this position
    u8g2Fonts.print(myObject["current"]["pressure"]);
    u8g2Fonts.print(String("hPa"));
    u8g2Fonts.setFont(u8g2_font_helvB10_tf);
    u8g2Fonts.setCursor(5, 294); // start writing at this position
    u8g2Fonts.print("UVI: ");
    u8g2Fonts.print(myObject["current"]["uvi"]);
    u8g2Fonts.setFont(u8g2_font_fur11_tf);
    double uv = double(myObject["current"]["uvi"]);
    if (uv < 2)
      u8g2Fonts.print(" Low");
    else if (uv < 5)
      u8g2Fonts.print(" Medium");
    else if (uv <= 7)
      u8g2Fonts.print(" High");
    else if (uv > 7)
      u8g2Fonts.print(" Danger");
    display.drawLine(136, 155, 136, 299, GxEPD_RED);
    display.drawLine(137, 155, 137, 299, GxEPD_RED);

    // Sunset sunrise print
    time_t t = strtoll(JSON.stringify(myObject["current"]["sunrise"]).c_str(), nullptr, 10);
    setTime(t);
    adjustTime(19800);
    iconSunRise(display, 152, 170, true);
    u8g2Fonts.setCursor(166, 175); // start writing at this position
    u8g2Fonts.print("0");
    u8g2Fonts.print(hour());
    u8g2Fonts.print(":");
    u8g2Fonts.print(minute() < 10 ? "0" + String(minute()) : minute());

    t = strtoll(JSON.stringify(myObject["current"]["sunset"]).c_str(), nullptr, 10);
    setTime(t);
    adjustTime(19800);
    iconSunRise(display, 267, 170, false);
    u8g2Fonts.setCursor(281, 175);
    u8g2Fonts.print(hour());
    u8g2Fonts.print(":");
    u8g2Fonts.print(minute() < 10 ? "0" + String(minute()) : minute());

    display.drawLine(320, 155, 320, 299, GxEPD_RED);
    display.drawLine(321, 155, 321, 299, GxEPD_RED);
    display.drawLine(320, 230, 400, 230, GxEPD_RED);
    display.drawLine(320, 231, 400, 231, GxEPD_RED);

    iconMoonPhase(display, 360, 260, 20, double(myObject["daily"][0]["moon_phase"]));
    u8g2Fonts.setFont(u8g2_font_luRS08_tf);
    u8g2Fonts.setCursor(330, 297);
    u8g2Fonts.print("Moon Phase");

    String s = JSON.stringify(myObject["current"]["weather"][0]["icon"]);
    int lastIndex = s.length() - 1;
    s.remove(lastIndex);
    s.remove(0, 1);

    if (s == "01d")
    { // Clear Day
      iconSun(display, 361, 189, 15);
      // iconSleet(x,y,r);//iconHail(x,y,r);//same
      // iconWind(x,y,r);
      // iconTornado(x,y,r);
    }
    else if (s == "01n") // Clear Night
      iconMoon(display, 361, 189, 15);
    else if (s == "02d") // few clouds
      iconCloudyDay(display, 330, 160, 60);
    else if (s == "02n")
      iconCloudyNight(display, 330, 160, 60);
    else if (s == "03d") // scattered clouds
      iconCloud(display, 361, 189, 15);
    else if (s == "03n")
      iconCloud(display, 361, 189, 15);
    else if (s == "04d") // broken clouds (two clouds)
      iconCloudy(display, 330, 160, 60);
    else if (s == "04n")
      iconCloudy(display, 330, 160, 60);
    else if (s == "09d") // shower rain
      iconSleet(display, 330, 160, 60);
    else if (s == "09n")
      iconSleet(display, 330, 160, 60);
    else if (s == "10d") // snow
      iconRain(display, 330, 160, 60);
    else if (s == "10n")
      iconRain(display, 330, 160, 60);
    else if (s == "11d") // thunderstorm
      iconThunderstorm(display, 330, 160, 60);
    else if (s == "11n")
      iconThunderstorm(display, 330, 160, 60);
    else if (s == "13d") // snow
      iconSnow(display, 330, 160, 60);
    else if (s == "13n")
      iconSnow(display, 330, 160, 60);
    else if (s == "50d") // mist
      iconFog(display, 330, 160, 60);
    else if (s == "50n")
      iconFog(display, 330, 160, 60);

    u8g2Fonts.setFont(u8g2_font_luRS08_tf); // u8g2_font_fur11_tf
    s = JSON.stringify(myObject["current"]["weather"][0]["main"]);
    lastIndex = s.length() - 1;
    s.remove(lastIndex);
    s.remove(0, 1);
    u8g2Fonts.setCursor(330, 227);
    u8g2Fonts.print(s);

    // u8g2Fonts.setCursor(186, 200);
    s = JSON.stringify(myObject["alerts"][0]["event"]);
    lastIndex = s.length() - 1;
    s.remove(lastIndex);
    s.remove(0, 1);
    if (s != "ul")
    {
      int16_t tbx, tby;
      uint16_t tbw, tbh;
      display.getTextBounds("Alerts: " + s, 0, 0, &tbx, &tby, &tbw, &tbh);
      // center the bounding box by transposition of the origin:
      uint16_t x = ((display.width() - tbw) / 2) - tbx;
      u8g2Fonts.setCursor(x, 25); // start writing at this position
      u8g2Fonts.print("Alerts: ");
      u8g2Fonts.print(s);
    }
  }
  // Turn off WiFi as soon as possible after data fetch
  turnOffWifi();
}

//=============== UI HELPER FUNCTIONS ===============

/**
 * @brief Displays network debugging information
 * @note Shows WiFi status, signal strength, and HTTP response codes
 */
void networkInfo(String msg)
{
  display.drawBitmap(270, 0, wifiError, 13, 13, GxEPD_BLACK);
  display.drawBitmap(100, 160, net, 29, 28, GxEPD_BLACK);
  u8g2Fonts.setFont(u8g2_font_logisoso20_tf);
  u8g2Fonts.setCursor(145, 184); // start writing at this position
  u8g2Fonts.print("Network Debug");

  u8g2Fonts.setFont(u8g2_font_logisoso16_tf);
  u8g2Fonts.setCursor(5, 220); // start writing at this position
  u8g2Fonts.print("Connected: ");
  if (WiFi.status() == WL_CONNECTED)
    u8g2Fonts.print("Yes (" + String(WiFi.SSID()) + ")");
  else
    u8g2Fonts.print("No");
  u8g2Fonts.setCursor(5, 245); // start writing at this position
  u8g2Fonts.print("HTTP Code: " + String(httpResponseCode));
  u8g2Fonts.setCursor(5, 270); // start writing at this position
  u8g2Fonts.print("WiFi RSSI: " + String(WiFi.RSSI()));

  if (WiFi.RSSI() > -50)
    u8g2Fonts.print(" Excellent");
  else if (WiFi.RSSI() > -60)
    u8g2Fonts.print(" Good");
  else if (WiFi.RSSI() > -70)
    u8g2Fonts.print(" Fair");
  else
    u8g2Fonts.print(" Poor");
  u8g2Fonts.setCursor(5, 295); // start writing at this position
  u8g2Fonts.print("Comments: " + msg);
}

/**
 * @brief Displays WiFi signal strength indicator
 * @param invert Inverts icon colors for ghost protection
 */
void wifiStatus(bool invert)
{
  if (WiFi.RSSI() >= -60)
    display.drawBitmap(270, 0, wifiOn, 12, 12, invert ? GxEPD_WHITE : GxEPD_BLACK);
  else
    display.drawBitmap(270, 0, wifiAvg, 12, 12, invert ? GxEPD_WHITE : GxEPD_BLACK);
}

/**
 * @brief Prints Alert icon and the passed message all over the screen. Implement a infinite while loop after calling this function
 */
void errMsg(String msg)
{
  display.setRotation(0);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  int16_t tbx, tby;
  uint16_t tbw, tbh;
  display.getTextBounds(msg, 0, 0, &tbx, &tby, &tbw, &tbh);
  // center the bounding box by transposition of the origin:
  uint16_t x = ((display.width() - tbw) / 2) - tbx;
  uint16_t y = ((display.height() - tbh) / 2) - tby;
  display.setFullWindow();
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.drawInvertedBitmap(151, 40, alert, 100, 90, GxEPD_BLACK);
    display.setCursor(x, y);
    display.print(msg);
  } while (display.nextPage());
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
