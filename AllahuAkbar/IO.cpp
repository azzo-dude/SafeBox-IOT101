#include "Arduino.h"
#include "IO.h"

//---------------------------------------------//
//               Led Class Methods             //
//---------------------------------------------//
int Led::getPin() {
  Serial.println("Getting pin for LED");
  return this->pin;
}

void Led::setPin(int pin) {
  Serial.println("Setting pin for LED");
  this->pin = pin;
}

void Led::on() {
  Serial.println("Turning LED on");
  digitalWrite(getPin(), HIGH);
}

void Led::off() {
  Serial.println("Turning LED off");
  digitalWrite(getPin(), LOW);
}

void Led::blink(int ms) {
  Serial.println("Blinking LED");
  on();
  delay(ms);
  off();
  delay(ms);
}

//---------------------------------------------//
//             Buzzer Class Methods            //
//---------------------------------------------//
int Buzzer::getPin() {
  Serial.println("Getting pin for Buzzer");
  return this->pin;
}

void Buzzer::setPin(int pin) {
  Serial.println("Setting pin for Buzzer");
  this->pin = pin;
}

void Buzzer::on(int hz) {
  Serial.println("Turning Buzzer on at frequency: " + String(hz) + " Hz");
  tone(this->getPin(), hz);
}

void Buzzer::off() {
  Serial.println("Turning Buzzer off");
  noTone(this->getPin());
}

void Buzzer::beep(int hz, int ms) {
  Serial.println("Beeping Buzzer at frequency: " + String(hz) + " Hz for " + String(ms) + " ms");
  on(hz);
  delay(ms);
  off();
  delay(ms);
}

//---------------------------------------------//
//              Button Class Methods           //
//---------------------------------------------//

int Button::getPin() {
  Serial.println("Getting pin for Button");
  return this->pin;
}

void Button::setPin(int pin) {
  Serial.println("Setting pin for Button");
  this->pin = pin;
}

void Button::on() {
  Serial.println("Button pressed");
  digitalWrite(getPin(), HIGH);
}

void Button::off() {
  Serial.println("Button released");
  digitalWrite(getPin(), LOW);
}

bool Button::isPressed() {
  bool pressed = digitalRead(getPin()) == HIGH;
  Serial.println("Button is " + String(pressed ? "pressed" : "not pressed"));
  return pressed;
}


//---------------------------------------------//
//             Keypad4x4 Class Methods         //
//---------------------------------------------//

Keypad Keypad4x4::getKeypad() {
  Serial.println("Getting Keypad instance");
  return this->keypad;
}

char Keypad4x4::getKey() {
  char key = keypad.getKey();
  if (key) {
    Serial.print("Key pressed: ");
    Serial.println(key);
  }
  return key;
}

String Keypad4x4::getString(char terminator) {
  String input = "";
  while (true) {
    char key = keypad.getKey();
    if (key) {
      if (key == terminator) {
        return input;
      }
      input += key;
      Serial.print("User Keypress: ");
      Serial.print(input);
    }
  }
}