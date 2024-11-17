#include "menu.hpp"
#include "lorawan_settings.hpp"

char char_ttn_app_eui[MAX_LORAWAN_CONF_CHAR_LEN];
char char_ttn_dev_eui[MAX_LORAWAN_CONF_CHAR_LEN];
char char_ttn_app_key[MAX_LORAWAN_CONF_CHAR_LEN];

const char *menu[] = {"param", "restart"};

WiFiManager wifiManager;
WiFiManagerParameter *ttn_app_eui;
WiFiManagerParameter *ttn_dev_eui;
WiFiManagerParameter *ttn_app_key;

void loadSetings() {
  if (lorawan_preferences.isKey("app_eui")) {
    strcpy(char_ttn_app_eui, lorawan_preferences.getString("app_eui").c_str());
    Serial.println(char_ttn_app_eui);
  } else {
    strcpy(char_ttn_app_eui, "00000000");
  }

  if (lorawan_preferences.isKey("dev_eui")) {
    strcpy(char_ttn_dev_eui, lorawan_preferences.getString("dev_eui").c_str());
    Serial.println(char_ttn_dev_eui);
  } else {
    strcpy(char_ttn_dev_eui, "00000000");
  }

  if (lorawan_preferences.isKey("app_key")) {
    strcpy(char_ttn_app_key, lorawan_preferences.getString("app_key").c_str());
    Serial.println(char_ttn_app_key);
  } else {
    strcpy(char_ttn_app_key, "00000000000000000000000000000000");
  }
}

void initMenu() {
  WiFi.mode(WIFI_STA);
  wifiManager.setMinimumSignalQuality(90);
  wifiManager.setRemoveDuplicateAPs(true);
  wifiManager.setSaveParamsCallback(saveConfigCallback);
  loadSetings();
  ttn_app_eui = new WiFiManagerParameter(
      "app_eui", "AppEUI lsb", char_ttn_app_eui, MAX_LORAWAN_CONF_CHAR_LEN);
  ttn_dev_eui = new WiFiManagerParameter(
      "dev_eui", "DevEUI lsb", char_ttn_dev_eui, MAX_LORAWAN_CONF_CHAR_LEN);
  ttn_app_key = new WiFiManagerParameter(
      "app_key", "APP Key msb", char_ttn_app_key, MAX_LORAWAN_CONF_CHAR_LEN);
  wifiManager.addParameter(ttn_app_eui);
  wifiManager.addParameter(ttn_dev_eui);
  wifiManager.addParameter(ttn_app_key);
  wifiManager.setMenu(menu, sizeof(menu) / sizeof(menu[0]));
}

void startWebConf() {
  leds[0] = CRGB::Red;
  FastLED.show();

  initMenu();
  wifiManager.setConfigPortalTimeout(300);
  if (!wifiManager.startConfigPortal("lora-node")) {
    Serial.println("failed to connect and hit timeout");
    ESP.restart();
  }
}

void saveConfigCallback() {
  Serial.println("Should save config");
  lorawan_preferences.putString("app_eui", ttn_app_eui->getValue());
  lorawan_preferences.putString("dev_eui", ttn_dev_eui->getValue());
  lorawan_preferences.putString("app_key", ttn_app_key->getValue());
  lorawan_preferences.putBool("ttn_otaa_config", true);
  leds[0] = CRGB::Green;
  FastLED.show();
}

void configModeCallback(WiFiManager *myWiFiManager) {
  WiFi.setTxPower(WIFI_POWER_8_5dBm);
  Serial.println("[CALLBACK] configModeCallback fired");
}
