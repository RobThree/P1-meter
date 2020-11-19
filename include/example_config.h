// COPY THIS FILE AND RENAME IT TO "config.h" AND ENTER ALL YOUR CONFIGURATION DATA BELOW

#ifndef CONFIG_H
#define CONFIG_H

// Replace with your actual SSID and password:
#define WIFI_SSID       "***SSID***"
#define WIFI_PASSWD     "***WIFIPASSWORD***"

#define OTA_HOSTNAME    "***HOSTNAME***"
#define OTA_PASSWORD    "***OTAPASSWORD***" // See example_env_secrets.ini / env_secrets.ini

#define POSTURL "http://server.local/p1meter/"

#define RXPIN D5        // RX Serial pin
#define BAUDRATE 115200
#define SERIALCONFIG SWSERIAL_8N1

#endif