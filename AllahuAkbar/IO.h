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
    int pin;
  public:
    Led(int pin) {
      this->pin = pin;
      pinMode(this->pin, OUTPUT);
      digitalWrite(this->pin, LOW);
    }
    int getPin();
    void setPin(int pin);
    void on();
    void off();
    void blink(int ms);
};

//---------------------------------------------//
//                 Buzzer Class                //
//---------------------------------------------//
class Buzzer {
  private:
    int pin;
  public:
    Buzzer(int pin) {
      this->pin = pin;
      pinMode(this->pin, OUTPUT);
      digitalWrite(this->pin, LOW);
    }
    int getPin();
    void setPin(int pin);
    void on(int hz);
    void off();
    void beep(int hz, int ms);
};

//---------------------------------------------//
//                 Button Class                //
//---------------------------------------------//
class Button {
  private:
    int pin;
  public:
    Button(int pin) {
      this->pin = pin;
      pinMode(this->pin, INPUT);
    }
    int getPin();
    void setPin(int pin);
    void on();
    void off();
    bool isPressed();
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
    Keypad4x4(const byte rowPins[], const byte columnPins[]) :
      keypad(makeKeymap(keys), const_cast<byte*>(rowPins), const_cast<byte*>(columnPins), 4, 4)
    {
      Serial.println("Keypad initialized");
    }
    void setPins(const byte rowPins[], const byte columnPins[]);
    Keypad getKeypad();
    char getKey();
    String getString(char terminator);
};
#endif