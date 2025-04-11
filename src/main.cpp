#include "appsettings.h"
#include "ota.h"
#include "settings.h"
#include "simplewifi.h"
#include "uptime.h"
#include "webserver.h"
#include <ESP8266HTTPClient.h>
#include <config.h>

OTA ota = OTA();
Settings settings = Settings();
SimpleWiFi wifi = SimpleWiFi();
Uptime uptime = Uptime();
Webserver server(LittleFS, HTTP_PORT);

AppSettings appsettings;
String telegram = "";
String lasttelegram = "";
unsigned long lasttelegrammillis = 0;
unsigned long telegramcount = 0;
bool ledstate = false;

void handleReading();
void handleSettings();

void post(String data);
void readTelegram();

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    // https://arduino-esp8266.readthedocs.io/en/latest/reference.html#serial
    Serial.setRxBufferSize(RXBUFFER);
    Serial.begin(BAUDRATE, SERIALCONFIG, SERIAL_RX_ONLY, 1, true); // true = invert

    settings.begin();
    settings.loadSettings(appsettings, [](AppSettings &defaults) {
        strncpy(appsettings.deviceName, DEVICENAME, sizeof(appsettings.deviceName) - 1);
        defaults.deviceName[sizeof(appsettings.deviceName) - 1] = '\0'; // Ensure null termination

        strncpy(appsettings.reportUrl, REPORTURL, sizeof(appsettings.reportUrl) - 1);
        defaults.reportUrl[sizeof(appsettings.reportUrl) - 1] = '\0'; // Ensure null termination

        defaults.postTimeout = POSTTIMEOUT;
    });

    wifi.begin(PORTALTIMEOUT, WIFICONNECTTIMEOUT, WIFICONNECTRETRIES, appsettings.deviceName);

    server.serveStatic("/", "/index.html");
    server.serveStatic("/css", "/main.css");
    server.serveStatic("/js", "/app.js");
    server.serveStatic("/favicon.ico", "/favicon.svg");
    server.serveStatic("/settings", "/settings.html");
    server.on("/read", HTTP_GET, handleReading);
    server.on("/settings", HTTP_POST, handleSettings);
    server.useDefaultEndpoints();
    server.begin();

    ota.begin(appsettings.deviceName, OTAPASSWORD);
}

void loop() {
    server.handleClient();
    ota.handle();
    wifi.ensureConnected();
    uptime.handle();
    readTelegram();
    yield();
}

void handleReading() {
    JsonDocument response;

    JsonObject wifinode = response["wifi"].to<JsonObject>();
    wifinode["rssi"] = WiFi.RSSI();

    JsonObject p1node = response["p1"].to<JsonObject>();
    p1node["last"] = lasttelegram;
    p1node["count"] = telegramcount;

    char lastUpdateStr[32];
    unsigned long lastupdate = millis() - lasttelegrammillis;
    snprintf(lastUpdateStr, sizeof(lastUpdateStr), "%.2f seconds ago", lastupdate / 1000.0);
    p1node["lastupdate"] = lastUpdateStr;
    p1node["lastupdate_ms"] = lastupdate;

    response["uptime"] = uptime.getFormattedUptime();
    response["uptime_ms"] = uptime.getUptimeMilliSeconds();
    response["devicename"] = appsettings.deviceName;
    response["reporturl"] = appsettings.reportUrl;
    response["posttimeout"] = appsettings.postTimeout;

    server.sendJson(response);
}

void handleSettings() {
    AppSettings newsettings;
    JsonDocument response;
    response["status"] = "";
    JsonArray errors = response["errors"].to<JsonArray>();
    strncpy(newsettings.deviceName, server.arg("devicename").c_str(), sizeof(newsettings.deviceName) - 1);
    newsettings.deviceName[sizeof(newsettings.deviceName) - 1] = '\0'; // Ensure null termination
    if (strlen(newsettings.deviceName) == 0 || strlen(newsettings.deviceName) > 31) {
        errors.add("Device name must be between 1 and 31 characters");
    }
    strncpy(newsettings.reportUrl, server.arg("reporturl").c_str(), sizeof(newsettings.reportUrl) - 1);
    newsettings.reportUrl[sizeof(newsettings.reportUrl) - 1] = '\0'; // Ensure null termination
    if (strlen(newsettings.reportUrl) == 0 || strlen(newsettings.reportUrl) > 255) {
        errors.add("Report URL must be between 1 and 255 characters");
    }
    newsettings.postTimeout = server.arg("posttimeout").toInt();
    if (newsettings.postTimeout < 0 || newsettings.postTimeout > 60000) {
        errors.add("POST timeout must be between 0 and 60000 ms");
    }

    if (errors.size() == 0 && settings.saveSettings(newsettings, appsettings)) {
        response["status"] = "success";
    } else {
        response["status"] = "error";
    }
    server.sendJson(response);
}

void post(String data) {
    WiFiClient client;
    HTTPClient http;

    // Post text as raw data
    http.setTimeout(appsettings.postTimeout);
    http.begin(client, appsettings.reportUrl);
    http.addHeader("Content-Type", "text/plain");
    http.POST(data);
    http.end();
}

void readTelegram() {
    // While data available
    while (Serial.available()) {
        // Read a line and add it to our buffer
        String line = Serial.readStringUntil('\n');
        telegram += line + '\n';

        // Blink LED
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

        yield();

        // If last line was an "end-of-telegram" OR buffer is becoming too large
        if (line.startsWith("!") || telegram.length() > 4096) {
            lasttelegram = telegram;
            // Clear buffer
            telegram = "";
            telegramcount++;
            lasttelegrammillis = millis();

            // Send whatever we got
            post(lasttelegram);

            digitalWrite(LED_BUILTIN, HIGH); // LED OFF
        }
    }
}