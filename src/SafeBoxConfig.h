#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// --- Wi-Fi Access Point Settings ---
#define AP_SSID "SafeBox_Config"
#define AP_PASS "12345678"

// --- Hardware Pin Definitions (ESP32 30-Pin) ---

// Keypad Pins
#define KEYPAD_R1 32
#define KEYPAD_R2 33
#define KEYPAD_R3 25
#define KEYPAD_R4 26
#define KEYPAD_C1 27
#define KEYPAD_C2 14
#define KEYPAD_C3 12
#define KEYPAD_C4 13

// I2C Pins for LCD
#define LCD_SDA 21
#define LCD_SCL 22

// Component Pins
#define SERVO_PIN 4
#define GREEN_LED_PIN 23
#define RED_LED_PIN 19
#define BUZZER_PIN 5

// Fingerprint Sensor Pins (using Serial2)
// Connect sensor's TX to ESP32's RX2 (GPIO 16 is default, but we use KEYPAD_C1)
// Connect sensor's RX to ESP32's TX2 (GPIO 17 is default, but we use KEYPAD_R4)
// IMPORTANT: The fingerprint sensor and keypad share some default pins.
// We've already assigned pins, so we just need to make sure Serial2 uses them.
// We will define Serial2 on pins 32(RX) and 33(TX) to avoid conflicts.
#define FINGERPRINT_RX 16
#define FINGERPRINT_TX 17

// --- Security & Timing Settings ---
#define DEFAULT_PASSWORD "1234"
#define MAX_PASSWORD_LENGTH 8
#define MAX_ATTEMPTS 3
#define LOCK_TIME 30000 // 30 seconds lockout time

// --- Servo Settings ---
#define SERVO_LOCKED_ANGLE 180
#define SERVO_UNLOCKED_ANGLE 0

// --- Fingerprint Settings ---
#define MAX_FINGERPRINTS 2 // Maximum number of fingerprints to store

#endif // CONFIG_H
