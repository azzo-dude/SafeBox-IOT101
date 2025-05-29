#include "IOManager.h"

//---------------------------------------------//
//               Led Class Methods             //
//---------------------------------------------//
uint8_t Led::getPin() const {
    return this->pin;
}

void Led::setPin(uint8_t pin) {
  this->pin = pin;
}

void Led::on() {
  digitalWrite(getPin(), HIGH);
}

void Led::off() {
  digitalWrite(getPin(), LOW);
}

void Led::blink(uint16_t ms) {
  on();
  delay(ms);
  off();
  delay(ms);
}

//---------------------------------------------//
//             Buzzer Class Methods            //
//---------------------------------------------//
uint8_t Buzzer::getPin() const {
  return this->pin;
}

void Buzzer::setPin(uint8_t pin) {
  this->pin = pin;
}

void Buzzer::on(uint16_t hz) {
  tone(this->getPin(), hz);
}

void Buzzer::off() {
  noTone(this->getPin());
}

void Buzzer::beep(uint16_t hz, uint16_t ms) {
  on(hz);
  delay(ms);
  off();
  delay(ms);
}

//---------------------------------------------//
//              Button Class Methods           //
//---------------------------------------------//
uint8_t Button::getPin() const {
  return this->pin;
}

void Button::setPin(uint8_t pin) {
  this->pin = pin;
}

void Button::on() {
  digitalWrite(getPin(), HIGH);
}

void Button::off() {
  digitalWrite(getPin(), LOW);
}

bool Button::isPressed() const {
  return digitalRead(getPin()) == HIGH;
}

//---------------------------------------------//
//             Keypad4x4 Class Methods         //
//---------------------------------------------//
Keypad Keypad4x4::getKeypad() const {
  return this->keypad;
}

char Keypad4x4::getKey() {
  char key = keypad.getKey();
  if (key) {
    Serial.print("[DEBUG] Input: ");
    Serial.println(key);
  }
  return key;
}

String Keypad4x4::getString(char terminator) {
  String input = "";
  while (true) {
    char key = keypad.getKey();
    if (key) {
      Serial.print("[DEBUG] Input: ");
      Serial.println(key);
      if (key == terminator) {
        return input;
      }
      input += key;
    }
  }
}

void Keypad4x4::setPins(const uint8_t rowPins[], const uint8_t columnPins[]) {
    keypad = Keypad(makeKeymap(keys), const_cast<uint8_t*>(rowPins), const_cast<uint8_t*>(columnPins), 4, 4);
}

//---------------------------------------------//
//             Lcd16x2I2C Class Methods        //
//---------------------------------------------//
void Lcd16x2I2C::print(const String& text) {
    lcd.print(text);
}

void Lcd16x2I2C::setCursor(uint8_t col, uint8_t row) {
    lcd.setCursor(col, row);
}

void Lcd16x2I2C::clear() {
    lcd.clear();
}