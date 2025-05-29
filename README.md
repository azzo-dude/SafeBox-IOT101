# IOManager Arduino Library

A custom Arduino library providing easy-to-use classes for controlling LEDs, buzzers, buttons, 4x4 keypads, and I2C LCDs, plus a configuration manager for easy hardware setup.

> **⚠️ Note:**  
> This library does **not** detect whether hardware is physically connected.  
> Status messages only indicate that a class instance is initialized and configured in software—not that the hardware is present or working.

---

## Features

- **Led**: Control digital output LEDs (`on`, `off`, `blink`).
- **Buzzer**: Control a buzzer with frequency and duration (`on`, `off`, `beep`).
- **Button**: Read and control digital input buttons (`isPressed`, `on`, `off`).
- **Keypad4x4**: Read input from a 4x4 matrix keypad (`getKey`, `getString`).
- **Lcd16x2I2C**: Display text on a 16x2 I2C LCD (`print`, `clear`, `setCursor`).
- **A2Config**: Static configuration manager for easy hardware setup and status reporting.

---

## Getting Started

### Installation

**Method 1: Add Library via .zip**
1. Download or clone this repository, or copy `IOManager.h` and `IOManager.cpp` (and `A2Config.h/cpp` if needed) into a folder named `IOManager`.
2. Compress the folder into a `.zip` file (e.g., `IOManager.zip`).
3. In the Arduino IDE, go to **Sketch > Include Library > Add .ZIP Library...**
4. Select your `IOManager.zip` file.

**Method 2: Manual Installation**
- Copy `IOManager.h` and `IOManager.cpp` (and `A2Config.h/cpp` if needed) into your Arduino project's folder or into `Documents/Arduino/libraries/IOManager`.

---

### Dependencies

- [Keypad](https://github.com/Chris--A/Keypad)
- [LiquidCrystal_I2C](https://github.com/johnrickman/LiquidCrystal_I2C)

---

### Usage Example

```cpp
#include "A2Config.h"
#include "IOManager.h"

uint8_t ledPin = 11;
uint8_t buzzerPin = 13;
uint8_t buttonPin = 10;
uint8_t rowPins[] = {9, 8, 7, 6};
uint8_t columnPins[] = {5, 4, 3, 2};

Led led(ledPin);
Buzzer buzzer(buzzerPin);
Button button(buttonPin);
Keypad4x4 keypad(rowPins, columnPins);
Lcd16x2I2C lcd;

void setup() {
    Serial.begin(9600);

    // Test Led
    led.on();
    delay(500);
    led.off();
    delay(500);
    led.blink(500);

    // Test Buzzer
    buzzer.on(3200);
    delay(200);
    buzzer.off();
    for (int i = 0; i < 3; i++) {
        buzzer.beep(340, 500);
        delay(500);
    }
    buzzer.beep(3000, 300);

    // Test Keypad
    Serial.println("Press keys on the keypad (end with #):");
    String input = keypad.getString('#');
    Serial.print("Keypad input: ");
    Serial.println(input);

    // Test LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Hello, LCD!");
    lcd.setCursor(0, 1);
    lcd.print("A2Config Ready");
    Serial.println("LCD test complete.");
}

void loop() {
    // Optionally, you can add more runtime tests here
}
```

---

## API Reference

### Led
- `Led(uint8_t pin)`
- `void on()`
- `void off()`
- `void blink(uint16_t ms)`
- `uint8_t getPin() const`
- `void setPin(uint8_t pin)`

### Buzzer
- `Buzzer(uint8_t pin)`
- `void on(uint16_t hz)`
- `void off()`
- `void beep(uint16_t hz, uint16_t ms)`
- `uint8_t getPin() const`
- `void setPin(uint8_t pin)`

### Button
- `Button(uint8_t pin)`
- `bool isPressed() const`
- `void on()`
- `void off()`
- `uint8_t getPin() const`
- `void setPin(uint8_t pin)`

### Keypad4x4
- `Keypad4x4(const uint8_t rowPins[], const uint8_t columnPins[])`
- `char getKey()`
- `String getString(char terminator)`
- `void setPins(const uint8_t rowPins[], const uint8_t columnPins[])`
- `Keypad getKeypad() const`

### Lcd16x2I2C
- `Lcd16x2I2C()`
- `Lcd16x2I2C(uint8_t cols, uint8_t rows)`
- `void print(const String& text)`
- `void clear()`
- `void setCursor(uint8_t col, uint8_t row)`

### A2Config
- `static Led ledSetup(uint8_t pin)`
- `static Buzzer buzzerSetup(uint8_t pin)`
- `static Keypad4x4 keypadSetup(const uint8_t rowPins[], const uint8_t columnPins[])`
- `static void init(int baudRate = 9600)`

---

## Notes

- All classes print debug information to the Serial Monitor for easier troubleshooting.
- Make sure your hardware wiring matches the pin numbers used in your code.
- For keypad and LCD, ensure you use the correct I2C address and pin mapping.
- The `A2Config` class provides a convenient way to set up and check the status of all your hardware devices.
- **This library does not detect hardware presence—status messages only reflect software configuration.**

---
