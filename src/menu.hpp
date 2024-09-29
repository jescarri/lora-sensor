#ifndef MENU_HPP_
#define MENU_HPP_
#include <Arduino.h>
#include <Preferences.h>
#include <WiFiManager.h>
#include <lmic.h>

#define CONFIG_TIMEOUT_SECONDS 120
#define APP_EUID_LEN 17

extern Preferences lorawan_preferences;

void loadSetings();
void saveConfigCallback();
void configModeCallback(WiFiManager *myWiFiManager);

void initMenu();
void startWebConf();

#endif
