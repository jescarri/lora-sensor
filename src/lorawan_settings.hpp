#ifndef LORAWAN_SETTINGS_HPP_
#define LORAWAN_SETTINGS_HPP_
#include "secrets/keys.hpp"
#include <Preferences.h>
#include <lmic.h>

#include <hal/hal.h>

#define LMIC_PREF_NS_NAME "lmic"
#define LMIC_INIT_NEEDED_KEY_NAME "init"
#define LMIC_BYTES_KEY_NAME "lmic_struct"
#define LORAWAN_CONFIG_PRESENT_KEY "lorawan_config"
#define APP_EUID_KEY "euid"
#define MAX_LORAWAN_CONF_CHAR_LEN 100

#define RW_MODE false
#define RO_MODE true

extern lmic_t SETTINGS_LMIC;
extern Preferences lorawan_preferences;
extern lmic_t LMIC;
void lorawan_preferences_init();
void lmic_save();
bool lmic_init_needed();
void load_lmic();
void resetLmic();
bool lorawanConfigPresent();

void saveAppEUID(char app_euid[]);
u1_t *getAppEUID();

#endif
