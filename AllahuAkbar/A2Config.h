// A2Config.h
#ifndef A2BOARD_H
#define A2BOARD_H

#include "IOManager.h" // Include the IOManager for peripheral classes

// --- A2Config Class ---
// Manages global configuration and access to peripheral instances.
// Uses static members to provide a singleton-like access pattern for peripherals.
class A2Config {
  protected:
    // Static instances of peripheral classes.
    static Led         ledInstance;
    static Buzzer      buzzerInstance;
    static Keypad4x4   keypadInstance;
    static Lcd16x2I2C  lcd16x2I2CInstance;

    // Flags to track if each peripheral has been configured.
    static bool        ledConfigured;
    static bool        buzzerConfigured;
    static bool        keypadConfigured;
    static bool        lcd16x2I2CConfigured;

    // Internal helper functions for debug and info messages.
    static void A2MessageDEBUG(String msg);
    static void A2MessageINFO(String component ,String msg);

  public:
    // --- Peripheral Configuration Methods ---
    // These methods provide access to the static peripheral instances and
    // allow for their initial configuration (e.g., setting pins, dimensions).

    // Get the Led instance, optionally configuring its pin.
    static Led&        led(uint8_t pin);
    // Get the Buzzer instance, optionally configuring its pin.
    static Buzzer&     buzzer(uint8_t pin);
    // Get the Keypad4x4 instance, configuring its row and column pins.
    static Keypad4x4&  keypad(const uint8_t rowPins[], const uint8_t columnPins[]);
    // Get the Lcd16x2I2C instance, optionally configuring its columns and rows.
    // This method also calls begin() internally.
    static Lcd16x2I2C& lcd16x2I2C(uint8_t cols = 16, uint8_t rows = 2); // Added default values
    
    // New: Get the already configured Lcd16x2I2C instance without re-configuring.
    static Lcd16x2I2C& getLcd(); 

    // --- System Management Methods ---
    // Provides debugging output (baudRate defaults to 9600).
    static void        DEBUG(int baudRate = 9600);
    // Runs a system self-test.
    static void        test(int baudRate = 9600);
    // Performs system startup routines.
    static void        startup();
    // Performs system shutdown routines.
    static void        shutdown();
    // Resets the system.
    static void        reset();
};

#endif