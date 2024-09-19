#include "lorawan_settings.hpp"

void lorawan_preferences_init() {
  lorawan_preferences.begin(LMIC_PREF_NS_NAME, RW_MODE);
}

bool lmic_init_needed() {
  return lorawan_preferences.isKey(LMIC_INIT_NEEDED_KEY_NAME);
}

void lmic_save(lmic_t value) {
  lorawan_preferences.putBytes(LMIC_BYTES_KEY_NAME, &value, sizeof(value));
}

lmic_t load_lmic() {
  lmic_t data;
  lorawan_preferences.getBytes(LMIC_BYTES_KEY_NAME, &data, sizeof(data));
  return data;
}
