#ifndef UPTIME_H
#define UPTIME_H

#include <Arduino.h>

class Uptime {
  public:
    void handle();
    void reset();
    unsigned long long getUptimeMilliSeconds();
    float getUptimeSeconds();
    float getUptimeMinutes();
    float getUptimeHours();
    float getUptimeDays();
    String getFormattedUptime();

  private:
    unsigned long _previousmillis = millis();
    unsigned long long _uptime = 0;
};

#endif // UPTIME_H
