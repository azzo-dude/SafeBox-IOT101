IOManager Arduino Library
A custom Arduino library providing easy-to-use classes for controlling LEDs, buzzers, buttons, 4x4 keypads, and I2C LCDs, plus a configuration manager for easy hardware setup.

⚠️ Note:
This library does not detect whether hardware is physically connected. Status messages only indicate that a class instance is initialized and configured in software—not that the hardware is present or working.

Features
Led: Control digital output LEDs (on, off, blink).

Buzzer: Control a buzzer with frequency and duration (on, off, beep).

Button: Read and control digital input buttons (isPressed).

Keypad4x4: Read input from a 4x4 matrix keypad (readKey, readStringBlocking, readStringNonBlocking).

Lcd16x2I2C: Display text on a 16x2 I2C LCD (print, clear, setCursor, scrollUp, scrollDown, printf).

A2Config: Static configuration manager for easy hardware setup and status reporting.

Getting Started
Installation
Method 1: Add Library via .zip

Download or clone this repository, or copy IOManager.h and IOManager.cpp (and A2Config.h/cpp if needed) into a folder named IOManager.

Compress the folder into a .zip file (e.g., IOManager.zip).

In the Arduino IDE, go to Sketch > Include Library > Add .ZIP Library...

Select your IOManager.zip file.

Method 2: Manual Installation

Copy IOManager.h and IOManager.cpp (and A2Config.h/cpp if needed) into your Arduino project's folder or into Documents/Arduino/libraries/IOManager.

Dependencies
Keypad

LiquidCrystal_I2C

Usage Example
#include "A2Config.h" // For the A2Config manager
#include "IOManager.h" // For individual peripheral classes

// Define pins for your hardware
uint8_t ledPin = 11;
uint8_t buzzerPin = 13;
uint8_t buttonPin = 10;
uint8_t rowPins[] = {9, 8, 7, 6}; // Connect to R1, R2, R3, R4 of keypad
uint8_t columnPins[] = {5, 4, 3, 2}; // Connect to C1, C2, C3, C4 of keypad

// Create instances of your peripherals
// It's recommended to use A2Config for setup in a real application
// but direct instantiation is shown here for demonstration of IOManager classes.
Led led(ledPin);
Buzzer buzzer(buzzerPin);
Button button(buttonPin);
Keypad4x4 keypad(rowPins, columnPins);
Lcd16x2I2C lcd; // Default to 16x2, address will be auto-detected

void setup() {
    Serial.begin(9600); // Initialize serial communication for debugging

    // Initialize LCD (important for auto-detection of I2C address)
    lcd.begin(); 
    // Check if LCD was configured successfully
    if (!lcd.isConfigured()) {
      Serial.println("LCD not found or not configured!");
    }

    Serial.println("--- Starting IOManager Example ---");

    // Test Led
    Serial.println("Testing LED...");
    led.on();
    delay(500);
    led.off();
    delay(500);
    led.blink(500); // Blinks twice (on/off, then on/off)
    Serial.println("LED test complete.");

    // Test Buzzer
    Serial.println("Testing Buzzer...");
    buzzer.on(3200); // Turn on at 3200 Hz
    delay(200);
    buzzer.off();
    for (int i = 0; i < 3; i++) {
        buzzer.beep(340, 500); // Beep at 340 Hz for 500ms
        delay(500);
    }
    buzzer.beep(3000, 300); // Final beep
    Serial.println("Buzzer test complete.");

    // Test Keypad (Blocking read example)
    Serial.println("Press keys on the keypad (end with #):");
    String input = keypad.readStringBlocking('#', '*'); // '*' as delete key
    Serial.print("Keypad input: ");
    Serial.println(input);

    // Test Button (initial state)
    if (button.isPressed()) {
        Serial.println("Button is pressed initially.");
    } else {
        Serial.println("Button is not pressed initially.");
    }

    // Test LCD
    Serial.println("Testing LCD...");
    if (lcd.isConfigured()) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Hello, LCD!");
      lcd.setCursor(0, 1);
      lcd.print("A2Config Ready");
      lcd.display(); // Ensure buffer is rendered
      delay(2000);
      lcd.clear();
      lcd.printf("Formatted: %d", 123); // Test printf
      lcd.display();
      delay(2000);
      lcd.clear();
    }
    Serial.println("LCD test complete.");
    Serial.println("--- IOManager Example Finished Setup ---");
}

void loop() {
    // Continuous check for button press
    if (button.isPressed()) {
        Serial.println("Button pressed!");
        led.on(); // Turn LED on when button is pressed
    } else {
        led.off(); // Turn LED off when button is not pressed
    }

    // Non-blocking keypad read example (can be used in loop)
    char key = keypad.readKey();
    if (key != NO_KEY) {
        Serial.print("Key pressed (non-blocking): ");
        Serial.println(key);
        // You could add logic here to update LCD or perform actions based on key
    }

    // Example of using A2Config for system-wide tests
    // A2Config::test(); // Uncomment to run a system test in loop (will block)
}

API Reference
Led
Led(uint8_t pin): Constructor. Initializes the LED on the given pin.

void on(): Turns the LED on (HIGH).

