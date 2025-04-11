#ifndef APPSETTINGS_H
#define APPSETTINGS_H

struct AppSettings {
    char deviceName[32];
    char reportUrl[256];
    int postTimeout;
};

#endif // APPSETTINGS_H