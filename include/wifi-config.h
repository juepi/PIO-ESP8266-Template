/*
 *   ESP8266 Template
 *   WiFi Settings
 */
#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

#include "macro-handling.h"

// Set WiFi Sleep Mode
// ====================
// CAUTION: Light sleep might disconnect you from broker during sketch execution!
// Handle with care!
#define WIFISLEEP WIFI_NONE_SLEEP
//#define WIFISLEEP WIFI_LIGHT_SLEEP

// WiFi Connection Timeout (milliseconds)
// Reset or go to sleep after timeout
#define WIFI_CONNECT_TIMEOUT 15000

// WLAN Network SSID and PSK
// ============================
extern const char *ssid;
extern const char *password;

// DHCP Hostname to report
#define WIFI_DHCPNAME TEXTIFY(CLTNAME)

#endif // WIFI_CONFIG_H