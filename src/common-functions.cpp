/*
 * ESP8266 Template
 * Common Functions
 */
#include "setup.h"

// Function to toggle a LED (GPIO pin)
void ToggleLed(int PIN, int WaitTime, int Count)
{
    // Toggle digital output
    for (int i = 0; i < Count; i++)
    {
        digitalWrite(PIN, !digitalRead(PIN));
        delay(WaitTime);
    }
}

// Function to connect to MQTT Broker and subscribe to Topics
bool MqttConnectToBroker()
{
    bool RetVal = false;
    int ConnAttempt = 0;
    // Try to connect x times, then return error
    while (ConnAttempt < MAXCONNATTEMPTS)
    {
        DEBUG_PRINT("Connecting to MQTT broker..");
        if (mqttClt.connect(MQTT_CLTNAME))
        {
            DEBUG_PRINTLN("connected");
            // Reset subscribed/received Topics counters
            int SubscribedTopics = 0;
            for (int i = 0; i < SubscribedTopicCnt; i++)
            {
                MqttSubscriptions[i].Subscribed = false;
                MqttSubscriptions[i].MsgRcvd = 0;
            }
            // Subscribe to all configured Topics
            while ((SubscribedTopics < SubscribedTopicCnt) && mqttClt.connected())
            {
                for (int i = 0; i < SubscribedTopicCnt; i++)
                {
                    // Make sure broker is still connected to avoid looping endlessly
                    if (!mqttClt.connected())
                    {
                        DEBUG_PRINTLN("Lost connection while subscribing to topics, reconnecting!");
                        break;
                    }
                    if (!MqttSubscriptions[i].Subscribed)
                    {
                        if (mqttClt.subscribe(MqttSubscriptions[i].Topic, SUB_QOS))
                        {
                            MqttSubscriptions[i].Subscribed = true;
                            SubscribedTopics++;
                            yield();
                        }
                    }
                }
            }
            if (SubscribedTopics == SubscribedTopicCnt)
            {
                // All done
                mqttClt.loop();
                delay(100);
                RetVal = true;
                break;
            }
            else
            {
                DEBUG_PRINTLN("Something went wrong, restarting..");
                delay(1000);
                ConnAttempt++;
            }
        }
        else
        {
            DEBUG_PRINTLN("failed, rc=" + String(mqttClt.state()));
            DEBUG_PRINTLN("Sleeping 2 seconds..");
            delay(2000);
            ConnAttempt++;
        }
    }
    return RetVal;
}

// Function to handle MQTT stuff (broker connections, subscriptions, updates), called in main loop
void MqttUpdater()
{
    if (!mqttClt.connected())
    {
        if (MqttConnectToBroker())
        {
            // New connection to broker, fetch topics
            // ATTN: only try for MAX_TOP_RCV_ATTEMPTS then end (reboot or DEEP_SLEEP)
            DEBUG_PRINT("Waiting for messages from subscribed topics..");
            int TopicRcvAttempts = 0;
            bool MissingTopics = true;
            while (TopicRcvAttempts < MAX_TOP_RCV_ATTEMPTS)
            {
                MissingTopics = false;
                for (int i = 0; i < SubscribedTopicCnt; i++)
                {
                    if (MqttSubscriptions[i].MsgRcvd == 0)
                    {
                        MissingTopics = true;
                        break;
                    }
                }
                if (MissingTopics)
                {
                    DEBUG_PRINT(".:T!:.");
                    TopicRcvAttempts++;
                    if (!mqttClt.loop())
                    {
                        DEBUG_PRINTLN("  Lost connection to broker while waiting for topics, reconnecting.");
                        MqttConnectToBroker();
                    }
#ifdef ONBOARD_LED
                    ToggleLed(LED, 50, 2);
#else
                    delay(100);
#endif
                }
                else
                {
                    DEBUG_PRINTLN("");
                    DEBUG_PRINTLN("Messages for all subscribed topics received.");
                    break;
                }
            }
            if (MissingTopics)
            {
#ifdef DEEP_SLEEP
                ESP.deepSleep(DEEP_SLEEP * 60000000);
#else
                ESP.restart();
#endif
            }
            DEBUG_PRINTLN("");
            DEBUG_PRINTLN("Messages for all subscribed topics received.");
        }
        else
        {
            DEBUG_PRINTLN("Unable to connect to MQTT broker.");
#ifdef ONBOARD_LED
            ToggleLed(LED, 100, 4);
#endif
#ifdef DEEP_SLEEP
            ESP.deepSleep(DEEP_SLEEP * 60000000);
#else
            ESP.restart();
#endif
        }
    }
    else
    {
        mqttClt.loop();
        yield();
    }
}

