#ifndef SETTINGS_H
#define SETTINGS_H

#include "appsettings.h"
#include <LittleFS.h>

class Settings {
  public:
    Settings(String filename = SETTINGS_FILENAME) : _filename(filename) {}
    void begin();
    bool loadSettings(AppSettings &settings, std::function<void(AppSettings &)> getDefaultSettings);
    bool saveSettings(const AppSettings &newsettings, AppSettings &settings);
    static const char *SETTINGS_FILENAME;

  private:
    String _filename;
};

#endif // SETTINGS_H
