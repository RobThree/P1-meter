#include "settings.h"
#include "appsettings.h"

const char *Settings::SETTINGS_FILENAME = "/settings.bin";

void Settings::begin() { LittleFS.begin(); }

bool Settings::loadSettings(AppSettings &settings, std::function<void(AppSettings &)> getDefaultSettings) {
    File file = LittleFS.open(_filename, "r");
    if (!file || file.size() < sizeof(AppSettings)) {
        getDefaultSettings(settings);
        return true; // No settings file, used default settings
    }
    size_t bytesRead = file.read((uint8_t *)&settings, sizeof(AppSettings));
    file.close();
    return bytesRead == sizeof(AppSettings);
}

bool Settings::saveSettings(const AppSettings &newsettings, AppSettings &settings) {
    File file = LittleFS.open(_filename, "w");
    if (!file) {
        return false;
    }
    size_t bytesWritten = file.write((uint8_t *)&newsettings, sizeof(AppSettings));
    file.close();
    settings = newsettings;
    return bytesWritten == sizeof(AppSettings);
}