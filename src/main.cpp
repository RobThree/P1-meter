#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoOTA.h>
#include <SoftwareSerial.h>
#include <config.h>

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWD;
const char* otapassword = OTA_PASSWORD;
const char* hostname = OTA_HOSTNAME;

WiFiEventHandler gotIpEventHandler, disconnectedEventHandler;

SoftwareSerial p1serial;

String telegram = "";

void blink(int milliseconds, int repeat = 1) {
  for (int i = 0; i < repeat; i++) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(milliseconds);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(milliseconds);
  }
}

void SetupOTA() {
  ArduinoOTA.setHostname(hostname);
  ArduinoOTA.setPassword(otapassword);
  ArduinoOTA.onStart([]() {
    String type = (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem";
    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
}

void SetupWiFi() {
  // Start WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  gotIpEventHandler = WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP& event)
  {
    Serial.printf("Got IP: %s\r\n", event.ip.toString().c_str());
    Serial.println("Starting OTA");
    ArduinoOTA.begin();
    Serial.println("OTA started");
  });

  disconnectedEventHandler = WiFi.onStationModeDisconnected([](const WiFiEventStationModeDisconnected& event)
  {
    Serial.printf("Disconnected. Reason: %d\r\n", event.reason);
    WiFi.reconnect();
  });
}

void StartWiFi() {
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    blink(1000, 5);
    delay(5000);
    ESP.restart();
  }
}

void Post(String data) {
  HTTPClient http;

  // Post text as raw data
  http.begin(POSTURL);
  http.addHeader("Content-Type", "text/plain");
  http.POST(data);
  http.end();
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Setup our P1 serial
  p1serial.begin(BAUDRATE, SERIALCONFIG, RXPIN, -1, true); // Last argument (true) = invert (0 <=> 1)
  
  SetupWiFi();
  SetupOTA();
  StartWiFi();

  // Blink a few times to signal we're connected
  blink(500, 3);
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
      // Send whatever we got
      Post(telegram);
      // Clear buffer
      telegram = "";
    }
  }
}

void loop() {
  ArduinoOTA.handle();
  readTelegram();
}