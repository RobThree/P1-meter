#ifndef SIMPLEWIFI_H
#define SIMPLEWIFI_H

#include <WiFiManager.h>

class SimpleWiFi {
  public:
    void begin(unsigned long portalTimeout, unsigned long wifiConnectTimeout, unsigned long wifiConnectRetries,
               const char *apName, const char *apPassword = (const char *)__null);
    void ensureConnected();
    String localIP();

  private:
    WiFiManager _wifiManager;
};

#endif // SIMPLEWIFI_H