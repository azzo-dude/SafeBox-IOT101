#include "A2Config.h"

Led A2Config::led;
Buzzer A2Config::buzzer;
Keypad4x4 A2Config::keypad;

Led A2Config::ledSetup(uint8_t pin) {
    led.setPin(pin);
    return led;
}

Led A2Config::ledSetup(uint8_t pin[]) {
    led.setPin(pin[0]);
    return led;
}

Buzzer A2Config::buzzerSetup(uint8_t pin) {
    buzzer.setPin(pin);
    return buzzer;
}

Buzzer A2Config::buzzerSetup(uint8_t pin[]) {
    buzzer.setPin(pin[0]);
    return buzzer;
}

Keypad4x4 A2Config::keypadSetup(const uint8_t rowPins[], const uint8_t columnPins[]) {
    keypad.setPins(rowPins, columnPins);
    return keypad;
}

void A2Config::init(int baudRate) {
    Serial.begin(baudRate);

    const char* statusMsg[2] = { "[ NO ]", "[ OK ]" };

    // LED
    Serial.print(statusMsg[led.getPin() != 255]);
    if (led.getPin() != 255) {
        Serial.print(" Create Instance Led configured at pin ");
        Serial.println(led.getPin());
    } else {
        Serial.println(" Not configured!");
    }

    // Buzzer
    Serial.print(statusMsg[buzzer.getPin() != 255]);
    if (buzzer.getPin() != 255) {
        Serial.print(" Create Instance Buzzer Configured at pin ");
        Serial.println(buzzer.getPin());
    } else {
        Serial.println(" Not configured!");
    }

    // Keypad
    Serial.print("[ OK ] Create Instance Keypad: ");
    Serial.println("configured");
}