/*
 * ESP8266 Template
 * Setup Functions, definitions of global variables and objects
 */
#include "setup.h"

// Define generic global vars
bool JustBooted = true;     // Helper to let you know you're running the first iteration of the main loop()
uint32_t UptimeSeconds = 0; // Uptime counter

// Define WiFi Variables
const char *ssid = WIFI_SSID;
const char *password = WIFI_PSK;

// Define OTA and MQTT related Variables
char message_buff[20];
bool OTAupdate = false;
bool SentUpdateRequested = false;
bool OtaInProgress = false;
bool OtaIPsetBySketch = false;
bool SentOtaIPtrue = false;
#ifdef READVCC
float VCC = 3.333;
#endif

// Set ADC mode to read VCC (Attn: Pin A0 must be floating!)
// ==========================================================
#ifdef READVCC
ADC_MODE(ADC_VCC);
#endif

// Setup WiFi instance
WiFiClient WiFiClt;

// Setup PubSub Client instance
PubSubClient mqttClt(MQTT_BROKER, 1883, MqttCallback, WiFiClt);

// WiFi Setup function
// ====================
void wifi_setup()
{
  // Set WiFi Sleep Mode
  WiFi.setSleepMode(WIFISLEEP);

  // Set WiFi Hostname
  WiFi.hostname(WIFI_DHCPNAME);

  // Connect to WiFi network
  DEBUG_PRINTLN();
  DEBUG_PRINTLN("Connecting to " + String(ssid));
  WiFi.begin(ssid, password);
  // Next command avoids ESP broadcasting an unwanted ESSID..
  WiFi.mode(WIFI_STA);
  unsigned long end_connect = millis() + WIFI_CONNECT_TIMEOUT;
  while (WiFi.status() != WL_CONNECTED)
  {
    if (millis() >= end_connect)
    {
      DEBUG_PRINTLN("");
      DEBUG_PRINTLN("Failed to connect to " + String(ssid));
#ifdef ONBOARD_LED
      ToggleLed(LED, 1000, 4);
#endif
#ifdef DEEP_SLEEP
      ESP.deepSleep(DS_DURATION_MIN * 60000000);
      delay(3000);
#else
      ESP.reset();
#endif
    }
    delay(500);
    DEBUG_PRINT(".");
  }
  DEBUG_PRINTLN("");
  DEBUG_PRINTLN("WiFi connected");
  DEBUG_PRINT("Device IP Address: ");
  DEBUG_PRINTLN(WiFi.localIP());
  DEBUG_PRINT("DHCP Hostname: ");
  DEBUG_PRINTLN(WIFI_DHCPNAME);
#ifdef ONBOARD_LED
  // WiFi connected - blink once
  ToggleLed(LED, 200, 2);
#endif
}

// OTA Setup function
// ===================
void ota_setup()
{
  // Setup OTA Updates
  // ATTENTION: calling MQTT Publish function inside ArduinoOTA functions MIGHT NOT WORK!
  ArduinoOTA.setHostname(OTA_CLTNAME);
  ArduinoOTA.setPassword(OTA_PWD);
  ArduinoOTA.onStart([]()
                     {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
        {
            type = "sketch";
        }
        else
        { // U_SPIFFS
            type = "filesystem";
        } });
  ArduinoOTA.onEnd([]()
                   {
#ifdef ONBOARD_LED
                     ToggleLed(LED, 200, 4);
#else
                     // ATTENTION: calling MQTT Publish function here does NOT WORK!
                     delay(200);
#endif
                   });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                        {
        int percentComplete = (progress / (total / 100));
        if (percentComplete == 100)
        {
            DEBUG_PRINTLN("Upload complete.");
            delay(500);
        } });
  ArduinoOTA.onError([](ota_error_t error)
                     {
        DEBUG_PRINTLN("Error: " + String(error));
        delay(500); });
  ArduinoOTA.begin();
}

/*
 * Main Setup Function
 * ========================================================================
 */
void setup()
{
// start serial port and digital Outputs
#ifdef SERIAL_OUT
  Serial.begin(BAUD_RATE);
#endif
  DEBUG_PRINTLN();
  DEBUG_PRINTLN(FIRMWARE_NAME);
  DEBUG_PRINTLN(FIRMWARE_VERSION);
#ifdef ONBOARD_LED
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LEDOFF);
#endif

  // Startup WiFi
  wifi_setup();

  // Setup OTA
  ota_setup();

  // Setup user specific stuff
  user_setup();

#ifdef ONBOARD_LED
  // Signal setup finished
  ToggleLed(LED, 200, 6);
#endif
}
