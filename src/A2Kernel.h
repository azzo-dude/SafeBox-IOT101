#ifndef A2KERNEL_H
#define A2KERNEL_H

#include "A2Driver.h"

#define A2KERNEL_VERSION "1.0.0"
#define MAX_PINS 255 // Max number of pins for LEDs/Buzzers

// Macros for user interaction
#define KERNEL_LED(...)       A2Kernel::___A2_DRIVER_LED(__VA_ARGS__)
#define KERNEL_BUZZER(...)    A2Kernel::___A2_DRIVER_BUZZER(__VA_ARGS__)
#define KERNEL_LCD16X2I2C()   A2Kernel::___A2_DRIVER_LCD16X2I2C()
#define KERNEL_JOYSTICK(...)  A2Kernel::___A2_DRIVER_JOYSTICK(__VA_ARGS__) // NEW
#define KERNEL_DISPLAY(...)   A2Kernel::___A2_DRIVER_DISPLAY(__VA_ARGS__)   // NEW
#define KERNEL_DEBUG_ON       A2Kernel::___A2_KERNEL_DEBUG(true)
#define KERNEL_DEBUG_OFF      A2Kernel::___A2_KERNEL_DEBUG(false)

class A2Kernel {
    protected:
        // Existing components
        static Led _A2LedPins[MAX_PINS];
        static bool      _isLedInitialized;
        static size_t    _ledCount;

        static Buzzer _A2BuzzerPins[MAX_PINS];
        static bool _isBuzzerInitialized;
        static size_t _buzzerCount;

        static Lcd16x2I2C _A2Lcd16x2I2C; // Single instance for LCD
        static bool _isLcdInitialized;

        // NEW Components
        static Joystick _A2Joystick;
        static bool _isJoystickInitialized;

        static Display* _A2Display; // Pointer to the abstract display
        static bool _isDisplayInitialized;

    public:
        // --- Existing Driver Initialization Macros ---
        template <typename... Args>
        static Led* ___A2_DRIVER_LED(Args... args) {
            uint8_t pins_val[] = { static_cast<uint8_t>(args)... };
            for (size_t i = 0; i < sizeof...(Args) && _ledCount < MAX_PINS; ++i) {
                _A2LedPins[_ledCount++] = Led(pins_val[i]);
            }
            _isLedInitialized = true;
            return _A2LedPins;
        }

        template <typename... Args>
        static Buzzer* ___A2_DRIVER_BUZZER(Args... args) {
            uint8_t pins_val[] = { static_cast<uint8_t>(args)... };
            for (size_t i = 0; i < sizeof...(Args) && _buzzerCount < MAX_PINS; ++i) {
                _A2BuzzerPins[_buzzerCount++] = Buzzer(pins_val[i]);
            }
            _isBuzzerInitialized = true;
            return _A2BuzzerPins;
        }

        static Lcd16x2I2C* ___A2_DRIVER_LCD16X2I2C(uint8_t lcd_addr = 0x27, uint8_t lcd_cols = 16, uint8_t lcd_rows = 2) {
            if (!_isLcdInitialized) {
                _A2Lcd16x2I2C = Lcd16x2I2C(lcd_addr, lcd_cols, lcd_rows);
                _A2Lcd16x2I2C.init();
                _isLcdInitialized = true;
            }
            return &_A2Lcd16x2I2C;
        }

        // --- NEW Driver Initialization Macros ---
        static Joystick* ___A2_DRIVER_JOYSTICK(uint8_t pinX, uint8_t pinY, uint8_t pinSW) {
            if (!_isJoystickInitialized) {
                _A2Joystick.setPins(pinX, pinY, pinSW);
                _isJoystickInitialized = true;
            }
            return &_A2Joystick;
        }

