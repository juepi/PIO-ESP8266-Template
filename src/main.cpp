/*
 * ESP8266 Template
 * ==================
 *
 * Includes useful functions like
 * - DeepSleep
 * - Read VCC
 * - MQTT
 * - OTA Updates (ATTN: requires MQTT!)
 *
 * If you use DeepSleep, make sure to connect pin 16 (D0) to RESET, or your ESP will never wake up!
 * Also keep in mind that you can DeepSleep for ~ 1 hour max (hardware limitation)!
 *
 * ATTENTION: This sketch is optimized to receive subscribed MQTT topics as quickly as possible (to minimize runtime / maximize battery runtime).
 * This has the drawback, that we expect to receive a (retained) message for every subscribed topic. The sketch will run in an endless loop
 * if subscribing to an empty topic! See MqttUpdater function in common-functions.cpp!
 *
 * Keep in mind that you'll need a reliable power source for OTA updates, 2x AA batteries might not be sufficient.
 * If you brick your ESP during OTA update, you can probably revive it by flashing it wired.
 */
#include "setup.h"

// HINT: setup loop in setup.cpp !

/*
 * Main Loop
 * ========================================================================
 */
void loop()
{
  delay(200);
  // Check connection to MQTT broker, subscribe and update topics
  MqttUpdater();

#ifdef OTA_UPDATE
  // Handle OTA updates
  OTAUpdateHandler();
#endif

// START STUFF YOU WANT TO RUN HERE!
// ============================================
#ifdef ONBOARD_LED
  // Toggle LED at each loop
  ToggleLed(LED, 500, 4);
#endif

  // Read VCC and publish to MQTT
  delay(300);
  VCC = ((float)ESP.getVcc()) / VCCCORRDIV;
  mqttClt.publish(vcc_topic, String(VCC).c_str(), true);
  delay(100);

#ifdef DEEP_SLEEP
  // disconnect WiFi and go to sleep
  DEBUG_PRINTLN("Good night for " + String(DS_DURATION_MIN) + " minutes.");
  WiFi.disconnect();
  ESP.deepSleep(DS_DURATION_MIN * 60000000);
#else
  DEBUG_PRINTLN("Loop finished, DeepSleep disabled. Restarting in 5 seconds.");
#endif
  // ATTN: Sketch continues to run for a short time after initiating DeepSleep, so pause here
  delay(5000);
}