void off(): Turns the LED off (LOW).

void blink(uint16_t ms): Blinks the LED for ms milliseconds (blocking).

uint8_t getPin() const: Returns the configured pin number.

void setPin(uint8_t pin): Sets the pin and configures it as OUTPUT.

Buzzer
Buzzer(uint8_t pin): Constructor. Initializes the buzzer on the given pin.

void on(uint16_t hz): Turns the buzzer on at the specified frequency (hz).

void off(): Turns the buzzer off.

void beep(uint16_t hz, uint16_t ms): Makes the buzzer beep at hz frequency for ms milliseconds (blocking).

uint8_t getPin() const: Returns the configured pin number.

void setPin(uint8_t pin): Sets the pin and configures it as OUTPUT.

Button
Button(uint8_t pin): Constructor. Initializes the button on the given pin with internal pull-up.

bool isPressed(): Returns true if the button is pressed (debounced), false otherwise.

uint8_t getPin() const: Returns the configured pin number.

void setPin(uint8_t pin): Sets the pin and configures it as INPUT_PULLUP.

Keypad4x4
Keypad4x4(const uint8_t rowPins[], const uint8_t columnPins[]): Constructor. Configures the keypad with row and column pins.

char readKey(): Reads a single key press (non-blocking). Returns NO_KEY if no key is pressed.

String readStringBlocking(char terminator, char delKey): Reads a string from the keypad, blocking until the terminator key is pressed. delKey can be used to delete the last character.

String readStringNonBlocking(char terminator): Reads a string from the keypad non-blocking. Returns the accumulated string when terminator is pressed, otherwise returns an empty string. The internal buffer is reset upon terminator press.

void setPins(const uint8_t rowPins[], const uint8_t columnPins[]): Sets the row and column pins for the keypad.

const Keypad& getKeypad() const: Returns a constant reference to the internal Keypad object.

bool isConfigured() const: Returns true if the keypad has been configured.

Lcd16x2I2C
Lcd16x2I2C(): Default constructor. Initializes with common 16x2 dimensions.

Lcd16x2I2C(uint8_t cols, uint8_t rows): Constructor with custom dimensions.

void begin(): Initializes the LCD: finds I2C address, calls lcd.init(), and turns on backlight.

void setDimensions(uint8_t cols, uint8_t rows): Sets the dimensions (columns and rows) of the LCD. Call begin() after this if changing dimensions after initial setup.

void print(const String& text): Prints text to the internal buffer. Automatically scrolls vertically if the buffer exceeds rows.

template <typename... Args> void printf(const char* format, Args... args): Formatted print function (like printf), outputting to the internal buffer.

void clear(): Clears the internal buffer and the physical LCD. Resets scroll indices.

void clearBuffer(): Clears only the internal text buffer, leaving the physical LCD as is.

void setCursor(uint8_t col, uint8_t row): Sets the physical LCD cursor position for direct writes (bypasses buffer).

void home(): Moves the physical LCD cursor to home position (0,0).

bool isConfigured() const: Returns true if the LCD is configured and ready for use.

void scrollUp(): Scrolls the buffered text up by one line.

void scrollDown(): Scrolls the buffered text down by one line.

void scrollRight(): Scrolls the buffered text right by one character.

void scrollLeft(): Scrolls the buffered text left by one character.

void display(): Displays the current content of the buffer on the physical LCD.

uint8_t getCols() const: Returns the number of columns on the LCD.

uint8_t getRows() const: Returns the number of rows on the LCD.

void cursor(): Turns on the block LCD cursor.

void noCursor(): Hides the LCD cursor.

void blink(): Turns on the blinking LCD cursor.

void noBlink(): Turns off the blinking LCD cursor.

uint8_t findAddress(): Internal helper to find the I2C address of the LCD.

A2Config (Static Class)
static Led& led(uint8_t pin): Configures and returns the Led instance.

static Buzzer& buzzer(uint8_t pin): Configures and returns the Buzzer instance.

static Keypad4x4& keypad(const uint8_t rowPins[], const uint8_t columnPins[]): Configures and returns the Keypad4x4 instance.

static Lcd16x2I2C& lcd16x2I2C(uint8_t cols = 16, uint8_t rows = 2): Configures and returns the Lcd16x2I2C instance. It also calls begin() internally.

static Lcd16x2I2C& getLcd(): Returns the already configured Lcd16x2I2C instance without re-configuring.

static void DEBUG(int baudRate = 9600): Initializes Serial communication and prints configuration status for all peripherals.

static void test(int baudRate = 9600): Runs a system self-test, including basic tests for configured peripherals.

static void startup(): Placeholder for system startup routines.

static void shutdown(): Placeholder for system shutdown routines.

static void reset(): Resets the system by calling shutdown() then startup().

Notes
All classes print debug information to the Serial Monitor for easier troubleshooting.

Make sure your hardware wiring matches the pin numbers used in your code.

For keypad and LCD, ensure you use the correct I2C address and pin mapping. The Lcd16x2I2C class attempts to auto-detect common I2C addresses.

The A2Config class provides a convenient way to set up and check the status of all your hardware devices.

This library does not detect hardware presence—status messages only reflect software configuration.
