#include "ota.h"
#include <ArduinoOTA.h>

void OTA::begin(const char *const hostname, const char *const password) {
    ArduinoOTA.setHostname(hostname);
    ArduinoOTA.setPassword(password);
    ArduinoOTA.begin();
}

void OTA::handle() { ArduinoOTA.handle(); }