/*
 *   ESP8266 Template
 *   MQTT and OTA-Flash Settings
 */
#ifndef MQTT_OTA_CONFIG_H
#define MQTT_OTA_CONFIG_H

#include <Arduino.h>
#include <PubSubClient.h>
#include "wifi-config.h"
#include "macro-handling.h"
#include "user-config.h"

//
// MQTT Broker Settings
//
// MQTT Client name used to subscribe to topics
#define MQTT_CLTNAME TEXTIFY(CLTNAME)
// Maximum connection attempts to MQTT broker before going to sleep
#define MAXCONNATTEMPTS 3
// Maximum retry attempts to receive messages for all subscribed topics; ESP will reset or DEEP_SLEEP if defined
// default setting of 300 should try for ~30sec to fetch messages for all subscribed topics
#define MAX_TOP_RCV_ATTEMPTS 300
// Message buffer for incoming Data from MQTT subscriptions
extern char message_buff[20];

// Default QoS for MQTT subscriptions (see platformio.ini)
#ifndef SUB_QOS
#define SUB_QOS 0
#endif

// MQTT Topic Tree prepended to all topics
// ATTN: Must end with "/"!
// alternatively defined in user-config.h
#ifndef TOPTREE
#define TOPTREE "HB7/Test/"
#endif

//
// OTA-Update MQTT Topics and corresponding global vars
//
// OTA Client Name
#define OTA_CLTNAME TEXTIFY(CLTNAME)
// OTA Update specific vars
// to start an OTA update on the ESP, you will need to set ota_topic to "on"
// (don't forget to add the "retain" flag, especially if you want a sleeping ESP to enter flash mode at next boot)
#define ota_topic TOPTREE "OTAupdate" // local BOOL, MQTT either "on" or "off"
extern bool OTAupdate;
#define otaStatus_topic TOPTREE "OTAstatus" // textual OTA-update status sent to broker
// OTAstatus strings sent by sketch
#define UPDATEREQ "update_requested"  // Waiting for binary upload
#define UPDATECANC "update_cancelled" // Update cancelled by user (OTAupdate reset to off befor upload)
#define UPDATEOK "update_success"     // Update successful
// An additional "external flag" is required to "remind" a freshly running sketch that it was just OTA-flashed..
// during an OTA update, PubSubClient functions do not ru (or cannot access the network)
// so this flag will be set to ON when actually waiting for the OTA update to start
// it will be reset if OtaInProgress and OTAupdate are true (in that case, ESP has most probably just been successfully flashed)
#define otaInProgress_topic TOPTREE "OTAinProgress" // local BOOL, MQTT either "on" or "off"
extern bool OtaInProgress;
// Internal helpers
extern bool SentUpdateRequested;
extern bool OtaIPsetBySketch;
extern bool SentOtaIPtrue;

//
// Configuration struct for MQTT subscriptions
//
struct MqttSubCfg
{
    const char *Topic; // Topic to subscribe to
    int Type;          // Type of message data received: 0=bool (message "on/off"); 1=int; 2=float
    bool Subscribed;   // true if successfully subscribed to topic
    uint32_t MsgRcvd;  // counter for received messages
    union              // Pointer to Variable which should be updated with the decoded message (only one applies acc. to "Type")
    {
        bool *BoolPtr;
        int *IntPtr;
        float *FloatPtr;
    };
};

extern const int SubscribedTopicCnt; // Number of elements in MqttSubscriptions array (define in mqtt-subscriptions.cpp)
extern MqttSubCfg MqttSubscriptions[];

// Topic where VCC will be published
#ifdef READVCC
#define vcc_topic TOPTREE "Vbat"
#endif

#endif // MQTT_OTA_CONFIG_H