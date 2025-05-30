#ifndef IO_H
#define IO_H

#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Arduino.h" // For Arduino specific functions like pinMode, digitalWrite, delay, millis, etc.

// --- LED Class ---
// Encapsulates LED control (on, off, blink).
class Led {
  private:
    uint8_t pin; // The Arduino pin connected to the LED

  public:
    // Default constructor: initializes pin to an invalid value.
    Led() : pin(255) {}
    // Constructor with pin: sets the pin and configures it as OUTPUT.
    Led(uint8_t pin) {
      setPin(pin); // Use setter to ensure pinMode and initial state are set
    }

    // Get the configured pin.
    uint8_t getPin() const;
    // Set the pin and configure it as OUTPUT, setting initial state to LOW.
    void    setPin(uint8_t pin);
    // Turn the LED on (HIGH).
    void    on();
    // Turn the LED off (LOW).
    void    off();
    // Blink the LED for a specified duration (blocking).
    void    blink(uint16_t ms);
};

// --- Buzzer Class ---
// Encapsulates Buzzer control (on, off, beep).
class Buzzer {
  private:
    uint8_t pin; // The Arduino pin connected to the Buzzer

  public:
    // Default constructor: initializes pin to an invalid value.
    Buzzer() : pin(255) {}
    // Constructor with pin: sets the pin and configures it as OUTPUT.
    Buzzer(uint8_t pin) {
      setPin(pin); // Use setter to ensure pinMode and initial state are set
    }

    // Get the configured pin.
    uint8_t getPin() const;
    // Set the pin and configure it as OUTPUT, setting initial state to LOW.
    void    setPin(uint8_t pin);
    // Turn the buzzer on with a specified frequency (tone).
    void    on(uint16_t hz);
    // Turn the buzzer off (noTone).
    void    off();
    // Make the buzzer beep for a specified duration (blocking).
    void    beep(uint16_t hz, uint16_t ms);
};

// --- Button Class ---
// Encapsulates Button control with debounce.
class Button {
  private:
    uint8_t       pin;             // The Arduino pin connected to the button
    bool          lastButtonState; // Last read state of the button
    unsigned long lastDebounceTime; // Last time the button state changed
    const unsigned long debounceDelay = 50; // Debounce time in milliseconds

  public:
    // Default constructor.
    Button() : pin(255), lastButtonState(LOW), lastDebounceTime(0) {}
    // Constructor with pin: sets the pin and configures it as INPUT_PULLUP.
    Button(uint8_t pin) {
      setPin(pin); // Use setter to ensure pinMode and initial state are set
    }

    // Get the configured pin.
    uint8_t getPin() const;
    // Set the pin and configure it as INPUT_PULLUP.
    void    setPin(uint8_t pin);
    // Check if the button is currently pressed (debounced).
    bool    isPressed();
};

// --- Keypad4x4 Class ---
// Encapsulates 4x4 Keypad control.
class Keypad4x4 {
  private:
    // Key mapping for a standard 4x4 keypad.
    char keys[4][4] = {
      {'1','2','3', 'A'},
      {'4','5','6', 'B'},
      {'7','8','9', 'C'},
      {'*','0','#', 'D'}
    };
    Keypad keypad; // Keypad library instance
    String currentInputString; // Buffer for non-blocking string input
    bool _isConfigured; // Flag to indicate if the keypad is configured

  public:
    // Default constructor: initializes keypad with dummy pins.
    Keypad4x4() : keypad(makeKeymap(keys), nullptr, nullptr, 4, 4), _isConfigured(false) {}
    // Constructor with row and column pins: configures the keypad.
    Keypad4x4(const uint8_t rowPins[], const uint8_t columnPins[]) :
      keypad(makeKeymap(keys), (uint8_t*)rowPins, (uint8_t*)columnPins, 4, 4), _isConfigured(true) {
      currentInputString = "";
    }

