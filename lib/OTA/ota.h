#ifndef OTA_H
#define OTA_H

#include <ArduinoOTA.h>

class OTA {
  public:
    void begin(const char *const hostname, const char *const password);
    void handle();
};

#endif // OTA_H
