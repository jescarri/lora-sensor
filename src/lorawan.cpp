#include "lorawan.hpp"

void LoraWANPrintLMICOpmode(void) {
  Serial.print(F("LMIC.opmode: "));
  if (LMIC.opmode & OP_NONE) {
    Serial.print(F("OP_NONE "));
  }
  if (LMIC.opmode & OP_SCAN) {
    Serial.print(F("OP_SCAN "));
  }
  if (LMIC.opmode & OP_TRACK) {
    Serial.print(F("OP_TRACK "));
  }
  if (LMIC.opmode & OP_JOINING) {
    Serial.print(F("OP_JOINING "));
  }
  if (LMIC.opmode & OP_TXDATA) {
    Serial.print(F("OP_TXDATA "));
  }
  if (LMIC.opmode & OP_POLL) {
    Serial.print(F("OP_POLL "));
  }
  if (LMIC.opmode & OP_REJOIN) {
    Serial.print(F("OP_REJOIN "));
  }
  if (LMIC.opmode & OP_SHUTDOWN) {
    Serial.print(F("OP_SHUTDOWN "));
  }
  if (LMIC.opmode & OP_TXRXPEND) {
    Serial.print(F("OP_TXRXPEND "));
  }
  if (LMIC.opmode & OP_RNDTX) {
    Serial.print(F("OP_RNDTX "));
  }
  if (LMIC.opmode & OP_PINGINI) {
    Serial.print(F("OP_PINGINI "));
  }
  if (LMIC.opmode & OP_PINGABLE) {
    Serial.print(F("OP_PINGABLE "));
  }
  if (LMIC.opmode & OP_NEXTCHNL) {
    Serial.print(F("OP_NEXTCHNL "));
  }
  if (LMIC.opmode & OP_LINKDEAD) {
    Serial.print(F("OP_LINKDEAD "));
  }
  if (LMIC.opmode & OP_LINKDEAD) {
    Serial.print(F("OP_LINKDEAD "));
  }
  if (LMIC.opmode & OP_TESTMODE) {
    Serial.print(F("OP_TESTMODE "));
  }
  if (LMIC.opmode & OP_UNJOIN) {
    Serial.print(F("OP_UNJOIN "));
  }
}

void LoraWANDebug(lmic_t lmic_check) {

  LoraWANPrintLMICOpmode();
  Serial.println("");
  Serial.println("-----");

  Serial.print(F("LMIC.seqnoUp = "));
  Serial.println(lmic_check.seqnoUp);

  Serial.print(F("LMIC.globalDutyRate = "));
  Serial.print(lmic_check.globalDutyRate);
  Serial.print(F(" osTicks, "));
  Serial.print(osticks2ms(lmic_check.globalDutyRate) / 1000);
  Serial.println(F(" sec"));

  Serial.print(F("LMIC.globalDutyAvail = "));
  Serial.print(lmic_check.globalDutyAvail);
  Serial.print(F(" osTicks, "));
  Serial.print(osticks2ms(lmic_check.globalDutyAvail) / 1000);
  Serial.println(F(" sec"));

  Serial.print(F("LMICbandplan_nextTx = "));
  Serial.print(LMICbandplan_nextTx(os_getTime()));
  Serial.print(F(" osTicks, "));
  Serial.print(osticks2ms(LMICbandplan_nextTx(os_getTime())) / 1000);
  Serial.println(F(" sec"));

  Serial.print(F("os_getTime = "));
  Serial.print(os_getTime());
  Serial.print(F(" osTicks, "));
  Serial.print(osticks2ms(os_getTime()) / 1000);
  Serial.println(F(" sec"));

  Serial.print(F("LMIC.txend = "));
  Serial.println(lmic_check.txend);
  Serial.print(F("LMIC.txChnl = "));
  Serial.println(lmic_check.txChnl);

  Serial.println("");
  Serial.println("");
}

void PrintLMICVersion() {
  Serial.print(F("LMIC: "));
  Serial.print(ARDUINO_LMIC_VERSION_GET_MAJOR(ARDUINO_LMIC_VERSION));
  Serial.print(F("."));
  Serial.print(ARDUINO_LMIC_VERSION_GET_MINOR(ARDUINO_LMIC_VERSION));
  Serial.print(F("."));
  Serial.print(ARDUINO_LMIC_VERSION_GET_PATCH(ARDUINO_LMIC_VERSION));
  Serial.print(F("."));
  Serial.println(ARDUINO_LMIC_VERSION_GET_LOCAL(ARDUINO_LMIC_VERSION));
}

