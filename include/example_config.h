// COPY THIS FILE AND RENAME IT TO "config.h" AND ENTER ALL YOUR CONFIGURATION DATA BELOW

#ifndef CONFIG_H
#define CONFIG_H

// These can all be set in the web interface and are used as defaults
#define DEVICENAME          "p1-meter"

// These can only be set by re-flashing the firmware
#define OTAPASSWORD         "my_ota_secret"
#define PORTALTIMEOUT       90
#define WIFICONNECTTIMEOUT  10
#define WIFICONNECTRETRIES  255
#define HTTP_PORT           80
#define SERIAL_BAUDRATE     115200
#define REPORTURL           "http://server.local/p1meter/"

#define BAUDRATE 115200
#define SERIALCONFIG SERIAL_8N1
#define RXBUFFER 1500

#endif