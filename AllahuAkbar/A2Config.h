#ifndef A2BOARD_H
#define A2BOARD_H

#include "IOManager.h"

class A2Config {
    private:
        static Led led;
        static Buzzer buzzer;
        static Keypad4x4 keypad;

    public:
        static Led ledSetup(uint8_t pin);
        static Led ledSetup(uint8_t pin[]);
        static Buzzer buzzerSetup(uint8_t pin);
        static Buzzer buzzerSetup(uint8_t pin[]);
        static Keypad4x4 keypadSetup(const uint8_t rowPins[], const uint8_t columnPins[]);
        static void init(int baudRate = 9600);
};

#endif