void onEvent(ev_t ev) {
  Serial.print(os_getTime());
  Serial.print(": ");
  switch (ev) {
  case EV_SCAN_TIMEOUT:
    Serial.println(F("EV_SCAN_TIMEOUT"));
    break;
  case EV_BEACON_FOUND:
    Serial.println(F("EV_BEACON_FOUND"));
    break;
  case EV_BEACON_MISSED:
    Serial.println(F("EV_BEACON_MISSED"));
    break;
  case EV_BEACON_TRACKED:
    Serial.println(F("EV_BEACON_TRACKED"));
    break;
  case EV_JOINING:
    Serial.println(F("EV_JOINING"));
    break;
  case EV_JOINED:
    Serial.println(F("EV_JOINED"));
    {
      u4_t netid = 0;
      devaddr_t devaddr = 0;
      u1_t nwkKey[16];
      u1_t artKey[16];
      LMIC_getSessionKeys(&netid, &devaddr, nwkKey, artKey);
      Serial.print("netid: ");
      Serial.println(netid, DEC);
      Serial.print("devaddr: ");
      Serial.println(devaddr, HEX);
      Serial.print("artKey: ");
      for (size_t i = 0; i < sizeof(artKey); ++i) {
        Serial.print(artKey[i], HEX);
      }
      Serial.println("");
      Serial.print("nwkKey: ");
      for (size_t i = 0; i < sizeof(nwkKey); ++i) {
        Serial.print(nwkKey[i], HEX);
      }
      Serial.println("");
    }
    // Disable link check validation (automatically enabled
    // during join, but because slow data rates change max TX
    // size, we don't use it in this example.
    LMIC_setLinkCheckMode(0);
    break;
  case EV_JOIN_FAILED:
    Serial.println(F("EV_JOIN_FAILED"));
    break;
  case EV_REJOIN_FAILED:
    Serial.println(F("EV_REJOIN_FAILED"));
    break;
  case EV_TXCOMPLETE:
    // Transmit completed, includes waiting for RX windows.
    Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
    // setTxIndicatorsOn(false);
    // printEvent(timestamp, ev);
    // printFrameCounters();
    // Check if downlink was received
    if (LMIC.dataLen != 0 || LMIC.dataBeg != 0) {
      uint8_t fPort = 0;
      if (LMIC.txrxFlags & TXRX_PORT) {
        fPort = LMIC.frame[LMIC.dataBeg - 1];
      }
      // printDownlinkInfo();
      // processDownlink(timestamp, fPort, LMIC.frame + LMIC.dataBeg,
      // LMIC.dataLen);
    }
    enableSleep_ = true;
    break;
  case EV_LOST_TSYNC:
    Serial.println(F("EV_LOST_TSYNC"));
    break;
  case EV_RESET:
    Serial.println(F("EV_RESET"));
    break;
  case EV_RXCOMPLETE:
    // data received in ping slot
    Serial.println(F("EV_RXCOMPLETE"));
    break;
  case EV_LINK_DEAD:
    Serial.println(F("EV_LINK_DEAD"));
    break;
  case EV_LINK_ALIVE:
    Serial.println(F("EV_LINK_ALIVE"));
    break;
  /*
      || This event is defined but not used in the code. No
      || point in wasting codespace on it.
      ||
      || case EV_SCAN_FOUND:
      ||    Serial.println(F("EV_SCAN_FOUND"));
      ||    break;
  */
  case EV_TXSTART:
    Serial.println(F("EV_TXSTART"));
    break;
  case EV_TXCANCELED:
    Serial.println(F("EV_TXCANCELED"));
    break;
  case EV_RXSTART:
    /* do not print anything -- it wrecks timing */
    break;
  case EV_JOIN_TXCOMPLETE:
    Serial.println(F("EV_JOIN_TXCOMPLETE: no JoinAccept"));
    break;
  default:
    Serial.print(F("Unknown event: "));
    Serial.println((unsigned)ev);
    break;
  }
}

void do_send(osjob_t *j) {
  static uint8_t mydata[] = "Test";
  lpp.reset();
  lpp.addGenericSensor(0, sd.soilMoistureValue);
  lpp.addVoltage(1, sd.vBat);
  lpp.addPercentage(2, sd.soilMoisturePercentage);
  // Check if there is not a current TX/RX job running
  Serial.println("do_send");
  Serial.print("LMIC.opmode= ");
  Serial.println(LMIC.opmode);
  if (LMIC.opmode & OP_TXRXPEND) {
    Serial.println(F("OP_TXRXPEND, not sending"));
  } else {
    // Prepare upstream data transmission at the next possible time.
    LMIC_setTxData2(1, lpp.getBuffer(), lpp.getSize(), 0);
    Serial.println(F("Packet queued"));
  }
  // Next TX is scheduled after TX_COMPLETE event.
}
