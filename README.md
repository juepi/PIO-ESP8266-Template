# Introduction 
A template for ESP8266 programming using VSC + [PlatformIO](https://platformio.org/) supporting MQTT, OTA-flashing, ESP Deep-Sleep and VCC readouts.  

## Local Requirements
A (local) MQTT broker is mandatory for OTA-Flashing. With deactivated OTA-flashing, you might remove MQTT functionality.  
Additionally, personal settings like WIFI SSID and Passphrase will be talken from local environment variables, see `platformio.ini`.  

## Hardware Requirements
To be able to use DEEP_SLEEP functionality, you will most probably need a small hardware modification for you ESP board: connect pin D0 to RST pin. This will allow the ESP to wake up after the defined sleep time as defined in the `include/generic-config.h` file.  

## Configuration
In addition to the `platformio.ini` file, see header files in the `include/` folder for additional settings.  
Configureable settings should be documented well enough there - hopefully ;-)

### A short notice on MQTT usage
As the main intention of this program is to run on a MCU that is powered off (sleeping) most of the time, we will **only work with retained messages** here! This ensures that a client subscribing to a topic will receive the last value published "instantly" and does not need to wait for someone to publish "latest news".

### An even shorter notice on "instantly" receiving MQTT topics
I've made the experience that it can take quite a while after subscribing to a MQTT topic on the broker until the actual value is being received by the ESP. "Quite a while" means an ESP uptime (millis) of about 27 seconds (see `WAIT_MILLIS_FOR_TOPICS_AFTER_BOOT` in file `mqtt-ota-config.h`). This is especially important for OTA updating, as we rely on some topics for the update process.

## MQTT Topics used
In order to run OTA updates, you will need at least the following MQTT topics on your broker (case sensitive) to be pre-created so ESP can subscribe to them:

* `topic/tree/OTAupdate` - default retained Value: **off**  
This will be translated to a bool variable in the sketch. You will need to set the topic value either to "on" or "off". During normal operation, this Topic needs to be set to "off". If you want to run an OTA-update on your ESP, set it to "on" (retained).  
After a successful update, the ESP will reset this flag to "off".

* `topic/tree/OTAinProgress` - default retained Value: **off**  
This is a helper flag topic required by the ESP.

* `topic/tree/OTAstatus` - default Value: none  
The ESP will publish OTA status strings here. No need to pre-create this topic.

* `topic/tree/Vcc` - default Value: none  
The sketch will publish the voltage measured on the 3.3V supply here. Note that the accuracy is quite low, but it is good enough to detect if the battery is running low when you supply the ESP in example by a LiFePo4 accumulator directly on the 3.3V pin.  
If you want to improve accuracy, measure the actual voltage with a multimeter and adopt the `VCCCORRDIV` in the `hardware-setup.h` file.

## Compiling and flashing walkthrough
I will give a rough walkthrough on the first steps, assuming you have a working PlatformIO environment:

* Prepare system environment variables with WIFI and MQTT Data:
    - WIFI_SSID
    - WIFI_PSK
    - MQTT_BROKER --> IP-Address of your broker
* Adopt `ClientName` in `platformio.ini` as needed
* Prepare `platformio.ini` for wired flashing
Deactivate OTA-flashing in the board specific area:
```
;upload_protocol = ${common_env_data.upload_protocol}
;upload_port = ${common_env_data.upload_port}
;upload_flags = ${common_env_data.upload_flags}
```
* Adopt board info and hardware settings in `include/hardware-config.h` if needed
* Compile and flash

**To re-flash the sketch over OTA:**
* Prepare `platformio.ini` for OTA flashing
Activate OTA-flashing in the board specific area:
```
upload_protocol = ${common_env_data.upload_protocol}
upload_port = ${common_env_data.upload_port}
upload_flags = ${common_env_data.upload_flags}
```
* Set topic `topic/tree/OTAupdate` retained "on"
* wait for the ESP to start up (or reset your ESP)
* When the ESP boots, it will slowly blink the onboard LED until all MQTT topics are received (`WAIT_MILLIS_FOR_TOPICS_AFTER_BOOT`)
* After that, the onboard LED will start flashing rapidly. The ESP is now waiting for the new binary to be uploaded
* Click "Upload" in PIO to compile and upload the new sketch
* When the upload has finished, the ESP will boot the new sketch and finish the OTA update process
* You can verify the status by reading the `topic/tree/OTAstatus` topic, which should throw the string "update_success"