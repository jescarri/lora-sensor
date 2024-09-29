#include "lorawan_settings.hpp"
#include "lorawan.hpp"

void lorawan_preferences_init() {
  lorawan_preferences.begin(LMIC_PREF_NS_NAME, RW_MODE);
}

bool lmic_init_needed() {
  return !lorawan_preferences.isKey(LMIC_BYTES_KEY_NAME);
}

void lmic_save() {
  LMIC.globalDutyAvail = 0;
  size_t a =
      lorawan_preferences.putBytes(LMIC_BYTES_KEY_NAME, &LMIC, sizeof(LMIC));
  Serial.print("Saved: ");
  Serial.println(a);
}

void load_lmic() {
  Serial.println("Start of Load LMIC");
  size_t schlen = lorawan_preferences.getBytesLength(LMIC_BYTES_KEY_NAME);
  char buffer[schlen];
  lorawan_preferences.getBytes(LMIC_BYTES_KEY_NAME, buffer, schlen);
  lmic_t *tmp = (lmic_t *)buffer;
  LMIC = *tmp;
  LoraWANDebug(LMIC);
  Serial.println("END of LoadLmic");
}

void resetLmic() { lorawan_preferences.remove(LMIC_BYTES_KEY_NAME); }

bool lorawanConfigPresent() {
  return lorawan_preferences.isKey(LORAWAN_CONFIG_PRESENT_KEY);
}

void saveDevEUID(char dev_euid[]) {
  Serial.println("Savind device EUID");
  lorawan_preferences.putString("dev_euid", dev_euid);
}

u1_t *getAppEUID() {
  char appEUIDstr[50] = {};
  strcpy(appEUIDstr, lorawan_preferences.getString("app_euid").c_str());
  uint8_t len_buffer = 0;
  static u1_t appEUID[8];
  for (int i = 0; i < strlen(appEUIDstr); i += 2) {
    char tmp = appEUIDstr[i + 3];
    appEUIDstr[i + 3] = 0;
    appEUID[len_buffer] = strtoul((const char *)appEUID, NULL, 16);
    len_buffer++;
    appEUIDstr[i + 3] = tmp;
  }
  return appEUID;
}
