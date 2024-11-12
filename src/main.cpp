#include "WiFi.h"
#include "driver/adc.h"

#include <Preferences.h>
#include <lmic.h>

#include "lorawan.hpp"
#include "lorawan_settings.hpp"
#include "menu.hpp"
#include <Adafruit_MAX1704X.h>
#include <FastLED.h>
#include <Wire.h>
#include <hal/hal.h>

#define VCC_ENA_PIN 13
#define START_WEB_CONFIG_PIN 16
#define NUM_LEDS 1
#define LED_DATA_PIN 17

lmic_t SETTINGS_LMIC;
Preferences lorawan_preferences;
CayenneLPP lpp(MAX_PAYLOAD_SIZE);
Adafruit_MAX17048 maxlipo;
CRGB leds[NUM_LEDS];
bool maxLipoFound = false;

// Active in LOW, normal operation is HIGH
bool startWebConfig = false;

void PrintRuntime();
void GoDeepSleep();
void ReadSensors();

volatile bool enableSleep_ = true;
unsigned long entry;

osjob_t sendjob;

const lmic_pinmap lmic_pins = {
    .nss = PIN_LMIC_NSS,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = PIN_LMIC_RST,
    .dio = {PIN_LMIC_DIO0, PIN_LMIC_DIO1, PIN_LMIC_DIO2},
    .rxtx_rx_active = 0,
    .spi_freq = 8000000,
};

// Schedule TX every this many seconds
// Respect Fair Access Policy and Maximum Duty Cycle!
// https://www.thethingsnetwork.org/docs/lorawan/duty-cycle.html
// https://www.loratools.nl/#/airtime
const unsigned TX_INTERVAL = 3600;

void setup() {
  pinMode(VCC_ENA_PIN, OUTPUT);
  digitalWrite(VCC_ENA_PIN, HIGH);
  pinMode(START_WEB_CONFIG_PIN, INPUT);
  WiFi.mode(WIFI_OFF);
  btStop();

  Wire.begin(21, 22);
  delay(100);
  Serial.begin(115200);
  adc_power_acquire();
  randomSeed(analogRead(0));
  // Init and Wake the Lipo Cel:
  if (maxlipo.begin()) {
    // Wake up the lipo gauge
    maxlipo.wake();
    maxLipoFound = true;
  } else {
    Serial.println("No MAX14048 found");
  }
  FastLED.addLeds<WS2812B, LED_DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(50);
  lorawan_preferences_init();
  Serial.print("LMIC CONFIG Present: ");
  Serial.println(lorawanConfigPresent());
  startWebConfig = !digitalRead(START_WEB_CONFIG_PIN);
  Serial.print("Webconf status: ");
  Serial.println(startWebConfig);
  bool otaa_cfg = lorawan_preferences.isKey("ttn_otaa_config");
  Serial.print("otaa_config_done: ");
  Serial.println(otaa_cfg);
  if ((startWebConfig == true) || (!otaa_cfg)) {
    resetLmic();
    startWebConf();
  }

  PrintLMICVersion();
  // LMIC init
  os_init();

  LMIC_reset();
  if (!lmic_init_needed()) {
    Serial.println(LMIC.seqnoUp);
    Serial.println("LMIC Data found");
    load_lmic();

    Serial.println(LMIC.seqnoUp);
  }

  LoraWANDebug(LMIC);

  // Start job (sending automatically starts OTAA too)
  LMIC_selectSubBand(1);

  do_send(&sendjob);
  entry = millis();
}

void loop() {
  static unsigned long lastPrintTime = 0;
  os_runloop_once();
  const bool timeCriticalJobs =
      os_queryTimeCriticalJobs(ms2osticksRound((TX_INTERVAL * 1000)));

  if (!timeCriticalJobs && enableSleep_ == true &&
      !(LMIC.opmode & OP_TXRXPEND)) {
    Serial.print(F("Can go sleep "));
    LoraWANPrintLMICOpmode();
    lmic_save();
    GoDeepSleep();
  } else if (lastPrintTime + 1000 < millis()) {
    Serial.print(F("Cannot sleep "));
    Serial.print(F("TimeCriticalJobs: "));
    Serial.print(timeCriticalJobs);
    Serial.print(" ");

    LoraWANPrintLMICOpmode();
    PrintRuntime();
    lastPrintTime = millis();
  }
  if (millis() - entry > 60000) {
    // ToDo: Delete LMIC config from nvram
    // so it joins the wan after next run
    resetLmic();
    GoDeepSleep();
  }
}

void PrintRuntime() {
  long seconds = millis() / 1000;
  Serial.print("Runtime: ");
  Serial.print(seconds);
  Serial.println(" seconds");
}

void GoDeepSleep() {
  // Turn off the lipo gauge:
  leds[0] = CRGB::Black;
  FastLED.show();
  maxlipo.hibernate();
  digitalWrite(VCC_ENA_PIN, LOW);
  WiFi.mode(WIFI_OFF);
  btStop();
  gpio_reset_pin(GPIO_NUM_0);
  gpio_reset_pin(GPIO_NUM_2);
  gpio_reset_pin(GPIO_NUM_4);
  gpio_reset_pin(GPIO_NUM_12);
  gpio_reset_pin(GPIO_NUM_13);
  gpio_reset_pin(GPIO_NUM_14);
  gpio_reset_pin(GPIO_NUM_15);
  gpio_reset_pin(GPIO_NUM_25);
  gpio_reset_pin(GPIO_NUM_26);
  gpio_reset_pin(GPIO_NUM_27);
  gpio_reset_pin(GPIO_NUM_32);
  gpio_reset_pin(GPIO_NUM_33);
  gpio_reset_pin(GPIO_NUM_34);
  gpio_reset_pin(GPIO_NUM_35);
  gpio_reset_pin(GPIO_NUM_36);
  gpio_reset_pin(GPIO_NUM_37);
  gpio_reset_pin(GPIO_NUM_38);
  gpio_reset_pin(GPIO_NUM_39);
  adc_power_release();
  Serial.println(F("Go DeepSleep"));
  PrintRuntime();
  Serial.flush();
  esp_sleep_enable_timer_wakeup(TX_INTERVAL * 1000000);
  esp_deep_sleep_start();
}
