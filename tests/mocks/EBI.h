// Host-side stub of the EBI LoRaWAN library, recording calls for assertions.
#pragma once
#include "Arduino.h"

enum EBI_Status { EBIES_Success = 0, EBIES_Failure = 1 };
enum EBI_Class { ClassA, ClassC };
enum EBI_JoinMode { ABP, OTAA };

class EBI_LoRaWAN {
 public:
  uint8_t AppSKey[16] = {0};
  uint8_t NetSKey[16] = {0};
  uint8_t DevAddress[4] = {0};
  EBI_Class lclass = ClassA;
  EBI_JoinMode joinMode = OTAA;
  int transmitPower = 0;
  bool ackEnabled = true, dutyCycleEnabled = true, adrEnabled = false;

  int beginFailures = 0, connectFailures = 0;
  EBI_Status sendResult = EBIES_Success;
  std::vector<std::pair<uint32_t, int>> sent;  // (value, fport)
  std::vector<uint8_t> downlink;

  EBI_Status begin() { return beginFailures-- > 0 ? EBIES_Failure : EBIES_Success; }
  EBI_Status connect() { return connectFailures-- > 0 ? EBIES_Failure : EBIES_Success; }
  void disableAck() { ackEnabled = false; }
  void disableDutyCycle() { dutyCycleEnabled = false; }
  void enableADR() { adrEnabled = true; }
  EBI_Status sendInt(uint32_t v, int port) { sent.push_back({v, port}); return sendResult; }
  int getLastTxAckRSSI() { return -42; }
  int getLastRxRSSI() { return -77; }
  int available(int) { return (int)downlink.size(); }
  int readBytes(uint8_t* buf, int n) {
    for (int k = 0; k < n; k++) buf[k] = downlink[k];
    downlink.clear();
    return n;
  }
};
