#ifndef IO_H
#define IO_H

#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Arduino.h"

//---------------------------------------------//
//                   Led Class                 //
//---------------------------------------------//
class Led {
  private:
    uint8_t pin;
  public:
    Led() : pin(255) {} // Default constructor
    Led(uint8_t pin) {
      this->pin = pin;
      pinMode(this->pin, OUTPUT);
      digitalWrite(this->pin, LOW);
    }
    uint8_t getPin() const;
    void setPin(uint8_t pin);
    void on();
    void off();
    void blink(uint16_t ms);
};

//---------------------------------------------//
//                 Buzzer Class                //
//---------------------------------------------//
class Buzzer {
  private:
    uint8_t pin;
  public:
    Buzzer() : pin(255) {} // Default constructor
    Buzzer(uint8_t pin) {
      this->pin = pin;
      pinMode(this->pin, OUTPUT);
      digitalWrite(this->pin, LOW);
    }
    uint8_t getPin() const;
    void setPin(uint8_t pin);
    void on(uint16_t hz);
    void off();
    void beep(uint16_t hz, uint16_t ms);
};

//---------------------------------------------//
//                 Button Class                //
//---------------------------------------------//
class Button {
  private:
    uint8_t pin;
  public:
    Button() : pin(255) {} // Default constructor
    Button(uint8_t pin) {
      this->pin = pin;
      pinMode(this->pin, INPUT);
    }
    uint8_t getPin() const;
    void setPin(uint8_t pin);
    void on();
    void off();
    bool isPressed() const;
};

//---------------------------------------------//
//                Keypad4x4 Class              //
//---------------------------------------------//
class Keypad4x4 {
  private:
    char keys[4][4] = {
      {'1','2','3', 'A'},
      {'4','5','6', 'B'},
      {'7','8','9', 'C'},
      {'*','0','#', 'D'}
    };
    Keypad keypad;
  public:
    Keypad4x4() : keypad(makeKeymap(keys), nullptr, nullptr, 4, 4) {} // Default constructor
    Keypad4x4(const uint8_t rowPins[], const uint8_t columnPins[]) :
      keypad(makeKeymap(keys), const_cast<uint8_t*>(rowPins), const_cast<uint8_t*>(columnPins), 4, 4)
    {
    }
    void setPins(const uint8_t rowPins[], const uint8_t columnPins[]);
    Keypad getKeypad() const;
    char getKey();
    String getString(char terminator);
    String getString(char terminator, char delKey);
};

//---------------------------------------------//
//                LCD Class                    //
//---------------------------------------------//
class Lcd16x2I2C {
  private:
    LiquidCrystal_I2C lcd;
    uint8_t findAddress() {
      uint8_t addresses[] = {0x27, 0x3F};
      for (uint8_t i = 0; i < sizeof(addresses); i++) {
        Wire.beginTransmission(addresses[i]);
        if (Wire.endTransmission() == 0) {
          return addresses[i];
        }
      }
      return 0x27;
    }
  public:
    Lcd16x2I2C() : lcd(findAddress(), 16, 2) { // Default constructor
      lcd.begin(16, 2);
      lcd.backlight();
    }
    Lcd16x2I2C(uint8_t cols, uint8_t rows)
      : lcd(findAddress(), cols, rows) {
      lcd.begin(cols, rows);
      lcd.backlight();
    }
    void print(const String& text);
    void clear();
    void setCursor(uint8_t col, uint8_t row);
};

#endif