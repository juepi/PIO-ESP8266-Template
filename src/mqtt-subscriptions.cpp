/*
 * ESP8266 Template
 * Definition of MQTT subscriptions
 */
#include "mqtt-ota-config.h"
#include "user-config.h"

//
// MqttSubscriptions is a dataset with all configuration information required
// to subscribe to configured topics and handle the received messages
// Results of decoded messages will be stored into a configured global variable using a pointer
//
// Configuration:
// .Topic: String of topic to subscribe to
// .Type:   0 = bool (expected message "on" or "off")
//          1 = integer (int)
//          2 = float
// .Subscribed: flag, true if successfully subscribed to topic
// .MsgRcvd: Counts messages received for subscribed topic (needs to be initialized with 0 here!)
// .[Bool|Int|Float]Ptr: Pointer to a global var (according to "Type") where the decoded message info will be stored 
//

const int SubscribedTopicCnt = 2; // Overall amount of topics to subscribe to

MqttSubCfg MqttSubscriptions[SubscribedTopicCnt]={
    {.Topic = ota_topic, .Type = 0, .Subscribed = false, .MsgRcvd = 0, .BoolPtr = &OTAupdate },
    {.Topic = otaInProgress_topic, .Type = 0, .Subscribed = false, .MsgRcvd = 0, .BoolPtr = &OtaInProgress }
};
