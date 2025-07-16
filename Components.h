#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <Arduino.h>

//Led Component
class Led {
private:
  uint8_t pin_;

public:
  // Constructor
  Led(uint8_t pin);

  // Methods
  uint8_t getPin() const;
  void setPin(uint8_t pin);
  void on();
  void off();
  void blink(uint16_t ms);
};


//Buzzer Component
class Buzzer {
private:
  uint8_t pin_;

public:
  // Constructor
  Buzzer(uint8_t pin);

  // Methods
  uint8_t getPin() const;
  void setPin(uint8_t pin);
  void tone(int16_t hz);
  void tone(int16_t hz, int64_t dur);
  void noTone();
};

#endif