// Function to periodically handle MQTT stuff while delaying
// This causes some inaccuracy in the delay of course.
void MqttDelay(uint32_t delayms)
{
    //  Call MqttUpdater every 200ms
    int Counter = delayms / 200;
    if (Counter == 0)
    {
        // Delay less than 200ms requested, just run delay and return
        delay(delayms);
        return;
    }
    else
    {
        for (int i = 0; i < Counter; i++)
        {
            MqttUpdater();
            delay(200);
        }
    }
}

// Function to handle OTA flashing (called in main loop)
// Returns TRUE while OTA-update was requested or in progress
bool OTAUpdateHandler()
{
    // If OTA Firmware Update is requested,
    // only loop through OTA function until finished (or reset by MQTT)
    if (OTAupdate)
    {
        if (OtaInProgress && !OtaIPsetBySketch)
        {
            DEBUG_PRINTLN("OTA firmware update successful, resuming normal operation..");
            // Make sure that MQTT Broker is connected
            MqttUpdater();
            mqttClt.publish(otaStatus_topic, String(UPDATEOK).c_str(), true);
            mqttClt.publish(ota_topic, String("off").c_str(), true);
            mqttClt.publish(otaInProgress_topic, String("off").c_str(), true);
            OTAupdate = false;
            OtaInProgress = false;
            OtaIPsetBySketch = true;
            SentOtaIPtrue = false;
            SentUpdateRequested = false;
            delay(200);
            return false;
        }
        if (!SentUpdateRequested)
        {
            mqttClt.publish(otaStatus_topic, String(UPDATEREQ).c_str(), true);
            SentUpdateRequested = true;
        }
        DEBUG_PRINTLN("OTA firmware update requested, waiting for upload..");
#ifdef ONBOARD_LED
        // Signal OTA update requested
        ToggleLed(LED, 100, 5);
#endif
        // set MQTT reminder that OTA update was executed
        if (!SentOtaIPtrue)
        {
            DEBUG_PRINTLN("Setting MQTT OTA-update reminder flag on broker..");
            mqttClt.publish(otaInProgress_topic, String("on").c_str(), true);
            OtaInProgress = true;
            SentOtaIPtrue = true;
            OtaIPsetBySketch = true;
            delay(200);
        }
        // call OTA function to receive upload
        ArduinoOTA.handle();
        return true;
    }
    else
    {
        if (SentUpdateRequested)
        {
            DEBUG_PRINTLN("OTA firmware update cancelled by user, cleaning up and rebooting..");
            // Make sure that MQTT Broker is connected
            MqttUpdater();
            mqttClt.publish(otaStatus_topic, String(UPDATECANC).c_str(), true);
            mqttClt.publish(otaInProgress_topic, String("off").c_str(), true);
            delay(200);
            // Reboot after cancelled update
            ESP.restart();
            delay(500);
            return false;
        }
    }
    return false;
}

/*
 * Callback Functions
 * ========================================================================
 */
//
// MQTT Subscription callback function
//
void MqttCallback(char *topic, byte *payload, unsigned int length)
{
    unsigned int i = 0;
    // create character buffer with ending null terminator (string)
    for (i = 0; i < length; i++)
    {
        message_buff[i] = payload[i];
    }
    message_buff[i] = '\0';
    String msgString = String(message_buff);

    DEBUG_PRINTLN("MQTT: Message arrived [" + String(topic) + "]: " + String(msgString));

    // run through topics
    for (int i = 0; i < SubscribedTopicCnt; i++)
    {
        if (String(topic) == String(MqttSubscriptions[i].Topic))
        {
            // Topic found, handle message
            switch (MqttSubscriptions[i].Type)
            {
            case 0:
                // Handle subscription Type BOOL
                if (msgString == "on")
                {
                    *MqttSubscriptions[i].BoolPtr = true;
                    MqttSubscriptions[i].MsgRcvd++;
                }
                else if (msgString == "off")
                {
                    *MqttSubscriptions[i].BoolPtr = false;
                    MqttSubscriptions[i].MsgRcvd++;
                }
                else
                {
                    DEBUG_PRINTLN("MQTT: ERROR: Fetched invalid BOOL for topic [" + String(topic) + "]: " + String(msgString));
                }
                break;
            case 1:
                // Handle subscription of type INTEGER
                *MqttSubscriptions[i].IntPtr = (int)msgString.toInt();
                MqttSubscriptions[i].MsgRcvd++;
                break;
            case 2:
                // Handle subscriptions of type FLOAT
                *MqttSubscriptions[i].FloatPtr = msgString.toFloat();
                MqttSubscriptions[i].MsgRcvd++;
                break;
            }
        }
    }
}
