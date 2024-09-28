#include "menu.hpp"
#include "lorawan_settings.hpp"

const char *menu[] = {"param"};
char char_ttn_app_eui[40] = "0000000000000000";
WiFiManager wifiManager;
WiFiManagerParameter ttn_app_eui = WiFiManagerParameter(
    "euid", "app eui lsb", char_ttn_app_eui, sizeof(char_ttn_app_eui));

bool configPresent() {
  if (lorawan_preferences.isKey(INIT_KEY)) {
    // Read the preferences and copy values to custom parameter
    // char_ttn_app_eui = lorawan_preferences.getChar(APP_EUID_KEY);
    return true;
  }
  return false;
}

void initMenu() {
  WiFi.mode(WIFI_STA);
  wifiManager.setMinimumSignalQuality(90);
  wifiManager.setRemoveDuplicateAPs(true);
  wifiManager.setSaveParamsCallback(saveConfigCallback);

  wifiManager.setMenu(menu, sizeof(menu) / sizeof(menu[0]));
  wifiManager.addParameter(&ttn_app_eui);
}

void startWebConf() {
  wifiManager.setConfigPortalTimeout(120);
  if (!wifiManager.startConfigPortal("lora-node")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    // reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(5000);
  }
}

void saveConfigCallback() {
  Serial.println("Should save config");
  ESP.restart();
}

void configModeCallback(WiFiManager *myWiFiManager) {
  WiFi.setTxPower(WIFI_POWER_8_5dBm);
  Serial.println("[CALLBACK] configModeCallback fired");
}
