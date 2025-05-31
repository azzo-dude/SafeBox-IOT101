#ifndef A2DRIVER_H
#define A2DRIVER_H

#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Arduino.h"
#include <Adafruit_GFX.h>

// --- Led Class ---
class Led {
  private:
    uint8_t _pin;
    bool    _isConfigured = false;
  public:
    Led() : _pin(255), _isConfigured(false) {}
    Led(uint8_t pin) { setPin(pin); _isConfigured = true; }
    uint8_t getPin() const;
    void setPin(uint8_t pin);
    bool isConfigured() const;
    void on();
    void off();
    void blink(uint16_t ms);
};

// --- Buzzer Class ---
class Buzzer {
  private:
    uint8_t pin;
    bool    _isConfigured = false;
  public:
    Buzzer() : pin(255), _isConfigured(false) {}
    Buzzer(uint8_t pin) { setPin(pin); }
    bool isConfigured() const { return _isConfigured; }
    uint8_t getPin() const;
    void setPin(uint8_t pin);
    void on(uint16_t hz);
    void off();
    void beep(uint16_t hz, uint16_t ms);
};

// --- Button Class ---
class Button {
  private:
    uint8_t pin;
    bool lastButtonState;
    unsigned long lastDebounceTime;
    const unsigned long debounceDelay = 50;
  public:
    Button() : pin(255), lastButtonState(LOW), lastDebounceTime(0) {}
    Button(uint8_t pin) { setPin(pin); }
    uint8_t getPin() const;
    void setPin(uint8_t pin);
    bool isPressed();
};

// --- Keypad4x4 Class ---
class Keypad4x4 {
  private:
    char keys[4][4] = {
      {'1','2','3', 'A'},
      {'4','5','6', 'B'},
      {'7','8','9', 'C'},
      {'*','0','#', 'D'}
    };
    Keypad keypad;
    String currentInputString;
    bool _isConfigured;
  public:
    Keypad4x4() : keypad(makeKeymap(keys), nullptr, nullptr, 4, 4), _isConfigured(false) {}
    Keypad4x4(const uint8_t rowPins[], const uint8_t columnPins[]) :
      keypad(makeKeymap(keys), (uint8_t*)rowPins, (uint8_t*)columnPins, 4, 4), _isConfigured(true) {
      currentInputString = "";
    }
    void setPins(const uint8_t rowPins[], const uint8_t columnPins[]);
    const Keypad& getKeypad() const;
    char readKey();
    String readStringBlocking(char terminator, char delKey);
    String readStringNonBlocking(char terminator);
    bool isConfigured() const;
};

// --- Joystick Class (NEW) ---
// Represents an analog joystick with X, Y axes and a button.
class Joystick {
  private:
    uint8_t _pinX; // Analog pin for X-axis
    uint8_t _pinY; // Analog pin for Y-axis
    uint8_t _pinSW; // Digital pin for the switch/button
    bool    _isConfigured;

    // Calibration values (adjust these based on your joystick)
    int _minX = 0;
    int _maxX = 4095; // ESP32 ADC resolution (12-bit)
    int _midX = 2048;
    int _minY = 0;
    int _maxY = 4095;
    int _midY = 2048;
    int _deadzone = 100; // Deadzone around the center to prevent drift

  public:
    Joystick() : _pinX(255), _pinY(255), _pinSW(255), _isConfigured(false) {}
    Joystick(uint8_t pinX, uint8_t pinY, uint8_t pinSW) { setPins(pinX, pinY, pinSW); }

    void setPins(uint8_t pinX, uint8_t pinY, uint8_t pinSW);
    bool isConfigured() const { return _isConfigured; }

    // Read raw analog values
    int readX() const;
    int readY() const;

    // Read normalized values (-1000 to 1000, with deadzone)
    int readNormalizedX() const;
    int readNormalizedY() const;

    // Read joystick button state
    bool isButtonPressed() const;

    // Set calibration values (optional, for more accurate readings)
    void setCalibration(int minX, int maxX, int midX, int minY, int maxY, int midY, int deadzone);
};

