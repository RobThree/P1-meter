#include "uptime.h"
#include <Arduino.h>

void Uptime::handle() {
    unsigned long currentmillis = millis();
    _uptime += currentmillis - _previousmillis;
    _previousmillis = currentmillis;
}

unsigned long long Uptime::getUptimeMilliSeconds() { return _uptime; }
float Uptime::getUptimeSeconds() { return _uptime / 1000.0f; }
float Uptime::getUptimeMinutes() { return _uptime / 60000.0f; }
float Uptime::getUptimeHours() { return _uptime / 3600000.0f; }
float Uptime::getUptimeDays() { return _uptime / 86400000.0f; }
String Uptime::getFormattedUptime() {
    unsigned long long ms = _uptime;

    unsigned long days = ms / 86400000;
    ms %= 86400000;
    unsigned long hours = ms / 3600000;
    ms %= 3600000;
    unsigned long minutes = ms / 60000;
    ms %= 60000;
    unsigned long seconds = ms / 1000;

    String result = "";
    result += String(days) + " day";
    if (days != 1)
        result += "s";
    result += ", " + String(hours) + " hour";
    if (hours != 1)
        result += "s";
    result += ", " + String(minutes) + " minute";
    if (minutes != 1)
        result += "s";
    result += ", " + String(seconds) + " second";
    if (seconds != 1)
        result += "s";

    return result;
}

void Uptime::reset() { _uptime = 0; }