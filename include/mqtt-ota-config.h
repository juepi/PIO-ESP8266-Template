/*
*   ESP8266 Template
*   MQTT and OTA-Flash Settings
*/
#ifndef MQTT_OTA_CONFIG_H
#define MQTT_OTA_CONFIG_H

#include <Arduino.h>
#include <PubSubClient.h>
#include "wifi-config.h"

// MQTT Broker Settings
//
// Maximum connection attempts to MQTT broker before going to sleep
#define MAXCONNATTEMPTS 3
// Message buffer for incoming Data from MQTT subscriptions
extern char message_buff[20];

// Required time to fetch topics after a boot
// Important during OTA-flash
//
#define WAIT_MILLIS_FOR_TOPICS_AFTER_BOOT 27000

// MQTT Topics and corresponding local vars
// ===========================================
#ifdef OTA_UPDATE
//OTA Update specific vars
//to start an OTA update on the ESP, you will need to set ota_topic to "on"
//(don't forget to add the "retain" flag, especially if you want a sleeping ESP to enter flash mode at next boot)
#define ota_topic "HB7/Test/OTAupdate" //local BOOL, MQTT either "on" or "off"
extern bool OTAupdate;
#define otaStatus_topic "HB7/Test/OTAstatus"
// OTAstatus strings sent by sketch
#define UPDATEREQ "update_requested"
#define UPDATECANC "update_cancelled"
#define UPDATEOK "update_success"
extern bool SentUpdateRequested;
//An additional "external flag" is required to "remind" a freshly running sketch that it was just OTA-flashed..
//during an OTA update, PubSubClient functions do not ru (or cannot access the network)
//so this flag will be set to ON when actually waiting for the OTA update to start
//it will be reset if OtaInProgress and OTAupdate are true (in that case, ESP has most probably just been successfully flashed)
#define otaInProgress_topic "HB7/Test/OTAinProgress" //local BOOL, MQTT either "on" or "off"
extern bool OtaInProgress;
extern bool OtaIPsetBySketch;
extern bool SentOtaIPtrue;
#endif

// Topic where VCC will be published
#define vcc_topic "HB7/Test/Vcc"
extern float VCC;

#endif //MQTT_OTA_CONFIG_H