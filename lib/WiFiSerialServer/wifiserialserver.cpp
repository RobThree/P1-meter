#include "wifiserialserver.h"

void WiFiSerialServer::handle() {
    // Only allow one client at a time. When server.hasClient() returns true, it means a new client is trying to
    // connect. We disconnect the current client and accept the new one.
    if (_serialserver.hasClient()) {
        _serialclient.stop();
        _serialclient = _serialserver.accept();
    }
}

void WiFiSerialServer::send(String data) {
    if (_serialclient && _serialclient.connected()) {
        // We SHOULD be able to send the entire telegram in one go:
        // https://github.com/esp8266/Arduino/issues/3218#issuecomment-300152069
        _serialclient.write(data.c_str(), data.length());
    }
}