// Minimal host-side Arduino API mock used only by the unit tests.
#pragma once
#include <cstdint>
#include <cstdio>
#include <map>
#include <string>
#include <vector>

typedef bool boolean;
#define HIGH 1
#define LOW 0
#define INPUT 0
#define OUTPUT 1
#define HEX 16
#define BIN 2
#define DEC 10

namespace mock {
inline std::map<int, int>& pinWrites() { static std::map<int, int> m; return m; }
inline std::map<int, int>& pinInputs() { static std::map<int, int> m; return m; }
inline unsigned long& pulseValue() { static unsigned long v = 0; return v; }
inline std::string& serialOut() { static std::string s; return s; }
inline void reset() {
  pinWrites().clear();
  pinInputs().clear();
  pulseValue() = 0;
  serialOut().clear();
}
}  // namespace mock

inline void pinMode(int, int) {}
inline void digitalWrite(int pin, int v) { mock::pinWrites()[pin] = v; }
inline int digitalRead(int pin) { return mock::pinInputs()[pin]; }
inline void delay(unsigned long) {}
inline void delayMicroseconds(unsigned int) {}
inline unsigned long pulseIn(int, int) { return mock::pulseValue(); }

struct MockSerial {
  void begin(long) {}
  void print(const char* s) { mock::serialOut() += s; }
  void println(const char* s) { print(s); nl(); }
  void print(double d) { char b[64]; snprintf(b, sizeof b, "%.2f", d); mock::serialOut() += b; }
  void println(double d) { print(d); nl(); }
  void print(long v) { mock::serialOut() += std::to_string(v); }
  void println(long v) { print(v); nl(); }
  void print(int v) { print((long)v); }
  void println(int v) { println((long)v); }
  void print(unsigned long long v, int base) { appendBase(v, base); }
  void println(unsigned long long v, int base) { appendBase(v, base); nl(); }
  void println() { nl(); }

 private:
  void nl() { mock::serialOut() += "\n"; }
  void appendBase(unsigned long long v, int base) {
    static const char digits[] = "0123456789ABCDEF";
    std::string s;
    if (v == 0) s = "0";
    while (v) { s.insert(s.begin(), digits[v % base]); v /= base; }
    mock::serialOut() += s;
  }
};
inline MockSerial Serial;
