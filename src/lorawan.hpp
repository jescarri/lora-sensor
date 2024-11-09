#ifndef LORAWAN_HPP_
#define LORAWAN_HPP_
#define PIN_LMIC_NSS 18
#define PIN_LMIC_RST 14
#define PIN_LMIC_DIO0 26
#define PIN_LMIC_DIO1 33
#define PIN_LMIC_DIO2 32
#define MAX_PAYLOAD_SIZE 51
#define MAX_SENSOR_READ 1

#define SOIL_SENSOR_PIN 34 // ADC1_CH6
const int AirValue = 2550;
const int WaterValue = 1100;
#include <Arduino.h>
#include <lmic.h>

#include <CayenneLPP.h>
#include <hal/hal.h>

struct sensorData {
  float soilMoisturePercentage;
  float soilMoistureValue;
  float vBat;
};

extern const lmic_pinmap lmic_pins;
extern volatile bool enableSleep_;
extern osjob_t sendjob;

extern CayenneLPP lpp;

void LoraWANPrintLMICOpmode(void);
void LoraWANDebug(lmic_t lmic_check);
void PrintLMICVersion();
void onEvent(ev_t ev);
void do_send(osjob_t *j);
void ReadSensors();

void os_getArtEui(u1_t *buf);
void os_getDevEui(u1_t *buf);
void os_getDevKey(u1_t *buf);

#endif