        // For display, we pass specific pins and dimensions for ST7789
        // This macro is a factory function for display types
        static Display* ___A2_DRIVER_DISPLAY(uint8_t type, uint8_t cs, uint8_t dc, uint8_t rst, uint16_t width, uint16_t height) {
            if (!_isDisplayInitialized) {
                switch (type) {
                    case 1: // Example: Type 1 for ST7789
                        _A2Display = new ST7789Display(cs, dc, rst, width, height);
                        break;
                    // Add more display types here (e.g., case 2: for ILI9341)
                    default:
                        DEBUG_COMPONENT("DISPLAY", "Unsupported display type.");
                        return nullptr;
                }
                if (_A2Display) {
                    _A2Display->init();
                    _isDisplayInitialized = true;
                }
            }
            return _A2Display;
        }


        // --- Debugging and Status ---
        static void ___A2_KERNEL_DEBUG(bool mode) {
            if (mode) {
                Serial.begin(115200); // Higher baud rate for faster debug output
                DEBUG_KERNEL("A2Kernel Debug Mode Enabled");
                DEBUG_KERNEL("Kernel Version: " A2KERNEL_VERSION);
                DEBUG_COMPONENT("WARNING", "THIS NOT MEAN THAT YOUR CODE IS BUG-FREE!");
                ___A2_INITIALIZED_DEBUG();
            } else {
                Serial.end();
            }
        }

        static void ___A2_INITIALIZED_DEBUG() {
            // LED Debug
            if(_isLedInitialized) {
                for(size_t i = 0; i < _ledCount; ++i) {
                    if(_A2LedPins[i].isConfigured()) {
                        DEBUG_COMPONENT("LED", "Pin " + String(_A2LedPins[i].getPin()) + " initialized.");
                    } else {
                        DEBUG_COMPONENT("LED", "Pin " + String(_A2LedPins[i].getPin()) + " not configured.");
                    }
                }
            } else {
                DEBUG_COMPONENT("LED", "No LED pins initialized.");
            }

            // Buzzer Debug
            if(_isBuzzerInitialized) {
                for(size_t i = 0; i < _buzzerCount; ++i) {
                    if(_A2BuzzerPins[i].isConfigured()) {
                        DEBUG_COMPONENT("BUZZER", "Pin " + String(_A2BuzzerPins[i].getPin()) + " initialized.");
                    } else {
                        DEBUG_COMPONENT("BUZZER", "Pin " + String(_A2BuzzerPins[i].getPin()) + " not configured.");
                    }
                }
            } else {
                DEBUG_COMPONENT("BUZZER", "No Buzzer pins initialized.");
            }

            // LCD Debug
            if(_isLcdInitialized) {
                DEBUG_COMPONENT("LCD16x2I2C", "LCD initialized.");
            } else {
                DEBUG_COMPONENT("LCD16x2I2C", "No LCD initialized.");
            }

            // Joystick Debug (NEW)
            if(_isJoystickInitialized) {
                DEBUG_COMPONENT("JOYSTICK", "Joystick initialized on X:" + String(_A2Joystick.readX()) + ", Y:" + String(_A2Joystick.readY()) + ".");
            } else {
                DEBUG_COMPONENT("JOYSTICK", "No Joystick initialized.");
            }

            // Display Debug (NEW)
            if(_isDisplayInitialized && _A2Display) {
                DEBUG_COMPONENT("DISPLAY", "Graphics display initialized. Resolution: " + String(_A2Display->width()) + "x" + String(_A2Display->height()) + ".");
            } else {
                DEBUG_COMPONENT("DISPLAY", "No graphics display initialized.");
            }
        }

        // --- Protected Debugging Functions (remain unchanged) ---
        protected:
            static void DEBUG_MESSAGE(const char* message);
            static void DEBUG_COMPONENT(const String& cmp, const String& message);
            static void DEBUG_KERNEL(const char* message);
            static void DEBUG_DRIVER(const char* message);
            static void DEBUG_STATUS(const String& cmp, const String& message, const bool status) {
                Serial.print("[" + cmp + "]: ");
                Serial.println(status ? " [ OK ] " : " [ NO ] ");
                Serial.print(message);
            }
};
#endif