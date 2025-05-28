# AllahuAkbar Arduino Library

A custom Arduino library providing easy-to-use classes for controlling LEDs, buzzers, buttons, 4x4 keypads, and I2C LCDs.

---

## Features

- **Led**: Control digital output LEDs (on, off, blink).
- **Buzzer**: Control a buzzer with frequency and duration (on, off, beep).
- **Button**: Read and control digital input buttons (isPressed, on, off).
- **Keypad4x4**: Read input from a 4x4 matrix keypad (getKey, getString).
- **Lcd16x2I2C**: Display text on a 16x2 I2C LCD (print, clear, setCursor, home).

---

## Installation

1. Copy `IO.h` and `IO.cpp` into your Arduino project's folder or library directory.
2. Make sure you have the following dependencies installed via Library Manager:
   - [Keypad](https://github.com/Chris--A/Keypad)
   - [LiquidCrystal_I2C](https://github.com/johnrickman/LiquidCrystal_I2C)

---

## Usage Example

```cpp
#include "IO.h"

const byte rowPins[] = {2, 3, 4, 5};
const byte colPins[] = {6, 7, 8, 9};
Led led(10);
Buzzer buzzer(11);
Button button(12);
Keypad4x4 keypad(rowPins, colPins);
Lcd16x2I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);
  lcd.print("System Ready!");
}

void loop() {
  // Keypad input as string, end with '#'
  String input = keypad.getString('#');
  lcd.clear();
  lcd.print("You: " + input);

  // Blink LED and beep buzzer
  led.blink(200);
  buzzer.beep(1000, 200);

  // Button check
  if (button.isPressed()) {
    lcd.clear();
    lcd.print("Button pressed!");
    delay(1000);
    lcd.clear();
  }

  delay(500);
}
```

---

## API Reference

### Led

- `Led(int pin)`
- `void on()`
- `void off()`
- `void blink(int ms)`
- `int getPin()`
- `void setPin(int pin)`

### Buzzer

- `Buzzer(int pin)`
- `void on(int hz)`
- `void off()`
- `void beep(int hz, int ms)`
- `int getPin()`
- `void setPin(int pin)`

### Button

- `Button(int pin)`
- `bool isPressed()`
- `void on()`
- `void off()`
- `int getPin()`
- `void setPin(int pin)`

### Keypad4x4

- `Keypad4x4(const byte rowPins[], const byte columnPins[])`
- `char getKey()`  
  Returns the last key pressed, or 0 if none.
- `String getString(char terminator)`  
  Reads a string from the keypad until the terminator key is pressed.
- `void setPins(const byte rowPins[], const byte columnPins[])`
- `Keypad getKeypad()`

### Lcd16x2I2C

- `Lcd16x2I2C(uint8_t address, uint8_t cols, uint8_t rows)`
- `void print(const String& text)`
- `void clear()`
- `void setCursor(int col, int row)`
- `void home()`

---

## Notes

- All classes print debug information to the Serial Monitor for easier troubleshooting.
- Make sure your hardware wiring matches the pin numbers used in your code.
- For keypad and LCD, ensure you use the correct I2C address and pin mapping.

---

## License

MIT License
