/*
 *   ESP8266 Template
 *   Include all config files and Setup Function declarations
 */
#ifndef USER_SETUP_H
#define USER_SETUP_H

#include "mqtt-ota-config.h"

// Define required user libraries here
// Don't forget to add them into platformio.ini
//#define <user_lib.h>

// Declare user setup, main and custom functions
extern void user_setup();
extern void user_loop();

// Declare global user specific objects
// extern abc xyz;

// MQTT Topic Tree prepended to all topics
// ATTN: Must end with "/"!
// will be set in mqtt-ota-config.h if not defined here
//#define TOPTREE "HB7/Test/"

#endif // USER_SETUP_H