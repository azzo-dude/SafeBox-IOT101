#ifndef A2DRIVER_H
#define A2DRIVER_H

#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Arduino.h"
#include <Adafruit_GFX.h>

// --- Led Class ---
class Led {
  private:
    uint8_t _pin;
    bool    _isConfigured = false;
  public:
    Led() : _pin(255), _isConfigured(false) {}
    Led(uint8_t pin) { setPin(pin); _isConfigured = true; }
    uint8_t getPin() const;
    void setPin(uint8_t pin);
    bool isConfigured() const;
    void on();
    void off();
    void blink(uint16_t ms);
};

#endif
