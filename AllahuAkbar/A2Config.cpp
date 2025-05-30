// A2Config.cpp
#include "A2Config.h"
#include "Arduino.h" // Required for Serial, String, etc.
#include <Wire.h>    // Required for I2C communication (used by Lcd16x2I2C)

// --- Static Member Definitions ---
// Initialize static instances of peripheral classes.
Led A2Config::ledInstance;
Buzzer A2Config::buzzerInstance;
Keypad4x4 A2Config::keypadInstance;
Lcd16x2I2C A2Config::lcd16x2I2CInstance;

// Initialize static configuration flags to false.
bool A2Config::ledConfigured = false;
bool A2Config::buzzerConfigured = false;
bool A2Config::keypadConfigured = false;
bool A2Config::lcd16x2I2CConfigured = false;

// --- Peripheral Configuration Method Implementations ---

// Configures and returns the Led instance.
Led& A2Config::led(uint8_t pin) {
  ledInstance.setPin(pin); // Set the LED pin
  ledConfigured = true;    // Mark LED as configured
  return ledInstance;
}

// Configures and returns the Buzzer instance.
Buzzer& A2Config::buzzer(uint8_t pin) {
  buzzerInstance.setPin(pin); // Set the Buzzer pin
  buzzerConfigured = true;    // Mark Buzzer as configured
  return buzzerInstance;
}

// Configures and returns the Keypad4x4 instance.
Keypad4x4& A2Config::keypad(const uint8_t rowPins[], const uint8_t columnPins[]) {
  keypadInstance.setPins(rowPins, columnPins); // Set keypad pins
  keypadConfigured = true;                     // Mark keypad as configured
  return keypadInstance;
}

// Configures and returns the Lcd16x2I2C instance.
Lcd16x2I2C& A2Config::lcd16x2I2C(uint8_t cols, uint8_t rows) {
  // Set the dimensions for the LCD instance.
  // This must be done before calling begin() if different from default constructor.
  lcd16x2I2CInstance.setDimensions(cols, rows);
  lcd16x2I2CInstance.begin(); // Attempt to initialize the LCD
  lcd16x2I2CConfigured = lcd16x2I2CInstance.isConfigured(); // Update flag based on success
  return lcd16x2I2CInstance;
}

// Get the already configured Lcd16x2I2C instance.
Lcd16x2I2C& A2Config::getLcd() {
    return lcd16x2I2CInstance;
}

// --- System Management Method Implementations ---

// Initializes Serial communication and prints configuration status.
void A2Config::DEBUG(int baudRate) {
  Serial.begin(baudRate);
  A2MessageDEBUG("Debugging started at " + String(baudRate) + " baud.");

  // Report status for each peripheral
  if (ledConfigured) {
    A2MessageINFO("LED", "LED is configured on pin " + String(ledInstance.getPin()));
  } else {
    A2MessageINFO("LED", "LED is not configured");
  }

  if (buzzerConfigured) {
    A2MessageINFO("Buzzer", "Buzzer is configured on pin " + String(buzzerInstance.getPin()));
  } else {
    A2MessageINFO("Buzzer", "Buzzer is not configured");
  }

  if (keypadConfigured) {
    A2MessageINFO("Keypad", "Keypad is configured.");
    // Optionally, you can add code here to print keypad pin configuration if needed for debugging.
    // This would require Keypad4x4 to store and expose its row/column pins.
  } else {
    A2MessageINFO("Keypad", "Keypad is not configured");
  }

  if (lcd16x2I2CConfigured) {
    A2MessageINFO("LCD", "LCD is configured with address 0x" + String(lcd16x2I2CInstance.findAddress(), HEX));
  } else {
    A2MessageINFO("LCD", "LCD is not configured. Check wiring and address.");
  }
}

// Internal helper for debug messages.
void A2Config::A2MessageDEBUG(String msg) {
    Serial.print("[DEBUG] ");
    Serial.println(msg);
}

// Internal helper for info messages.
void A2Config::A2MessageINFO(String component, String msg) {
    Serial.print("[INFO] [");
    Serial.print(component);
    Serial.print("] ");
    Serial.println(msg);
}

// Placeholder for system self-test.
void A2Config::test(int baudRate) {
    DEBUG(baudRate); // Ensure debug is active for test output
    A2MessageINFO("System", "Running system test...");
    
    // Example test: Blink LED if configured
    if (ledConfigured) {
        A2MessageINFO("LED Test", "Blinking LED...");
        ledInstance.blink(200); // Fast blink
        ledInstance.blink(200);
    } else {
        A2MessageINFO("LED Test", "LED not configured, skipping test.");
    }

    // Example test: Beep buzzer if configured
    if (buzzerConfigured) {
        A2MessageINFO("Buzzer Test", "Beeping buzzer...");
        buzzerInstance.beep(3200, 1000); // 1kHz beep
        buzzerInstance.beep(1500, 1000);
    } else {
        A2MessageINFO("Buzzer Test", "Buzzer not configured, skipping test.");
    }

    // Example test: Display message on LCD if configured
    if (lcd16x2I2CConfigured) {
        A2MessageINFO("LCD Test", "Displaying test message...");
        lcd16x2I2CInstance.clear();
        lcd16x2I2CInstance.print("LCD Test OK!"); // Use print, which handles buffering
        // lcd16x2I2CInstance.display(); // print() already calls display() internally
        delay(1000);
        lcd16x2I2CInstance.clear();
    } else {
        A2MessageINFO("LCD Test", "LCD not configured, skipping test.");
    }

    A2MessageINFO("System", "System test finished.");
}

// Placeholder for system startup routines.
void A2Config::startup() {
    A2MessageINFO("System", "Performing startup routines...");
    // Add any necessary initialization steps here, e.g., setting initial states
    // for peripherals, loading configurations, etc.
}

// Placeholder for system shutdown routines.
void A2Config::shutdown() {
    A2MessageINFO("System", "Performing shutdown routines...");
    // Add any necessary cleanup steps here, e.g., turning off all LEDs,
    // stopping buzzers, clearing displays, saving state.
    if (ledConfigured) ledInstance.off();
    if (buzzerConfigured) buzzerInstance.off();
    if (lcd16x2I2CConfigured) lcd16x2I2CInstance.clear();
}

// Placeholder for system reset.
void A2Config::reset() {
    A2MessageINFO("System", "Initiating system reset...");
    shutdown();
    startup();
    A2MessageINFO("System", "System reset complete.");
}