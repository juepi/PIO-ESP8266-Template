/*
 *   ESP8266 Template
 *   Firmware, DeepSleep and Serial Settings
 */
#ifndef GENERIC_CONFIG_H
#define GENERIC_CONFIG_H

#include <Arduino.h>

// Firmware Information
#define FIRMWARE_NAME "PIO ESP8266 Template"
#define FIRMWARE_VERSION "1.0.0"
#define TEMPLATE_VERSION "1.1.2"

// Serial Output configuration
//
#define BAUD_RATE 115200
#ifdef SERIAL_OUT
#define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
#define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#define DEBUG_PRINTLN(...)
#endif

// ESP DeepSleep Configuration
// DeepSleep duration in Minutes (max. 60)
#define DS_DURATION_MIN 2

#endif // GENERIC_CONFIG_H