// --- Abstract Display Class (NEW) ---
// Defines a common interface for all display drivers.
class Display {
  public:
    virtual ~Display() = default; // Virtual destructor for proper cleanup

    // Basic display control
    virtual void init() = 0;
    virtual void clear() = 0;
    virtual void fillScreen(uint16_t color) = 0;
    virtual void setRotation(uint8_t r) = 0;
    virtual void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) = 0;
    virtual void pushColors(uint16_t *data, uint32_t len) = 0;

    // Drawing primitives
    virtual void drawPixel(int16_t x, int16_t y, uint16_t color) = 0;
    virtual void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) = 0;
    virtual void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) = 0;
    virtual void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) = 0;

    // Text rendering (basic, can be expanded)
    virtual void setCursor(int16_t x, int16_t y) = 0;
    virtual void setTextColor(uint16_t color) = 0;
    virtual void setTextSize(uint8_t s) = 0;
    virtual size_t write(uint8_t c) = 0; // For printing characters

    // Get display dimensions
    virtual int16_t width() = 0;
    virtual int16_t height() = 0;

    // Frame buffer management (for double buffering)
    virtual uint16_t* getFrameBuffer() = 0; // Get pointer to the frame buffer
    virtual void updateDisplay() = 0; // Copy frame buffer to actual display

    // Color conversion (RGB888 to RGB565)
    static uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
      return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
    }
};

// --- Concrete ST7789 Display Implementation (NEW) ---
// This is a placeholder. You would typically use a library like TFT_eSPI or Adafruit_ST7789.
// For demonstration, we'll simulate its interface.
// For a real project, you'd include the actual library and wrap its functions.
// Example: #include <Adafruit_ST7789.h>
// Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
class ST7789Display : public Display {
  private:
    // Pins for ST7789 (these would be defined in your main sketch or config)
    uint8_t _cs, _dc, _rst;
    uint16_t _width, _height;
    uint16_t* _frameBuffer; // Software frame buffer for double buffering

    // Placeholder for actual display driver object (e.g., Adafruit_ST7789* _tft;)
    // We'll simulate it for now.

  public:
    ST7789Display() : _cs(255), _dc(255), _rst(255), _width(240), _height(240), _frameBuffer(nullptr) {}
    ST7789Display(uint8_t cs, uint8_t dc, uint8_t rst, uint16_t width, uint16_t height);
    ~ST7789Display(); // Destructor to free frame buffer

    void init() override;
    void clear() override;
    void fillScreen(uint16_t color) override;
    void setRotation(uint8_t r) override;
    void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) override;
    void pushColors(uint16_t *data, uint32_t len) override;

    void drawPixel(int16_t x, int16_t y, uint16_t color) override;
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) override;
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) override;
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) override;

    void setCursor(int16_t x, int16_t y) override;
    void setTextColor(uint16_t color) override;
    void setTextSize(uint8_t s) override;
    size_t write(uint8_t c) override;

    int16_t width() override { return _width; }
    int16_t height() override { return _height; }

    uint16_t* getFrameBuffer() override { return _frameBuffer; }
    void updateDisplay() override; // Copies _frameBuffer to actual display

    // Helper to draw a bitmap from a 16-bit color array
    void drawBitmap(int16_t x, int16_t y, const uint16_t *bitmap, int16_t w, int16_t h);
};

// --- LCD16x2I2C Class (Placeholder - not for Doom graphics) ---
// This class remains as is, but it's not suitable for Doom's graphics.
class Lcd16x2I2C {
  private:
    LiquidCrystal_I2C _lcd;
    bool _isConfigured;
  public:
    Lcd16x2I2C() : _lcd(0x27, 16, 2), _isConfigured(false) {} // Default address
    Lcd16x2I2C(uint8_t lcd_addr, uint8_t lcd_cols, uint8_t lcd_rows);
    void init();
    void print(const String& message);
    void setCursor(uint8_t col, uint8_t row);
    void clear();
    bool isConfigured() const { return _isConfigured; }
};

#endif // A2DRIVER_H