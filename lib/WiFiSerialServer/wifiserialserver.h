#ifndef WIFISERIALSERVER_H
#define WIFISERIALSERVER_H

#include <ESP8266WiFi.h>

class WiFiSerialServer {
  public:
    WiFiSerialServer(int port = 23) : _serialserver(port) {};
    void begin() {
        _serialserver.begin();
        _serialserver.setNoDelay(true);
    }
    void handle();
    void send(String data);

  private:
    WiFiServer _serialserver;
    WiFiClient _serialclient;
};

#endif // WIFISERIALSERVER_H
