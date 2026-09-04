#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <EEPROM.h>
#include <Arduino_JSON.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "time.h"
#include <math.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

// ==================================================
// 1. ASSETS & CONFIG
// ==================================================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SDA_PIN D2   // GPIO4
#define SCL_PIN D1   // GPIO5
#define TOUCH_PIN D5 // GPIO14, external TTP223 OUT

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// --- WEATHER ICONS ---
const unsigned char bmp_clear[] PROGMEM = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0xc0, 0x80, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0xff, 0xff, 0x00, 0x06, 0xff, 0xff, 0x60, 0x06, 0xff, 0xff, 0x60, 0x06, 0xff, 0xff, 0x60, 0x00, 0xff, 0xff, 0x00, 0x3e, 0xff, 0xff, 0x7c, 0x3e, 0xff, 0xff, 0x7c, 0x3e, 0xff, 0xff, 0x7c, 0x00, 0xff, 0xff, 0x00, 0x06, 0xff, 0xff, 0x60, 0x06, 0xff, 0xff, 0x60, 0x06, 0xff, 0xff, 0x60, 0x00, 0xff, 0xff, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x01, 0x0f, 0xf0, 0x80, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
const unsigned char bmp_clouds[] PROGMEM = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xe0, 0x00, 0x00, 0x0f, 0xf8, 0x00, 0x00, 0x1f, 0xfc, 0x00, 0x00, 0x3f, 0xfe, 0x00, 0x00, 0x3f, 0xff, 0x00, 0x00, 0x7f, 0xff, 0x80, 0x00, 0xff, 0xff, 0xc0, 0x00, 0xff, 0xff, 0xe0, 0x01, 0xff, 0xff, 0xf0, 0x03, 0xff, 0xff, 0xf8, 0x07, 0xff, 0xff, 0xfc, 0x07, 0xff, 0xff, 0xfc, 0x0f, 0xff, 0xff, 0xfe, 0x0f, 0xff, 0xff, 0xfe, 0x1f, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0x0f, 0xff, 0xff, 0xfe, 0x07, 0xff, 0xff, 0xfc, 0x03, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff, 0xe0, 0x00, 0x3f, 0xff, 0x80, 0x00, 0x0f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
const unsigned char bmp_rain[] PROGMEM = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xe0, 0x00, 0x00, 0x0f, 0xf8, 0x00, 0x00, 0x1f, 0xfc, 0x00, 0x00, 0x3f, 0xfe, 0x00, 0x00, 0x7f, 0xff, 0x80, 0x00, 0xff, 0xff, 0xc0, 0x01, 0xff, 0xff, 0xf0, 0x03, 0xff, 0xff, 0xf8, 0x07, 0xff, 0xff, 0xfc, 0x0f, 0xff, 0xff, 0xfe, 0x1f, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0x0f, 0xff, 0xff, 0xfe, 0x07, 0xff, 0xff, 0xfc, 0x03, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff, 0xe0, 0x00, 0x3f, 0xff, 0x80, 0x00, 0x0f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x0c, 0x00, 0x00, 0x60, 0x0c, 0x00, 0x00, 0xe0, 0x1c, 0x00, 0x00, 0xc0, 0x18, 0x00, 0x03, 0x80, 0x70, 0x00, 0x03, 0x80, 0x70, 0x00, 0x03, 0x00, 0x60, 0x00, 0x02, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
const unsigned char mini_sun[] PROGMEM = { 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x10, 0x08, 0x04, 0x20, 0x03, 0xc0, 0x27, 0xe4, 0x07, 0xe0, 0x07, 0xe0, 0x27, 0xe4, 0x03, 0xc0, 0x04, 0x20, 0x10, 0x08, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00 };
const unsigned char mini_cloud[] PROGMEM = { 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x07, 0xe0, 0x0f, 0xf0, 0x1f, 0xf8, 0x1f, 0xf8, 0x3f, 0xfc, 0x3f, 0xfc, 0x7f, 0xfe, 0x3f, 0xfe, 0x1f, 0xfc, 0x0f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
const unsigned char mini_rain[] PROGMEM = { 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x07, 0xe0, 0x0f, 0xf0, 0x1f, 0xf8, 0x1f, 0xf8, 0x3f, 0xfc, 0x3f, 0xfc, 0x7f, 0xfe, 0x3f, 0xfe, 0x1f, 0xfc, 0x00, 0x00, 0x44, 0x44, 0x22, 0x22, 0x11, 0x11 };
const unsigned char bmp_tiny_drop[] PROGMEM = { 0x10, 0x38, 0x7c, 0xfe, 0xfe, 0x7c, 0x38, 0x00 };

// --- EMOTION PARTICLES (16x16) ---
const unsigned char bmp_heart[] PROGMEM = { 0x00, 0x00, 0x0c, 0x60, 0x1e, 0xf0, 0x3f, 0xf8, 0x7f, 0xfc, 0x7f, 0xfc, 0x7f, 0xfc, 0x3f, 0xf8, 0x1f, 0xf0, 0x0f, 0xe0, 0x07, 0xc0, 0x03, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
const unsigned char bmp_zzz[] PROGMEM = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x0c, 0x00, 0x18, 0x00, 0x30, 0x00, 0x7e, 0x00, 0x00, 0x3c, 0x00, 0x0c, 0x00, 0x18, 0x00, 0x30, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00 };
const unsigned char bmp_anger[] PROGMEM = { 0x00, 0x00, 0x11, 0x10, 0x2a, 0x90, 0x44, 0x40, 0x80, 0x20, 0x80, 0x20, 0x44, 0x40, 0x2a, 0x90, 0x11, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

// --- GLOBALS ---
int currentPage = 0;
bool highBrightness = true;
int tapCounter = 0;
unsigned long lastTapTime = 0;
bool lastPinState = false;
unsigned long pressStartTime = 0;
bool isLongPressHandled = false;
const unsigned long LONG_PRESS_TIME = 800;
const unsigned long DOUBLE_TAP_DELAY = 300;
unsigned long lastPageSwitch = 0;
const unsigned long PAGE_INTERVAL = 8000;

// MOODS
#define MOOD_NORMAL 0
#define MOOD_HAPPY 1
#define MOOD_SURPRISED 2
#define MOOD_SLEEPY 3
#define MOOD_ANGRY 4
#define MOOD_SAD 5
#define MOOD_EXCITED 6
#define MOOD_LOVE 7
#define MOOD_SUSPICIOUS 8
int currentMood = MOOD_NORMAL;

String city;       // Loaded from Preferences
String countryCode; // Loaded from Preferences
String apiKey;   // Loaded from Preferences or defaults
String wifiSsid; // Loaded from Preferences
String wifiPass; // Loaded from Preferences
unsigned long lastWeatherUpdate = 0;
float temperature = 0.0;
float feelsLike = 0.0;
int humidity = 0;
String weatherMain = "Loading";
String weatherDesc = "Wait...";

struct ForecastDay {
  String dayName;
  int temp;
  String iconType;
};
ForecastDay fcast[3];
const char* ntpServer = "pool.ntp.org";
String tzString;   // Loaded from Preferences (e.g. "IST-5:30")

// ==================================================
// 2. ULTRA PRO PHYSICS ENGINE
// ==================================================

struct Eye {
  float x, y;  // Current position (Top-Left)
  float w, h;  // Current size
  float targetX, targetY, targetW, targetH;

  // Pupil Physics (Secondary Motion)
  float pupilX, pupilY;
  float targetPupilX, targetPupilY;

  // Physics constants
  float velX, velY, velW, velH;
  float pVelX, pVelY;
  float k = 0.12;   // Eye Spring
  float d = 0.60;   // Eye Damping (Heavier feel)
  float pk = 0.08;  // Pupil Spring (Softer/Laggier)
  float pd = 0.50;  // Pupil Damping

  bool blinking;
  unsigned long lastBlink;
  unsigned long nextBlinkTime;

  void init(float _x, float _y, float _w, float _h) {
    x = targetX = _x;
    y = targetY = _y;
    w = targetW = _w;
    h = targetH = _h;
    pupilX = targetPupilX = 0;
    pupilY = targetPupilY = 0;
    nextBlinkTime = millis() + random(1000, 4000);
  }

  void update() {
    // 1. Main Eye Physics
    float ax = (targetX - x) * k;
    float ay = (targetY - y) * k;
    float aw = (targetW - w) * k;
    float ah = (targetH - h) * k;

    velX = (velX + ax) * d;
    velY = (velY + ay) * d;
    velW = (velW + aw) * d;
    velH = (velH + ah) * d;

    x += velX;
    y += velY;
    w += velW;
    h += velH;

    // 2. Pupil Physics (Dragging behind)
    float pax = (targetPupilX - pupilX) * pk;
    float pay = (targetPupilY - pupilY) * pk;
    pVelX = (pVelX + pax) * pd;
    pVelY = (pVelY + pay) * pd;
    pupilX += pVelX;
    pupilY += pVelY;
  }
};

Eye leftEye, rightEye;
unsigned long lastSaccade = 0;
unsigned long saccadeInterval = 3000;
float breathVal = 0;

// ==================================================
// 2b. CONFIG PORTAL (WiFi + API Key via local web)
// ==================================================
#define CONFIG_AP_SSID   "DeskBuddy-Setup"
#define CONFIG_AP_PASS   "12345678"
#define CONFIG_HOLD_MS   3000

// EEPROM storage for ESP8266
#define EEPROM_SIZE 1024
#define CONFIG_MAGIC 0x44425544UL

struct SavedConfig {
  uint32_t magic;
  char ssid[33];
  char pass[65];
  char apikey[80];
  char city[48];
  char country[8];
  char tz[40];
};

SavedConfig savedConfig;
ESP8266WebServer configServer(80);
bool inConfigMode = false;

void copyToField(char* dest, size_t destSize, const String& value) {
  memset(dest, 0, destSize);
  value.substring(0, destSize - 1).toCharArray(dest, destSize);
}

void readStoredConfig() {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.get(0, savedConfig);
}

void writeStoredConfig() {
  savedConfig.magic = CONFIG_MAGIC;
  EEPROM.put(0, savedConfig);
  EEPROM.commit();
}

void loadConfig() {
  readStoredConfig();

  if (savedConfig.magic == CONFIG_MAGIC) {
    wifiSsid    = String(savedConfig.ssid);
    wifiPass    = String(savedConfig.pass);
    apiKey      = String(savedConfig.apikey);
    city        = String(savedConfig.city);
    countryCode = String(savedConfig.country);
    tzString    = String(savedConfig.tz);
  } else {
    wifiSsid = "";
    wifiPass = "";
    apiKey = "";
    city = "";
    countryCode = "";
    tzString = "";
  }

  // Defaults for non-WiFi settings. WiFi remains empty on a fresh device,
  // so the configuration AP opens automatically.
  if (apiKey.isEmpty()) apiKey = "45fcf5807a5920e2006c2b8a077d423f";
  if (city.isEmpty()) city = "Surat";
  if (countryCode.isEmpty()) countryCode = "IN";
  if (tzString.isEmpty()) tzString = "IST-5:30";
}

void saveConfig(const String& s, const String& p, const String& ak,
                const String& cty, const String& ctry, const String& tz) {
  memset(&savedConfig, 0, sizeof(savedConfig));
  copyToField(savedConfig.ssid, sizeof(savedConfig.ssid), s);
  copyToField(savedConfig.pass, sizeof(savedConfig.pass), p);
  copyToField(savedConfig.apikey, sizeof(savedConfig.apikey), ak);
  copyToField(savedConfig.city, sizeof(savedConfig.city), cty);
  copyToField(savedConfig.country, sizeof(savedConfig.country), ctry);
  copyToField(savedConfig.tz, sizeof(savedConfig.tz), tz);
  writeStoredConfig();
}

void handleConfigRoot() {
  readStoredConfig();
  String sSsid = (savedConfig.magic == CONFIG_MAGIC) ? String(savedConfig.ssid) : "";
  String sApik = (savedConfig.magic == CONFIG_MAGIC && strlen(savedConfig.apikey)) ? String(savedConfig.apikey) : apiKey;
  String sCity = (savedConfig.magic == CONFIG_MAGIC && strlen(savedConfig.city)) ? String(savedConfig.city) : city;
  String sCtry = (savedConfig.magic == CONFIG_MAGIC && strlen(savedConfig.country)) ? String(savedConfig.country) : countryCode;
  String sTz   = (savedConfig.magic == CONFIG_MAGIC && strlen(savedConfig.tz)) ? String(savedConfig.tz) : tzString;

  String html = R"rawliteral(
<!DOCTYPE html><html><head><meta name="viewport" content="width=device-width,initial-scale=1">
<title>DeskBuddy Config</title>
<style>
body{font-family:sans-serif;max-width:420px;margin:30px auto;padding:24px;background:#0c1929;color:#e8f4fc;}
h1{color:#5ba3f5;margin-bottom:8px;}
input{width:100%;padding:10px;margin:6px 0;border:1px solid #2d4a6f;border-radius:6px;box-sizing:border-box;background:#1a2d47;color:#e8f4fc;}
input:focus{outline:none;border-color:#5ba3f5;}
button{width:100%;padding:12px;background:#3498db;color:#fff;border:none;border-radius:6px;font-size:16px;cursor:pointer;margin-top:16px;}
label{display:block;margin-top:14px;color:#8ab4e8;font-size:14px;}
.section{margin-top:20px;padding-top:16px;border-top:1px solid #1e3a5f;}
.section-title{color:#5ba3f5;font-size:13px;margin-bottom:8px;}
</style></head><body>
<h1>DeskBuddy Setup</h1>
<form action="/save" method="POST">
<label>WiFi SSID</label><input name="ssid" placeholder="Your WiFi name" value=")rawliteral";
  html += sSsid;
  html += R"rawliteral(">
<label>WiFi Password</label><input name="pass" type="password" placeholder="WiFi password">
<div class="section"><div class="section-title">Weather (OpenWeatherMap)</div>
<label>API Key</label><input name="apikey" placeholder="API key" value=")rawliteral";
  html += sApik;
  html += R"rawliteral(">
<label>City</label><input name="city" placeholder="e.g. London" value=")rawliteral";
  html += sCity;
  html += R"rawliteral(">
<label>Country Code</label><input name="country" placeholder="e.g. IN, US, GB" value=")rawliteral";
  html += sCtry;
  html += R"rawliteral(">
</div>
<div class="section"><div class="section-title">Time</div>
<label>Timezone</label><input name="tz" placeholder="e.g. IST-5:30, EST5EDT" value=")rawliteral";
  html += sTz;
  html += R"rawliteral(">
</div>
<button type="submit">Save &amp; Reboot</button>
</form></body></html>)rawliteral";
  configServer.send(200, "text/html", html);
}

void handleConfigSave() {
  if (!configServer.hasArg("ssid") || configServer.arg("ssid").length() == 0) {
    configServer.send(400, "text/plain", "SSID required");
    return;
  }

  String s   = configServer.arg("ssid");
  String p   = configServer.arg("pass");
  String ak  = configServer.arg("apikey");
  String cty = configServer.arg("city");
  String ctr = configServer.arg("country");
  String tz  = configServer.arg("tz");
  s.trim(); p.trim(); ak.trim(); cty.trim(); ctr.trim(); tz.trim();

  readStoredConfig();
  if (p.isEmpty() && savedConfig.magic == CONFIG_MAGIC) p = String(savedConfig.pass);
  if (ak.isEmpty())  ak  = (savedConfig.magic == CONFIG_MAGIC && strlen(savedConfig.apikey)) ? String(savedConfig.apikey) : apiKey;
  if (cty.isEmpty()) cty = (savedConfig.magic == CONFIG_MAGIC && strlen(savedConfig.city)) ? String(savedConfig.city) : city;
  if (ctr.isEmpty()) ctr = (savedConfig.magic == CONFIG_MAGIC && strlen(savedConfig.country)) ? String(savedConfig.country) : countryCode;
  if (tz.isEmpty())  tz  = (savedConfig.magic == CONFIG_MAGIC && strlen(savedConfig.tz)) ? String(savedConfig.tz) : tzString;

  saveConfig(s, p, ak, cty, ctr, tz);

  // Verify EEPROM write before rebooting
  readStoredConfig();
  bool saveOK = (savedConfig.magic == CONFIG_MAGIC &&
                 String(savedConfig.ssid) == s);

  Serial.println();
  Serial.println("CONFIG SAVE REQUEST");
  Serial.print("Save verification: ");
  Serial.println(saveOK ? "OK" : "FAILED");
  Serial.print("SSID saved: [");
  Serial.print(savedConfig.ssid);
  Serial.println("]");
  Serial.print("Password length saved: ");
  Serial.println(strlen(savedConfig.pass));

  if (!saveOK) {
    configServer.send(500, "text/plain", "Failed to save configuration");
    return;
  }

  configServer.send(200, "text/html",
    "<!DOCTYPE html><html><body style='font-family:sans-serif;background:#0c1929;color:#e8f4fc;padding:40px;'>"
    "<h2 style='color:#5ba3f5'>Saved!</h2>"
    "<p>Device is restarting and will connect to your WiFi.</p>"
    "<p>You can close this page.</p></body></html>");

  delay(1000);
  yield();
  ESP.restart();
}

void startConfigPortal() {
  inConfigMode = true;

  WiFi.persistent(false);
  WiFi.disconnect();
  delay(200);
  WiFi.mode(WIFI_AP);
  delay(200);

  IPAddress apIP(192, 168, 4, 1);
  IPAddress gateway(192, 168, 4, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.softAPConfig(apIP, gateway, subnet);

  bool ok = WiFi.softAP(CONFIG_AP_SSID, CONFIG_AP_PASS, 6, false, 4);

  configServer.on("/", handleConfigRoot);
  configServer.on("/save", HTTP_POST, handleConfigSave);
  configServer.begin();

  display.clearDisplay();
  display.setFont(NULL);
  display.setCursor(0, 0);
  display.print(ok ? "Config mode\n\nConnect to:\n" : "AP START FAILED\n\nRestart device");
  if (ok) {
    display.print(CONFIG_AP_SSID);
    display.print("\n\nThen open:\n192.168.4.1");
  }
  display.display();
  // display.invertDisplay(true);
  Serial.println();
  Serial.println("CONFIG PORTAL");
  Serial.print("AP started: "); Serial.println(ok ? "YES" : "NO");
  Serial.print("SSID: "); Serial.println(CONFIG_AP_SSID);
  Serial.print("IP: "); Serial.println(WiFi.softAPIP());
}

// ==================================================
// 3. LOGIC & NETWORK
// ==================================================
const unsigned char* getBigIcon(String w) {
  if (w == "Clear") return bmp_clear;
  if (w == "Clouds") return bmp_clouds;
  if (w == "Rain" || w == "Drizzle") return bmp_rain;
  return bmp_clouds;
}
const unsigned char* getMiniIcon(String w) {
  if (w == "Clear") return mini_sun;
  if (w == "Rain" || w == "Drizzle" || w == "Thunderstorm") return mini_rain;
  return mini_cloud;
}

void updateMoodBasedOnWeather() {
  int m = MOOD_NORMAL;
  if (weatherMain == "Clear") m = MOOD_HAPPY;
  else if (weatherMain == "Rain" || weatherMain == "Drizzle") m = MOOD_SAD;
  else if (weatherMain == "Thunderstorm") m = MOOD_SURPRISED;
  else if (weatherMain == "Clouds") m = MOOD_NORMAL;
  else if (temperature > 25) m = MOOD_EXCITED;
  else if (temperature < 5) m = MOOD_SLEEPY;
  currentMood = m;
}

void handleTouch() {
  bool currentPinState = digitalRead(TOUCH_PIN);
  unsigned long now = millis();
  if (currentPinState && !lastPinState) {
    pressStartTime = now;
    isLongPressHandled = false;
  } else if (currentPinState && lastPinState) {
    if ((now - pressStartTime > LONG_PRESS_TIME) && !isLongPressHandled) {
      lastPageSwitch = now;
      if (currentPage == 0) {
        currentMood++;
        if (currentMood > MOOD_SUSPICIOUS) currentMood = 0;
        lastSaccade = 0;  // Trigger move
      } else if (currentPage == 1) currentPage = 3;
      else if (currentPage == 2) currentPage = 4;
      isLongPressHandled = true;
    }
  } else if (!currentPinState && lastPinState) {
    if ((now - pressStartTime < LONG_PRESS_TIME) && !isLongPressHandled) {
      tapCounter++;
      lastTapTime = now;
    }
  }
  lastPinState = currentPinState;
  if (tapCounter > 0) {
    if (now - lastTapTime > DOUBLE_TAP_DELAY) {
      lastPageSwitch = now;
      if (tapCounter == 2) {
        highBrightness = !highBrightness;
        display.ssd1306_command(0x81); display.ssd1306_command(highBrightness ? 255 : 1);
        display.display();
      } else if (tapCounter == 1) {
        if (currentPage == 3) currentPage = 1;
        else if (currentPage == 4) currentPage = 2;
        else {
          currentPage++;
          if (currentPage > 2) currentPage = 0;
        }
      }
      tapCounter = 0;
    }
  }
}

void getWeatherAndForecast() {
  if (WiFi.status() != WL_CONNECTED) return;
  HTTPClient http;
  WiFiClient httpClient;
  String url = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&appid=" + apiKey + "&units=metric";
  http.begin(httpClient, url);
  if (http.GET() == 200) {
    String payload = http.getString();
    JSONVar myObject = JSON.parse(payload);
    if (JSON.typeof(myObject) != "undefined") {
      temperature = double(myObject["main"]["temp"]);
      feelsLike = double(myObject["main"]["feels_like"]);
      humidity = int(myObject["main"]["humidity"]);
      weatherMain = (const char*)myObject["weather"][0]["main"];
      weatherDesc = (const char*)myObject["weather"][0]["description"];
      weatherDesc[0] = toupper(weatherDesc[0]);
      updateMoodBasedOnWeather();
    }
  }
  http.end();
  url = "http://api.openweathermap.org/data/2.5/forecast?q=" + city + "," + countryCode + "&appid=" + apiKey + "&units=metric";
  http.begin(httpClient, url);
  if (http.GET() == 200) {
    String payload = http.getString();
    JSONVar fo = JSON.parse(payload);
    if (JSON.typeof(fo) != "undefined") {
      struct tm t;
      getLocalTime(&t);
      int today = t.tm_wday;
      const char* days[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
      int indices[3] = { 7, 15, 23 };
      for (int i = 0; i < 3; i++) {
        int idx = indices[i];
        fcast[i].temp = (int)double(fo["list"][idx]["main"]["temp"]);
        fcast[i].iconType = (const char*)fo["list"][idx]["weather"][0]["main"];
        int nextDayIndex = (today + i + 1) % 7;
        fcast[i].dayName = days[nextDayIndex];
      }
    }
  }
  http.end();
}

// ==================================================
// 4. DRAWING & ANIMATION
// ==================================================

void drawEyelidMask(float x, float y, float w, float h, int mood, bool isLeft) {
  int ix = (int)x;
  int iy = (int)y;
  int iw = (int)w;
  int ih = (int)h;
  display.setTextColor(SSD1306_BLACK);

  // ANGRY: Sharp slanted cut
  if (mood == MOOD_ANGRY) {
    if (isLeft)
      for (int i = 0; i < 16; i++) display.drawLine(ix, iy + i, ix + iw, iy - 6 + i, SSD1306_BLACK);
    else
      for (int i = 0; i < 16; i++) display.drawLine(ix, iy - 6 + i, ix + iw, iy + i, SSD1306_BLACK);
  }
  // SAD: Inverse slanted cut
  else if (mood == MOOD_SAD) {
    if (isLeft)
      for (int i = 0; i < 16; i++) display.drawLine(ix, iy - 6 + i, ix + iw, iy + i, SSD1306_BLACK);
    else
      for (int i = 0; i < 16; i++) display.drawLine(ix, iy + i, ix + iw, iy - 6 + i, SSD1306_BLACK);
  }
  // HAPPY/LOVE: Cheek push up
  else if (mood == MOOD_HAPPY || mood == MOOD_LOVE || mood == MOOD_EXCITED) {
    display.fillRect(ix, iy + ih - 12, iw, 14, SSD1306_BLACK);
    display.fillCircle(ix + iw / 2, iy + ih + 6, iw / 1.3, SSD1306_BLACK);  // Round cut
  }
  // SLEEPY: Heavy lids
  else if (mood == MOOD_SLEEPY) {
    display.fillRect(ix, iy, iw, ih / 2 + 2, SSD1306_BLACK);
  }
  // SUSPICIOUS: One eye squint, one open
  else if (mood == MOOD_SUSPICIOUS) {
    if (isLeft) display.fillRect(ix, iy, iw, ih / 2 - 2, SSD1306_BLACK);
    else display.fillRect(ix, iy + ih - 8, iw, 8, SSD1306_BLACK);
  }
}

void drawUltraProEye(Eye& e, bool isLeft) {
  int ix = (int)e.x;
  int iy = (int)e.y;
  int iw = (int)e.w;
  int ih = (int)e.h;

  // 1. Draw Sclera (White base)
  int r = 8;
  if (iw < 20) r = 3;
  display.fillRoundRect(ix, iy, iw, ih, r, SSD1306_WHITE);

  // 2. Draw Pupil (Black inner eye) - Constrained to be inside
  // Calculate center of eye
  int cx = ix + iw / 2;
  int cy = iy + ih / 2;

  // Pupil size is proportional to eye size
  int pw = iw / 2.2;
  int ph = ih / 2.2;

  // Apply pupil offset (Gaze)
  int px = cx + (int)e.pupilX - (pw / 2);
  int py = cy + (int)e.pupilY - (ph / 2);

  // Clamp pupil to ensure it doesn't leave the eye completely
  if (px < ix) px = ix;
  if (px + pw > ix + iw) px = ix + iw - pw;
  if (py < iy) py = iy;
  if (py + ph > iy + ih) py = iy + ih - ph;

  display.fillRoundRect(px, py, pw, ph, r / 2, SSD1306_BLACK);

  // 3. Draw Specular Highlight (The "Glint" of life)
  // A tiny white dot in the top-right of the pupil
  if (iw > 15 && ih > 15) {
    display.fillCircle(px + pw - 4, py + 4, 2, SSD1306_WHITE);
  }

  // 4. Apply Eyelid Masks (Expressions)
  drawEyelidMask(e.x, e.y, e.w, e.h, currentMood, isLeft);
}

void updatePhysicsAndMood() {
  unsigned long now = millis();
  breathVal = sin(now / 800.0) * 1.5;  // Breathing effect

  // --- BLINK LOGIC ---
  if (now > leftEye.nextBlinkTime) {
    leftEye.blinking = true;
    leftEye.lastBlink = now;
    rightEye.blinking = true;
    leftEye.nextBlinkTime = now + random(2000, 6000);
  }
  if (leftEye.blinking) {
    leftEye.targetH = 2;
    rightEye.targetH = 2;  // Close
    if (now - leftEye.lastBlink > 120) {
      leftEye.blinking = false;
      rightEye.blinking = false;
    }
  }

  // --- SACCADE (Gaze) LOGIC ---
  if (!leftEye.blinking && now - lastSaccade > saccadeInterval) {
    lastSaccade = now;
    saccadeInterval = random(500, 3000);

    // Pick direction
    int dir = random(0, 10);
    float lx = 0, ly = 0;

    if (dir < 4) {
      lx = 0;
      ly = 0;
    }  // Center
    else if (dir == 4) {
      lx = -6;
      ly = -4;
    }  // TL
    else if (dir == 5) {
      lx = 6;
      ly = -4;
    }  // TR
    else if (dir == 6) {
      lx = -6;
      ly = 4;
    }  // BL
    else if (dir == 7) {
      lx = 6;
      ly = 4;
    }  // BR
    else if (dir == 8) {
      lx = 8;
      ly = 0;
    }  // R
    else if (dir == 9) {
      lx = -8;
      ly = 0;
    }  // L

    // Move pupil target relative to center
    leftEye.targetPupilX = lx;
    leftEye.targetPupilY = ly;
    rightEye.targetPupilX = lx;
    rightEye.targetPupilY = ly;

    // Move eye container slightly (Head follow)
    leftEye.targetX = 18 + (lx * 0.3);
    leftEye.targetY = 14 + (ly * 0.3);
    rightEye.targetX = 74 + (lx * 0.3);
    rightEye.targetY = 14 + (ly * 0.3);
  }

  // --- MOOD SHAPES (Overrides targets) ---
  if (!leftEye.blinking) {
    float baseW = 36;
    float baseH = 36;

    // Breathing effect applied to height
    baseH += breathVal;

    switch (currentMood) {
      case MOOD_NORMAL:
        leftEye.targetW = baseW;
        leftEye.targetH = baseH;
        rightEye.targetW = baseW;
        rightEye.targetH = baseH;
        break;
      case MOOD_HAPPY:
      case MOOD_LOVE:
        leftEye.targetW = 40;
        leftEye.targetH = 32;
        rightEye.targetW = 40;
        rightEye.targetH = 32;
        break;
      case MOOD_SURPRISED:
        leftEye.targetW = 30;
        leftEye.targetH = 45;
        rightEye.targetW = 30;
        rightEye.targetH = 45;
        // Jitter pupil
        leftEye.targetPupilX += random(-1, 2);
        break;
      case MOOD_SLEEPY:
        leftEye.targetW = 38;
        leftEye.targetH = 30;
        rightEye.targetW = 38;
        rightEye.targetH = 30;
        break;
      case MOOD_ANGRY:
        leftEye.targetW = 34;
        leftEye.targetH = 32;
        rightEye.targetW = 34;
        rightEye.targetH = 32;
        break;
      case MOOD_SAD:
        leftEye.targetW = 34;
        leftEye.targetH = 40;
        rightEye.targetW = 34;
        rightEye.targetH = 40;
        break;
      case MOOD_SUSPICIOUS:
        leftEye.targetW = 36;
        leftEye.targetH = 20;  // Left Squint
        rightEye.targetW = 36;
        rightEye.targetH = 42;  // Right Wide
        break;
    }
  }

  leftEye.update();
  rightEye.update();
}

void drawEmoPage() {
  updatePhysicsAndMood();

  // Draw Floating Particles based on Mood
  if (currentMood == MOOD_LOVE) {
    display.drawBitmap(56, 0, bmp_heart, 16, 16, SSD1306_WHITE);
  } else if (currentMood == MOOD_SLEEPY) {
    display.drawBitmap(110, 0, bmp_zzz, 16, 16, SSD1306_WHITE);
  } else if (currentMood == MOOD_ANGRY) {
    display.drawBitmap(56, 0, bmp_anger, 16, 16, SSD1306_WHITE);
  }

  drawUltraProEye(leftEye, true);
  drawUltraProEye(rightEye, false);
}

// --- STANDARD PAGES ---
void drawForecastPage() {
  display.fillRect(0, 0, 128, 16, SSD1306_WHITE);
  display.setFont(NULL);
  display.setTextColor(SSD1306_BLACK);
  display.setCursor(20, 4);
  display.print("3-DAY FORECAST");
  display.setTextColor(SSD1306_WHITE);
  display.drawLine(42, 16, 42, 64, SSD1306_WHITE);
  display.drawLine(85, 16, 85, 64, SSD1306_WHITE);
  for (int i = 0; i < 3; i++) {
    int xStart = i * 43;
    int centerX = xStart + 21;
    display.setFont(NULL);
    String d = fcast[i].dayName;
    if (d == "") d = "Wait";
    display.setCursor(centerX - (d.length() * 3), 20);
    display.print(d);
    display.drawBitmap(centerX - 8, 28, getMiniIcon(fcast[i].iconType), 16, 16, SSD1306_WHITE);
    display.setFont(&FreeSansBold9pt7b);
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(String(fcast[i].temp).c_str(), 0, 0, &x1, &y1, &w, &h);
    display.setCursor(centerX - (w / 2) - 2, 60);
    display.print(fcast[i].temp);
    display.fillCircle(centerX + (w / 2) + 1, 52, 2, SSD1306_WHITE);
  }
}
void drawClock() {
  struct tm t;
  if (!getLocalTime(&t)) {
    display.setFont(NULL);
    display.setCursor(30, 30);
    display.print("Syncing...");
    return;
  }
  String ampm = (t.tm_hour >= 12) ? "PM" : "AM";
  int h12 = t.tm_hour % 12;
  if (h12 == 0) h12 = 12;
  display.setTextColor(SSD1306_WHITE);
  display.setFont(NULL);
  display.setTextSize(1);
  display.setCursor(114, 0);
  display.print(ampm);
  display.setFont(&FreeSansBold18pt7b);
  char timeStr[6];
  sprintf(timeStr, "%02d:%02d", h12, t.tm_min);
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(timeStr, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_WIDTH - w) / 2, 42);
  display.print(timeStr);
  display.setFont(&FreeSans9pt7b);
  char dateStr[20];
  strftime(dateStr, 20, "%a, %b %d", &t);
  display.getTextBounds(dateStr, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_WIDTH - w) / 2, 62);
  display.print(dateStr);
}
void drawWeatherCard() {
  if (WiFi.status() != WL_CONNECTED) {
    display.setFont(NULL);
    display.setCursor(0, 0);
    display.print("No WiFi");
    return;
  }
  display.drawBitmap(96, 0, getBigIcon(weatherMain), 32, 32, SSD1306_WHITE);
  display.setFont(&FreeSansBold9pt7b);
  String c = city;
  c.toUpperCase();
  display.setCursor(0, 14);
  if (c.length() > 9) c = c.substring(0, 8) + ".";
  display.print(c);
  display.setFont(&FreeSansBold18pt7b);
  int tempInt = (int)temperature;
  display.setCursor(0, 48);
  display.print(tempInt);
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(String(tempInt).c_str(), 0, 48, &x1, &y1, &w, &h);
  display.fillCircle(x1 + w + 5, 26, 4, SSD1306_WHITE);
  display.setFont(NULL);
  display.drawBitmap(88, 32, bmp_tiny_drop, 8, 8, SSD1306_WHITE);
  display.setCursor(100, 32);
  display.print(humidity);
  display.print("%");
  display.setCursor(88, 45);
  display.print("~");
  display.print((int)feelsLike);
  display.drawLine(0, 52, 128, 52, SSD1306_WHITE);
  display.setCursor(0, 55);
  display.print(weatherDesc);
}
void drawWorldClock() {
  time_t now;
  time(&now);
  time_t indiaEpoch = now + (5 * 3600) + (30 * 60);
  time_t sydneyEpoch = now + (11 * 3600);
  struct tm* indiatm = gmtime(&indiaEpoch);
  int i_h = indiatm->tm_hour;
  int i_m = indiatm->tm_min;
  struct tm* sydneytm = gmtime(&sydneyEpoch);
  int s_h = sydneytm->tm_hour;
  int s_m = sydneytm->tm_min;
  display.fillRect(0, 0, 128, 16, SSD1306_WHITE);
  display.setFont(NULL);
  display.setTextColor(SSD1306_BLACK);
  display.setCursor(32, 4);
  display.print("WORLD CLOCK");
  display.setTextColor(SSD1306_WHITE);
  display.drawLine(64, 18, 64, 54, SSD1306_WHITE);
  display.setFont(NULL);
  display.setCursor(16, 22);
  display.print("INDIA");
  display.setFont(&FreeSansBold9pt7b);
  char iStr[10];
  sprintf(iStr, "%02d:%02d", i_h, i_m);
  display.setCursor(5, 46);
  display.print(iStr);
  display.setFont(NULL);
  display.setCursor(78, 22);
  display.print("SYDNEY");
  display.setFont(&FreeSansBold9pt7b);
  char sStr[10];
  sprintf(sStr, "%02d:%02d", s_h, s_m);
  display.setCursor(69, 46);
  display.print(sStr);
  display.setFont(NULL);
  display.setCursor(35, 56);
  display.print("Tap to Exit");
}

// ==================================================
// 5. BOOT & MAIN
// ==================================================

void playBootAnimation() {
  display.setTextColor(SSD1306_WHITE);
  int cx = 64;
  int cy = 32;
  // Expanding Circle Wipe
  for (int r = 0; r < 80; r += 4) {
    display.clearDisplay();
    display.fillCircle(cx, cy, r, SSD1306_WHITE);
    display.display();
    delay(10);
  }
  // Inverse Wipe
  for (int r = 0; r < 80; r += 4) {
    display.clearDisplay();
    display.fillCircle(cx, cy, 80, SSD1306_WHITE);
    display.fillCircle(cx, cy, r, SSD1306_BLACK);
    display.display();
    delay(10);
  }

  // Text Display - "PR-TECH"
  display.setFont(&FreeSansBold9pt7b);
  String bootText = "PyLin 3D";

  // Calculate width to center it perfectly
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(bootText, 0, 0, &x1, &y1, &w, &h);

  display.clearDisplay();
  display.setCursor((SCREEN_WIDTH - w) / 2, 36);
  display.print(bootText);
  display.display();
  delay(2000);
}

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println();
  Serial.println("DESKBUDDY ESP8266 BOOT");

  Wire.begin(SDA_PIN, SCL_PIN);
  pinMode(TOUCH_PIN, INPUT);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(SSD1306_WHITE);

  // --------------------------------------------------
  // LOAD SAVED CONFIGURATION
  // --------------------------------------------------
  loadConfig();

  Serial.print("Saved SSID: [");
  Serial.print(wifiSsid);
  Serial.println("]");
  Serial.print("Saved password length: ");
  Serial.println(wifiPass.length());

  // Touch is NOT used for WiFi or configuration mode.
  // If credentials are saved, always continue to WiFi connection.
  // Config portal starts only when no WiFi is saved or connection fails.
  if (wifiSsid.length() == 0) {
    Serial.println("No saved WiFi configuration");
    startConfigPortal();
    return;
  }

  // Init Eyes Center
  leftEye.init(18, 14, 36, 36);
  rightEye.init(74, 14, 36, 36);

  playBootAnimation();

  display.clearDisplay();
  display.setFont(NULL);
  display.setCursor(40, 30);
  display.print("connecting");
  display.display();
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  delay(300);

  Serial.print("Connecting to SSID: [");
  Serial.print(wifiSsid);
  Serial.println("]");
  Serial.print("Password length: ");
  Serial.println(wifiPass.length());

  WiFi.disconnect();
  delay(200);
  WiFi.begin(wifiSsid.c_str(), wifiPass.c_str());

  unsigned long wifiStart = millis();
  wl_status_t lastStatus = WiFi.status();

  while (WiFi.status() != WL_CONNECTED && (millis() - wifiStart < 30000)) {
    wl_status_t currentStatus = WiFi.status();

    if (currentStatus != lastStatus) {
      Serial.print("WiFi status changed to: ");
      Serial.println((int)currentStatus);
      lastStatus = currentStatus;
    }

    Serial.print(".");
    delay(500);
    yield();
  }
  Serial.println();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("WiFi failed. Final status: ");
    Serial.println((int)WiFi.status());

    Serial.println("Opening config portal...");
    startConfigPortal();
    return;
  }

  Serial.println("WiFi connected");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  configTime(0, 0, ntpServer);
  setenv("TZ", tzString.c_str(), 1);
  tzset();
  getWeatherAndForecast();
  lastWeatherUpdate = millis();
  lastPageSwitch = millis();
}

void loop() {
  if (inConfigMode) {
    configServer.handleClient();
    return;
  }
  unsigned long now = millis();
  handleTouch();
  if (now - lastWeatherUpdate > 600000) {
    getWeatherAndForecast();
    lastWeatherUpdate = now;
  }

  if (currentPage < 3 && now - lastPageSwitch > PAGE_INTERVAL) {
    currentPage++;
    if (currentPage > 2) currentPage = 0;
    lastPageSwitch = now;
    lastSaccade = 0;
  }

  display.clearDisplay();
  switch (currentPage) {
    case 0: drawEmoPage(); break;
    case 1: drawClock(); break;
    case 2: drawWeatherCard(); break;
    case 3: drawWorldClock(); break;
    case 4: drawForecastPage(); break;
  }
  display.display();
}