# Introduction 
A template for ESP8266 programming using VSC + PlatformIO supporting MQTT, OTA-flashing, ESP Deep-Sleep and VCC readouts.  

## Local Requirements
A local MQTT broker is mandatory for OTA-Flashing. With deactivated OTA-flashing, you might remove MQTT functionality.  
Additionally, personal settings like WIFI SSID and Passphrase will be talken from local environment variables, see platformio.ini.  

## Hardware Requirements
To be able to use DEEP_SLEEP functionality, you will most probably need a small hardware modification for you ESP board: connect pin D0 to RST pin. This will allow the ESP to wake up after the defined sleep time as defined in the include/generic-config.h file.  

## Configurarion
In addition to the platformio.ini file, see header files in the include/ folder for additional settings.  
The mqtt-ota-config.h is of special importance, as it will allow you to adopt the MQTT topics for your needs.  

