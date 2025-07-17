#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <Arduino.h>
#include <Keypad.h>
#include <Adafruit_Fingerprint.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//================================================================================
// Led Component
//================================================================================
class Led {
private:
  uint8_t pin_;

public:
  Led(uint8_t pin);
  uint8_t getPin() const;
  void setPin(uint8_t pin);
  void on();
  void off();
  void blink(uint64_t ms);
  void blink(uint64_t ms, uint64_t count);
};

//================================================================================
// Buzzer Component
//================================================================================
class Buzzer {
private:
  uint8_t pin_;

public:
  Buzzer(uint8_t pin);
  uint8_t getPin() const;
  void setPin(uint8_t pin);
  void tone(int16_t hz);
  void tone(int16_t hz, int64_t dur);
  void noTone();
};

//
================================================================================
// Keypad 3x4 Component
//
================================================================================
class Keypad3x4 {
private:
  Keypad* keypad_;
  const static byte ROWS = 4;
  const static byte COLS = 3;
  char keys_[ROWS][COLS];
  byte rowPins_[ROWS];
  byte colPins_[COLS];

public:
  Keypad3x4(byte rowPins[ROWS], byte colPins[COLS]);
  ~Keypad3x4();

  char getKey();
};

//================================================================================
// AS608 Fingerprint Sensor Component
//================================================================================
class AS608 {
private:
    Adafruit_Fingerprint* finger_;

public:
    AS608(HardwareSerial* serial);
    ~AS608();

    bool begin();
    bool verifyPassword();
    int16_t getFingerprintID();
};

//================================================================================
// NFC MFRC522 Reader/Writer Component
//================================================================================
class NFC_MFRC522 {
private:
    MFRC522* mfrc522_;
    uint8_t ssPin_;
    uint8_t rstPin_;

public:
    NFC_MFRC522(uint8_t ssPin, uint8_t rstPin);
    ~NFC_MFRC522();

    void begin();
    bool isNewCardPresent();
    String readCardSerial();
};

//================================================================================
// Lcd16x2I2C Component
//================================================================================
class Lcd16x2I2C {
private:
    LiquidCrystal_I2C* lcd_;
    uint8_t address_;
    uint8_t cols_;
    uint8_t rows_;

public:
    Lcd16x2I2C(uint8_t address, uint8_t cols, uint8_t rows);
    ~Lcd16x2I2C();

    void begin();
    void clear();
    void backlight();
    void noBacklight();
    void setCursor(uint8_t col, uint8_t row);
    void print(const String& text);
    void print(const String& text, uint8_t col, uint8_t row);
};

#endif // COMPONENTS_H
