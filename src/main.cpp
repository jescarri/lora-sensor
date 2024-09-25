#include <Preferences.h>
#include <lmic.h>

#include "lorawan.hpp"
#include "lorawan_settings.hpp"
#include <hal/hal.h>

#define VCC_ENA_PIN 13
#define START_WEB_CONFIG_PIN 16
#define SOIL_SENSOR_PIN 34 // ADC1_CH6
#define MAX_SENSOR_READ 10

lmic_t SETTINGS_LMIC;
Preferences lorawan_preferences;
extern CayenneLPP lpp(MAX_PAYLOAD_SIZE);

// Active in LOW, normal operation is HIGH
bool startWebConfig = false;

// This needs to come from calibration data.
const int AirValue = 790;
const int WaterValue = 390;

sensorData sd;

static const u1_t PROGMEM APPEUI[8] = TTN_APPEUI;
static const u1_t PROGMEM DEVEUI[8] = TTN_DEVEUI;
static const u1_t PROGMEM APPKEY[16] = TTN_APPKEY;
void os_getArtEui(u1_t *buf) { memcpy_P(buf, APPEUI, 8); }
void os_getDevEui(u1_t *buf) { memcpy_P(buf, DEVEUI, 8); }
void os_getDevKey(u1_t *buf) { memcpy_P(buf, APPKEY, 16); }
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

  // analogSetCycles(20);
  // analogSetSamples(10);
  // analogSetAttenuation(ADC_11db);

  Serial.begin(115200);

  startWebConfig = !digitalRead(START_WEB_CONFIG_PIN);
  Serial.print("Webconf status: ");
  Serial.println(startWebConfig);
  // Read the sensors
  ReadSensors();
  lorawan_preferences_init();
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
    unsigned long now = millis();

    LMIC.globalDutyAvail = 0; // - ((now / 1000.0 + TX_INTERVAL) * 1000);
    if (LMIC.globalDutyAvail < 0) {
      LMIC.globalDutyAvail = 0;
    }
    lmic_save();
    GoDeepSleep();
  } else if (lastPrintTime + 2000 < millis()) {
    Serial.print(F("Cannot sleep "));
    Serial.print(F("TimeCriticalJobs: "));
    Serial.print(timeCriticalJobs);
    Serial.print(" ");

    LoraWANPrintLMICOpmode();
    PrintRuntime();
    lastPrintTime = millis();
  }
  if (millis() - entry > 60000) {
    // Serial.println("Reset LMIC");
    // LMIC.seqnoUp = 0;
    unsigned long now = millis();

    LMIC.globalDutyAvail = 0; //- ((now / 1000.0 + TX_INTERVAL) * 1000);
    if (LMIC.globalDutyAvail < 0) {
      LMIC.globalDutyAvail = 0;
    }

    lmic_save();
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
  digitalWrite(VCC_ENA_PIN, LOW);
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
  Serial.println(F("Go DeepSleep"));
  PrintRuntime();
  Serial.flush();
  esp_sleep_enable_timer_wakeup(TX_INTERVAL * 1000000);
  esp_deep_sleep_start();
}

void ReadSensors() {
  sd.soilMoisturePercentage = 0;
  sd.soilMoistureValue = 0;
  sd.vBat = 0;
  for (int i = 0; i < MAX_SENSOR_READ; i++) {
    sd.soilMoistureValue += analogRead(SOIL_SENSOR_PIN);
  }
  sd.soilMoistureValue = sd.soilMoistureValue / 10;
  sd.soilMoisturePercentage =
      map(sd.soilMoistureValue, AirValue, WaterValue, 0, 100);
}