    // Set the row and column pins for the keypad.
    void   setPins(const uint8_t rowPins[], const uint8_t columnPins[]);
    // Get the internal Keypad object (use with caution, for advanced access).
    const Keypad& getKeypad() const; // Changed to return const reference for efficiency
    // Read a single key press (non-blocking). Returns NO_KEY if no key is pressed.
    char   readKey();
    // Read a string from the keypad, blocking until terminator or delete key is pressed.
    String readStringBlocking(char terminator, char delKey);
    // Read a string from the keypad non-blocking. Returns accumulated string when terminator is pressed.
    String readStringNonBlocking(char terminator);
    // Check if the keypad is configured.
    bool isConfigured() const;
};

// --- Lcd16x2I2C Class ---
// Encapsulates 16x2 I2C LCD control with text buffering and scrolling.
class Lcd16x2I2C {
  private:
    LiquidCrystal_I2C lcd; // LiquidCrystal_I2C library instance
    uint8_t _cols;         // Number of columns on the LCD
    uint8_t _rows;         // Number of rows on the LCD
    uint8_t _address;      // I2C address of the LCD
    bool _isConfigured;    // Flag to indicate if the LCD is configured
    String buffer;         // Internal buffer to hold all text
    int scrollVerticalIndex; // Current top line index for vertical scrolling
    int scrollHorizontalIndex; // Current horizontal offset for visible lines

    // Internal helper to render the buffer to the physical LCD.
    void renderBufferToLcd();

  public:
    // Internal helper to find the I2C address of the LCD.
    uint8_t findAddress(); // This was public in your original, keeping it that way

    // Default constructor: initializes with common 16x2 dimensions and dummy address.
    Lcd16x2I2C() : lcd(0x00, 16, 2), _cols(16), _rows(2), _address(0x00), _isConfigured(false), buffer(""), scrollVerticalIndex(0), scrollHorizontalIndex(0) {}
    // Constructor with custom dimensions: initializes with specified dimensions.
    Lcd16x2I2C(uint8_t cols, uint8_t rows) : lcd(0x00, cols, rows), _cols(cols), _rows(rows), _address(0x00), _isConfigured(false), buffer(""), scrollVerticalIndex(0), scrollHorizontalIndex(0) {}

    // Initialize the LCD: finds address, calls lcd.begin(), and turns on backlight.
    void begin();
    // Set the dimensions (columns and rows) of the LCD.
    void setDimensions(uint8_t cols, uint8_t rows);
    // Print text to the internal buffer. Automatically scrolls vertically if buffer exceeds rows.
    void print(const String& text); // This function will be modified in .cpp
    // Clear the internal buffer and the physical LCD. Resets scroll indices.
    void clear();
    // Set the physical LCD cursor position for direct writes (bypasses buffer).
    void setCursor(uint8_t col, uint8_t row);
    // Move the physical LCD cursor to home position (0,0).
    void home();
    // Check if the LCD is configured and ready for use.
    bool isConfigured() const;
    // Scroll the buffered text up by one line.
    void scrollUp();
    // Scroll the buffered text down by one line.
    void scrollDown();
    // Scroll the buffered text right by one character.
    void scrollRight();
    // Scroll the buffered text left by one character.
    void scrollLeft();
    // Display the current content of the buffer on the physical LCD.
    void display();
    // Clear only the internal text buffer, leaving the physical LCD as is.
    void clearBuffer();

    // Get the number of columns on the LCD.
    uint8_t getCols() const { return _cols; }
    // Get the number of rows on the LCD.
    uint8_t getRows() const { return _rows; }

    // Control the visibility of the LCD cursor (block).
    void cursor();
    // Hide the LCD cursor.
    void noCursor();
    // Turn on the blinking LCD cursor.
    void blink();
    // Turn off the blinking LCD cursor.
    void noBlink();

    // Formatted print function (like printf).
    template <typename... Args>
    void printf(const char* format, Args... args) {
        char tempBuffer[256]; // A temporary buffer for formatted string
        snprintf(tempBuffer, sizeof(tempBuffer), format, args...);
        print(String(tempBuffer));
    }
};

#endif