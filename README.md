IOManager Arduino Library
A modular library for managing LEDs, buzzers, buttons, 4x4 keypads, I2C LCDs, and configuration—all in one place.

⚠️ Important Note
This library does not detect whether hardware is physically connected.
Status messages only confirm that the class instance is configured in software, not that the hardware is working or connected.

✨ Features
Led: Control digital LEDs — on, off, blink.

Buzzer: Play tones and beeps at custom frequency and duration.

Button: Read push-button states with built-in debounce logic.

Keypad4x4: Read from a 4x4 matrix keypad — key reads, strings (blocking/non-blocking).

Lcd16x2I2C: Print to a 16x2 I2C LCD — with auto-scrolling buffer and printf support.

A2Config: Static configuration manager for easy initialization and testing.

🚀 Getting Started
Installation
🔹 Method 1: Install via .ZIP
Clone or download this repository.

Compress the IOManager folder into IOManager.zip.

In Arduino IDE: Sketch > Include Library > Add .ZIP Library...

Select your IOManager.zip file.

🔹 Method 2: Manual Install
Copy these files into your Arduino libraries/IOManager folder:

IOManager.h, IOManager.cpp

A2Config.h, A2Config.cpp (optional but recommended)

📦 Dependencies
Make sure to install:

Keypad

LiquidCrystal_I2C

🔧 Usage Example
cpp
Copy
Edit
#include "A2Config.h"
#include "IOManager.h"

// Pin Configuration
uint8_t ledPin = 11;
uint8_t buzzerPin = 13;
uint8_t buttonPin = 10;
uint8_t rowPins[] = {9, 8, 7, 6};
uint8_t columnPins[] = {5, 4, 3, 2};

// Peripheral Instances
Led led(ledPin);
Buzzer buzzer(buzzerPin);
Button button(buttonPin);
Keypad4x4 keypad(rowPins, columnPins);
Lcd16x2I2C lcd;

void setup() {
    Serial.begin(9600);
    lcd.begin(); // Auto-detects I2C address

    if (!lcd.isConfigured()) {
        Serial.println("LCD not found or not configured!");
    }

    led.blink(500); // Quick blink
    buzzer.beep(3200, 200); // Quick beep

    String input = keypad.readStringBlocking('#', '*');
    Serial.print("Keypad input: ");
    Serial.println(input);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Hello, LCD!");
    lcd.display();
}

void loop() {
    if (button.isPressed()) {
        led.on();
    } else {
        led.off();
    }

    char key = keypad.readKey();
    if (key != NO_KEY) {
        Serial.print("Key: ");
        Serial.println(key);
    }
}
📚 API Reference
🔴 Led
cpp
Copy
Edit
Led(uint8_t pin);
void on();
void off();
void blink(uint16_t ms);
🔊 Buzzer
cpp
Copy
Edit
Buzzer(uint8_t pin);
void on(uint16_t hz);
void off();
void beep(uint16_t hz, uint16_t ms);
🔘 Button
cpp
Copy
Edit
Button(uint8_t pin);
bool isPressed();
⌨️ Keypad4x4
cpp
Copy
Edit
Keypad4x4(const uint8_t rowPins[], const uint8_t columnPins[]);
char readKey();
String readStringBlocking(char terminator, char delKey);
String readStringNonBlocking(char terminator);
📺 Lcd16x2I2C
cpp
Copy
Edit
Lcd16x2I2C();
void begin();
void print(const String& text);
void printf(const char* format, ...);
void clear();
void setCursor(uint8_t col, uint8_t row);
void display();
⚙️ A2Config (Static Helper)
cpp
Copy
Edit
static Led& led(uint8_t pin);
static Buzzer& buzzer(uint8_t pin);
static Keypad4x4& keypad(const uint8_t rowPins[], const uint8_t columnPins[]);
static Lcd16x2I2C& lcd16x2I2C(uint8_t cols = 16, uint8_t rows = 2);
static void DEBUG(int baudRate = 9600);
static void test(int baudRate = 9600);
🧠 Notes
All peripherals log status and debug messages via Serial.

Use A2Config::DEBUG() and A2Config::test() for diagnostics.

LCD I2C address is auto-detected among common values (0x27, 0x3F, etc.).

readStringBlocking() and readStringNonBlocking() give full keypad input control.

📄 License
MIT License — feel free to use, modify, and distribute.
