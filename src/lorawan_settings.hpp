#ifndef LORAWAN_SETTINGS_HPP_
#define LORAWAN_SETTINGS_HPP_
#include "secrets/keys.hpp"
#include <Preferences.h>
#include <lmic.h>

#include <hal/hal.h>

#define LMIC_PREF_NS_NAME "lmic"
#define LMIC_INIT_NEEDED_KEY_NAME "init"
#define LMIC_BYTES_KEY_NAME "lmic_struct"
#define RW_MODE false
#define RO_MODE true

extern lmic_t SETTINGS_LMIC;
extern Preferences lorawan_preferences;

void lorawan_preferences_init();
void lmic_save(const lmic_t &value);
bool lmic_init_needed();
lmic_t load_lmic();
#endif
