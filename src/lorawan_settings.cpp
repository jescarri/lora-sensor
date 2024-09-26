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
