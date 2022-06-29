/*
*   ESP8266 Template
*   Include all config files and Setup Function declarations
*/
#ifndef SETUP_H
#define SETUP_H

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include "wifi-config.h"
#include "generic-config.h"
#include "mqtt-ota-config.h"
#include "common-functions.h"
#include "hardware-config.h"

// Declare setup functions
extern void wifi_setup();
extern void ota_setup();

#endif //SETUP_H