#include "A2Driver.h"
#include "A2Kernel.h"

// --- Static Member Initialization (Existing) ---
Led A2Kernel::_A2LedPins[MAX_PINS];
size_t A2Kernel::_ledCount = 0;
bool A2Kernel::_isLedInitialized = false;

Buzzer A2Kernel::_A2BuzzerPins[MAX_PINS];
size_t A2Kernel::_buzzerCount = 0;
bool A2Kernel::_isBuzzerInitialized = false;

Lcd16x2I2C A2Kernel::_A2Lcd16x2I2C;
bool A2Kernel::_isLcdInitialized = false;

// --- Static Member Initialization (NEW) ---
Joystick A2Kernel::_A2Joystick;
bool A2Kernel::_isJoystickInitialized = false;

Display* A2Kernel::_A2Display = nullptr; // Initialize pointer to null
bool A2Kernel::_isDisplayInitialized = false;


// --- Debugging Functions (Existing) ---
void A2Kernel::DEBUG_MESSAGE(const char *message) {
    Serial.print("[DEBUG]: ");
    Serial.println(message);
}

void A2Kernel::DEBUG_COMPONENT(const String &cmp, const String& message) {
    Serial.print("[" + cmp + "]: ");
    Serial.println(message);
}

void A2Kernel::DEBUG_KERNEL(const char *message) {
    DEBUG_COMPONENT("A2KERNEL", message);
}

void A2Kernel::DEBUG_DRIVER(const char *message) {
    DEBUG_COMPONENT("A2DRIVER", message);
}
