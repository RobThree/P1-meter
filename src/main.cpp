#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <SoftwareSerial.h>
#include <WiFiManager.h>
#include <Wire.h>
#include <config.h>
#include <settings.h>

SoftwareSerial p1serial;
ESP8266WebServer server(HTTP_PORT);
ESP8266HTTPUpdateServer httpUpdateServer;
Settings settings;
String telegram = "";
String lasttelegram = "";
unsigned long lasttelegramMillis = 0; // Store the last time a telegram was sent
const char *const SETTINGS_FILENAME = "/settings.bin";

void Post(String data) {
    WiFiClient client;
    HTTPClient http;

    // Post text as raw data
    http.begin(client, REPORTURL);
    http.addHeader("Content-Type", "text/plain");
    http.POST(data);
    http.end();
}

void restart(const String &status) {
    delay(3000);
    ESP.restart();
}

bool loadSettings() {
    File file = LittleFS.open(SETTINGS_FILENAME, "r");
    if (!file || file.size() < sizeof(Settings)) {
        strncpy(settings.deviceName, DEVICENAME, sizeof(settings.deviceName) - 1);
        settings.deviceName[sizeof(settings.deviceName) - 1] = '\0'; // Ensure null termination
        return false;                                                // No settings file, use default settings
    }
    size_t bytesRead = file.read((uint8_t *)&settings, sizeof(Settings));
    file.close();
    return bytesRead == sizeof(Settings);
}

bool saveSettings(const Settings &newsettings) {
    File file = LittleFS.open(SETTINGS_FILENAME, "w");
    if (!file) {
        return false;
    }
    size_t bytesWritten = file.write((uint8_t *)&newsettings, sizeof(Settings));
    file.close();
    settings = newsettings;
    return bytesWritten == sizeof(Settings);
}

void serveFile(const char *path, const char *contentType = "text/html", int cacheTTL = 300) {
    File file = LittleFS.open(path, "r");
    if (!file) {
        server.send(404, "text/plain", "File not found");
        return;
    }
    server.sendHeader("Cache-Control", "max-age=" + String(cacheTTL));
    server.streamFile(file, contentType);
    file.close();
}

void readTelegram() {
    // While data available
    while (p1serial.available()) {
        // Read a line
        String line = p1serial.readStringUntil('\n');
        // Add to our buffer
        telegram += line + '\n';
        yield();
        // If last line was an "end-of-telegram" OR buffer is becoming too large
        if (line.startsWith("!") || telegram.length() > 4096) {
            lasttelegramMillis = millis();
            lasttelegram = telegram;
            // Clear buffer
            telegram = "";

            // Send whatever we got
            Post(lasttelegram);
        }
    }
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    p1serial.begin(BAUDRATE, SERIALCONFIG, RXPIN, -1, true, 256); // Last boolean argument (true) = invert (0 <=> 1)

    LittleFS.begin();
    loadSettings();

    WiFiManager wifiManager;
    wifiManager.setConfigPortalTimeout(PORTALTIMEOUT);
    wifiManager.setConnectTimeout(WIFICONNECTTIMEOUT);
    wifiManager.setConnectRetries(WIFICONNECTRETRIES);
    wifiManager.setWiFiAutoReconnect(true);
    if (!wifiManager.autoConnect(settings.deviceName)) {
        restart("Autconnect failed...");
    }

    httpUpdateServer.setup(&server);
    server.on("/read", HTTP_GET, []() {
        JsonDocument root;
        JsonObject wifinode = root["wifi"].to<JsonObject>();
        wifinode["rssi"] = WiFi.RSSI();
        root["devicename"] = settings.deviceName;

        String response;
        serializeJson(root, response);

        server.send(200, "application/json", response);
    });

    server.on("/reset", HTTP_PUT, []() {
        server.send(200, "text/html", "reset");
        restart("Restarting");
    });

    server.on("/", HTTP_GET, []() { serveFile("/index.html"); });
    server.on("/css", HTTP_GET, []() { serveFile("/main.css", "text/css"); });
    server.on("/js", HTTP_GET, []() { serveFile("/app.js", "text/javascript"); });
    server.on("/favicon.ico", HTTP_GET, []() { serveFile("/favicon.svg", "image/svg+xml", 60 * 60 * 24); });
    server.on("/settings", HTTP_GET, []() { serveFile("/settings.html"); });
    server.on("/settings", HTTP_POST, []() {
        Settings newsettings;
        JsonDocument root;
        root["status"] = "";
        JsonArray errors = root["errors"].to<JsonArray>();
        strncpy(newsettings.deviceName, server.arg("devicename").c_str(), sizeof(newsettings.deviceName) - 1);
        newsettings.deviceName[sizeof(newsettings.deviceName) - 1] = '\0'; // Ensure null termination
        if (strlen(newsettings.deviceName) == 0 || strlen(newsettings.deviceName) > 31) {
            errors.add("Device name must be between 1 and 31 characters");
        }

        if (errors.size() == 0 && saveSettings(newsettings)) {
            root["status"] = "success";
        } else {
            root["status"] = "error";
        }
        String response;
        serializeJson(root, response);

        server.send(200, "application/json", response);
    });

    server.onNotFound([]() { server.send(404, "text/plain", "File not found"); });

    server.begin();

    ArduinoOTA.setPassword(OTAPASSWORD);
    ArduinoOTA.setHostname(settings.deviceName);
    ArduinoOTA.onStart([]() { String type = (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem"; });
    ArduinoOTA.onEnd([]() {});
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {});
    ArduinoOTA.onError([](ota_error_t error) {});
    ArduinoOTA.begin();
}

void loop() {
    server.handleClient();
    ArduinoOTA.handle();
    if ((WiFi.status() != WL_CONNECTED)) {
        WiFi.reconnect();
        unsigned long start = millis();

        while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
            delay(100);
        }

        if (WiFi.status() != WL_CONNECTED) {
            delay(1000);
        }
    }

    readTelegram();
}