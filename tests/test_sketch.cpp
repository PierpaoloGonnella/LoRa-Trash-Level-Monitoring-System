// Host-side unit tests for IoT_SmartTrashCan.ino.
// The sketch is compiled unmodified against the mocks in tests/mocks/.
#include "Arduino.h"
#include "../IoT_SmartTrashCan.ino"

#include <cstring>
#include <iostream>

static int failures = 0, checks = 0;
#define CHECK(cond)                                                     \
  do {                                                                  \
    ++checks;                                                           \
    if (!(cond)) {                                                      \
      ++failures;                                                       \
      std::cerr << __FILE__ << ":" << __LINE__ << " FAILED: " #cond "\n"; \
    }                                                                   \
  } while (0)

static void resetAll() {
  mock::reset();
  hEbi = EBI_LoRaWAN();
  state = 1;
  i = 0;
  distance = 0;
  duration = 0;
}

// distance [cm] -> echo duration [us], inverse of distance = duration*0.034/2
static unsigned long usFor(int cm) { return (unsigned long)(cm * 2 / 0.034) + 1; }

static void expectLeds(int blue, int green, int red) {
  CHECK(mock::pinWrites()[blueLed] == blue);
  CHECK(mock::pinWrites()[greenLed] == green);
  CHECK(mock::pinWrites()[redLed] == red);
}

static void test_distance_conversion() {
  resetAll();
  mock::pulseValue() = 1000;  // 1000 us round trip -> 17 cm
  CHECK(measureThrashLevel() == 17);
  mock::pulseValue() = 0;
  CHECK(measureThrashLevel() == 0);
  mock::pulseValue() = 5882;  // ~1 m
  CHECK(measureThrashLevel() == 99);
  CHECK(mock::pinWrites()[trigPin] == LOW);  // trigger left low after the pulse
}

static void test_levels_and_leds() {
  resetAll();
  mock::pulseValue() = usFor(60); measureThrashLevel(); expectLeds(LOW, LOW, LOW);   // empty
  mock::pulseValue() = usFor(46); measureThrashLevel(); expectLeds(LOW, LOW, LOW);   // boundary, not < 46
  mock::pulseValue() = usFor(30); measureThrashLevel(); expectLeds(HIGH, LOW, LOW);  // level 1
  mock::pulseValue() = usFor(23); measureThrashLevel(); expectLeds(HIGH, LOW, LOW);  // boundary
  mock::pulseValue() = usFor(15); measureThrashLevel(); expectLeds(LOW, HIGH, LOW);  // level 2
  mock::pulseValue() = usFor(5);  measureThrashLevel(); expectLeds(LOW, LOW, HIGH);  // level 3 full
}

static void test_touch_sensor_toggle() {
  resetAll();
  mock::pinInputs()[TOUCH_SENSOR] = 0;
  CHECK(readTouchSensor() == false);  // closed, no touch: do not read
  mock::pinInputs()[TOUCH_SENSOR] = 1;
  CHECK(readTouchSensor() == true);  // touch toggles state 1 -> 0
  CHECK(mock::serialOut().find("Can is now OPEN") != std::string::npos);
  mock::pinInputs()[TOUCH_SENSOR] = 0;
  CHECK(readTouchSensor() == true);  // state persists without touch
  mock::pinInputs()[TOUCH_SENSOR] = 1;
  CHECK(readTouchSensor() == false);  // toggles back
}

static void test_setup_configures_lora() {
  resetAll();
  hEbi.beginFailures = 2;
  hEbi.connectFailures = 3;
  setup();
  CHECK(std::memcmp(hEbi.AppSKey, AppSKey, 16) == 0);
  CHECK(std::memcmp(hEbi.NetSKey, NetSKey, 16) == 0);
  CHECK(std::memcmp(hEbi.DevAddress, DevAddress, 4) == 0);
  CHECK(hEbi.lclass == ClassC);
  CHECK(hEbi.joinMode == ABP);
  CHECK(hEbi.transmitPower == 14);
  CHECK(!hEbi.ackEnabled && !hEbi.dutyCycleEnabled && hEbi.adrEnabled);
  CHECK(hEbi.beginFailures < 0 && hEbi.connectFailures < 0);  // retried until success
}

static void test_send_data_payload_byteswapped() {
  resetAll();
  sendData(50);
  CHECK(hEbi.sent.size() == 1);
  CHECK(hEbi.sent[0].first == 0x32000000u);  // 50 byte-swapped
  CHECK(hEbi.sent[0].second == 6);           // FPort 6
  CHECK(mock::serialOut().find("Success!") != std::string::npos);
}

static void test_send_data_failure_and_downlink() {
  resetAll();
  hEbi.sendResult = EBIES_Failure;
  hEbi.downlink = {0x0A, 0xFF};
  sendData(1);
  CHECK(mock::serialOut().find("Failed") != std::string::npos);
  CHECK(rxBuffer[0] == 0x0A && rxBuffer[1] == 0xFF);
  CHECK(mock::serialOut().find("0A FF") != std::string::npos);  // zero-padded hex dump
}

static void test_loop_averages_and_sends_after_100_samples() {
  resetAll();
  mock::pulseValue() = usFor(23);
  mock::pinInputs()[TOUCH_SENSOR] = 1;
  loop();  // opens the can -> sample 1
  mock::pinInputs()[TOUCH_SENSOR] = 0;
  for (int k = 0; k < 98; k++) loop();
  CHECK(hEbi.sent.empty());
  CHECK(i == 99);
  loop();  // 100th sample triggers transmission
  CHECK(i == 0);
  CHECK(hEbi.sent.size() == 1);
  // avg 23 cm -> (1 - 23/46) * 100 = 50 %
  CHECK(hEbi.sent.size() == 1 && hEbi.sent[0].first == __builtin_bswap32(50));
}

static void test_loop_idle_when_closed() {
  resetAll();
  mock::pinInputs()[TOUCH_SENSOR] = 0;
  mock::pulseValue() = usFor(10);
  for (int k = 0; k < 200; k++) loop();
  CHECK(i == 0);
  CHECK(hEbi.sent.empty());
}

int main() {
  test_distance_conversion();
  test_levels_and_leds();
  test_touch_sensor_toggle();
  test_setup_configures_lora();
  test_send_data_payload_byteswapped();
  test_send_data_failure_and_downlink();
  test_loop_averages_and_sends_after_100_samples();
  test_loop_idle_when_closed();
  std::cout << (checks - failures) << "/" << checks << " checks passed\n";
  return failures ? 1 : 0;
}
