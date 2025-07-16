#include "Components.h"

//Led Component

// Constructor: initializes the pin and sets its mode
Led::Led(uint8_t pin) : pin_(pin) {
  pinMode(pin_, OUTPUT);
  digitalWrite(pin_, LOW);
}

// Getter for the pin number
uint8_t Led::getPin() const {
  return pin_;
}

// Setter to change the pin
void Led::setPin(uint8_t pin) {
  pin_ = pin;
  pinMode(pin_, OUTPUT);
  digitalWrite(pin_, LOW);
}

// Turns the LED on
void Led::on() {
  digitalWrite(pin_, HIGH);
}

// Turns the LED off
void Led::off() {
  digitalWrite(pin_, LOW);
}

// Blinks the LED at a given interval
void Led::blink(uint16_t ms) {
  static unsigned long previousMillis = 0;
  static bool ledState = false;
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= ms) {
    ledState = !ledState;
    digitalWrite(pin_, ledState ? HIGH : LOW);
    previousMillis = currentMillis;
  }
}

//Buzzer Component

// Constructor: initializes the pin and sets its mode
Buzzer::Buzzer(uint8_t pin) : _pin(pin) {
  pinMode(pin_, OUTPUT);
  digitalWrite(pin_, LOW);
}

// Getter for the pin number
uint8_t Buzzer::getPin() const {
  return pin_;
}

// Setter to change the pin
void Buzzer::setPin(uint8_t pin) {
  pin_ = pin;
  pinMode(pin_, OUTPUT);
  digitalWrite(pin_, LOW);
}

// Plays a tone with a specific frequency
void Buzzer::tone(int16_t hz) {
  ::tone(pin_, hz);
}

// Plays a tone with a specific frequency and duration
void Buzzer::tone(int16_t hz, int64_t dur) {
  ::tone(pin_, hz, dur);
}

// Stops the tone
void Buzzer::noTone() {
  ::noTone(pin_);
}