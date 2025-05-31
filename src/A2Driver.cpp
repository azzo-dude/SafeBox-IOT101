#include "A2Driver.h"
#include <SPI.h> // Required for many displays like ST7789
// For a real ST7789, you'd include the actual library here, e.g.:
// #include <Adafruit_GFX.h>
// #include <Adafruit_ST7789.h>
// #include <Fonts/FreeSansBold9pt7b.h> // Example font

// --- Led Implementation ---
uint8_t Led::getPin() const {
  return _pin;
}

void Led::setPin(uint8_t pin) {
  this->_pin = pin;
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW); // Ensure LED is off initially
  _isConfigured = true; // Mark as configured
}

bool Led::isConfigured() const {
    return this->_isConfigured;
}

void Led::on() {
  if (_isConfigured) {
    digitalWrite(getPin(), HIGH);
  }
}

void Led::off() {
  if (_isConfigured) {
    digitalWrite(getPin(), LOW);
  }
}

void Led::blink(uint16_t ms) {
  if (_isConfigured) {
    on();
    delay(ms);
    off();
    delay(ms);
  }
}

// --- Buzzer Implementation ---
uint8_t Buzzer::getPin() const {
  return pin;
}

void Buzzer::setPin(uint8_t pin) {
  this->pin = pin;
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW); // Ensure buzzer is off initially
  _isConfigured = true; // Mark as configured
}

void Buzzer::on(uint16_t hz) {
  if (_isConfigured) {
    tone(getPin(), hz);
  }
}

void Buzzer::off() {
  if (_isConfigured) {
    noTone(getPin());
  }
}

void Buzzer::beep(uint16_t hz, uint16_t ms) {
  if (_isConfigured) {
    on(hz);
    delay(ms);
    off();
    delay(ms);
  }
}

// --- Button Implementation ---
uint8_t Button::getPin() const {
  return pin;
}

void Button::setPin(uint8_t pin) {
  this->pin = pin;
  pinMode(pin, INPUT_PULLUP); // Use INPUT_PULLUP for internal pull-up resistor
  lastButtonState = digitalRead(pin); // Read initial state
}

bool Button::isPressed() {
  int reading = digitalRead(getPin()); // Read current state

  // If the button state has changed, reset the debounce timer
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  // If the debounce time has passed, update the button state
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != lastButtonState) {
      lastButtonState = reading; // Update state only if stable
    }
  }
  // Return true if the button is currently pressed (LOW for INPUT_PULLUP)
  return (lastButtonState == LOW);
}

// --- Keypad4x4 Implementation ---
void Keypad4x4::setPins(const uint8_t rowPins[], const uint8_t columnPins[]) {
  // Initialize the Keypad object with the provided pins
  keypad = Keypad(makeKeymap(keys), (uint8_t*)rowPins, (uint8_t*)columnPins, 4, 4);
  _isConfigured = true;
}

const Keypad& Keypad4x4::getKeypad() const { // Changed to return const reference for efficiency
  return keypad;
}

char Keypad4x4::readKey() {
  if (_isConfigured) {
    return keypad.getKey(); // Returns NO_KEY if no key is pressed
  }
  return NO_KEY;
}

String Keypad4x4::readStringBlocking(char terminator, char delKey) {
  if (!_isConfigured) return "";
  String input = "";
  while (true) {
    char key = keypad.getKey();
    if (key) { // If a key is pressed
      Serial.print("[DEBUG] Keypad Input: ");
      Serial.println(key);
      if (key == terminator) {
        return input; // Return the accumulated string
      } else if (key == delKey) {
        if (input.length() > 0) {
          input.remove(input.length() - 1); // Remove last character
          Serial.println("[DEBUG] Deleted last character.");
        }
      } else {
        input += key; // Append key to string
      }
    }
    delay(50); // Small delay to prevent multiple reads of a single press
  }
}

String Keypad4x4::readStringNonBlocking(char terminator) {
  if (!_isConfigured) return "";
  char key = keypad.getKey();
  if (key) { // If a key is pressed
    if (key == terminator) {
      String tempInput = currentInputString;
      currentInputString = ""; // Reset buffer for next input
      return tempInput; // Return the completed string
    }
    currentInputString += key; // Append key to buffer
  }
  return ""; // Return empty string if terminator not pressed yet
}

bool Keypad4x4::isConfigured() const {
    return _isConfigured;
}

// --- Joystick Implementation (NEW) ---
void Joystick::setPins(uint8_t pinX, uint8_t pinY, uint8_t pinSW) {
  _pinX = pinX;
  _pinY = pinY;
  _pinSW = pinSW;
  pinMode(_pinSW, INPUT_PULLUP); // Joystick button usually active low
  _isConfigured = true;
  // ESP32 ADC setup (optional, can be done once in setup)
  // analogReadResolution(12); // Set ADC resolution to 12 bits (0-4095)
  // analogSetAttenuation(ADC_11db); // Set attenuation for 0-3.6V input
}

int Joystick::readX() const {
  if (_isConfigured) {
    return analogRead(_pinX);
  }
  return 0;
}

int Joystick::readY() const {
  if (_isConfigured) {
    return analogRead(_pinY);
  }
  return 0;
}

int Joystick::readNormalizedX() const {
  if (!_isConfigured) return 0;
  int rawX = readX();
  if (rawX > _midX + _deadzone) {
    return map(rawX, _midX + _deadzone, _maxX, 0, 1000);
  } else if (rawX < _midX - _deadzone) {
    return map(rawX, _minX, _midX - _deadzone, -1000, 0);
  }
  return 0; // Within deadzone
}

int Joystick::readNormalizedY() const {
  if (!_isConfigured) return 0;
  int rawY = readY();
  if (rawY > _midY + _deadzone) {
    return map(rawY, _midY + _deadzone, _maxY, 0, 1000);
  } else if (rawY < _midY - _deadzone) {
    return map(rawY, _minY, _midY - _deadzone, -1000, 0);
  }
  return 0; // Within deadzone
}

bool Joystick::isButtonPressed() const {
  if (_isConfigured) {
    return digitalRead(_pinSW) == LOW; // Low when pressed
  }
  return false;
}

void Joystick::setCalibration(int minX, int maxX, int midX, int minY, int maxY, int midY, int deadzone) {
  _minX = minX;
  _maxX = maxX;
  _midX = midX;
  _minY = minY;
  _maxY = maxY;
  _midY = midY;
  _deadzone = deadzone;
}


// --- ST7789Display Implementation (NEW) ---
// This is a simplified implementation. A real one would use a library.
// For example, using Adafruit_ST7789 library:
// Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
// And then wrap its functions:
// ST7789Display::init() { tft.init(width, height); tft.setRotation(0); }
// ST7789Display::drawPixel(x,y,color) { tft.drawPixel(x,y,color); }
// ... and so on.

ST7789Display::ST7789Display(uint8_t cs, uint8_t dc, uint8_t rst, uint16_t width, uint16_t height)
  : _cs(cs), _dc(dc), _rst(rst), _width(width), _height(height) {
  // Allocate frame buffer using malloc.
  // If PSRAM is available and desired on ESP32, use `ps_malloc` or `heap_caps_malloc`
  // from <esp_heap_caps.h> like: `_frameBuffer = (uint16_t*) heap_caps_malloc(width * height * sizeof(uint16_t), MALLOC_CAP_SPIRAM);`
  _frameBuffer = (uint16_t*) malloc(width * height * sizeof(uint16_t));
  if (!_frameBuffer) {
    Serial.println("[ERROR] Failed to allocate frame buffer for ST7789Display!");
  }
}

ST7789Display::~ST7789Display() {
  if (_frameBuffer) {
    free(_frameBuffer);
    _frameBuffer = nullptr;
  }
}

void ST7789Display::init() {
  // In a real scenario, this would initialize the ST7789 hardware.
  // For example:
  // _tft = new Adafruit_ST7789(_cs, _dc, _rst);
  // _tft->init(_width, _height);
  // _tft->setRotation(0);
  // _tft->fillScreen(ST77XX_BLACK);
  Serial.println("[ST7789] Display initialized (simulated).");
  if (_frameBuffer) {
    fillScreen(color565(0, 0, 0)); // Clear software frame buffer
  }
}

void ST7789Display::clear() {
  fillScreen(color565(0, 0, 0));
}

void ST7789Display::fillScreen(uint16_t color) {
  // In a real scenario, this would fill the actual display.
  // _tft->fillScreen(color);
  if (_frameBuffer) {
    for (size_t i = 0; i < (size_t)_width * _height; ++i) {
      _frameBuffer[i] = color;
    }
  }
}

void ST7789Display::setRotation(uint8_t r) {
  // _tft->setRotation(r);
  Serial.print("[ST7789] Set rotation to ");
  Serial.print(r);
  Serial.println(" (simulated).");
  // Adjust _width and _height if rotation swaps them
  if (r == 1 || r == 3) { // 90 or 270 degrees
    uint16_t temp = _width;
    _width = _height;
    _height = temp;
  }
}

void ST7789Display::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
  // _tft->setAddrWindow(x0, y0, x1, y1);
  Serial.print("[ST7789] Set address window: (");
  Serial.print(x0);
  Serial.print(",");
  Serial.print(y0);
  Serial.print(") to (");
  Serial.print(x1);
  Serial.print(",");
  Serial.print(y1);
  Serial.println(") (simulated).");
}

void ST7789Display::pushColors(uint16_t *data, uint32_t len) {
  // _tft->pushColors(data, len);
  // For software frame buffer, this would copy data into the buffer
  // This method is generally used by low-level graphics libraries.
  Serial.print("[ST7789] Pushed ");
  Serial.print(len);
  Serial.println(" colors (simulated).");
}

void ST7789Display::drawPixel(int16_t x, int16_t y, uint16_t color) {
  // Fix for signed/unsigned comparison warning: cast _width and _height to int16_t for comparison
  if (x < 0 || x >= (int16_t)_width || y < 0 || y >= (int16_t)_height) return; // Bounds check
  // _tft->drawPixel(x, y, color);
  if (_frameBuffer) {
    _frameBuffer[y * _width + x] = color;
  }
}

void ST7789Display::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
  // _tft->drawLine(x0, y0, x1, y1, color);
  // Basic Bresenham's line algorithm for frame buffer:
  int16_t dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
  int16_t dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
  int16_t err = dx + dy, e2; // error value e_xy

  for (;;) {
    drawPixel(x0, y0, color);
    if (x0 == x1 && y0 == y1) break;
    e2 = 2 * err;
    if (e2 >= dy) { err += dy; x0 += sx; } // e_xy + e_x > 0
    if (e2 <= dx) { err += dx; y0 += sy; } // e_xy + e_y < 0
  }
}

void ST7789Display::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  // _tft->drawRect(x, y, w, h, color);
  drawLine(x, y, x + w - 1, y, color);
  drawLine(x, y + h - 1, x + w - 1, y + h - 1, color);
  drawLine(x, y, x, y + h - 1, color);
  drawLine(x + w - 1, y, x + w - 1, y + h - 1, color);
}

void ST7789Display::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  // _tft->fillRect(x, y, w, h, color);
  for (int16_t i = x; i < x + w; ++i) {
    for (int16_t j = y; j < y + h; ++j) {
      drawPixel(i, j, color);
    }
  }
}

void ST7789Display::setCursor(int16_t x, int16_t y) {
  // _tft->setCursor(x, y);
  Serial.print("[ST7789] Set cursor to (");
  Serial.print(x);
  Serial.print(",");
  Serial.print(y);
  Serial.println(") (simulated).");
}

void ST7789Display::setTextColor(uint16_t color) {
  // _tft->setTextColor(color);
  Serial.println("[ST7789] Set text color (simulated).");
}

void ST7789Display::setTextSize(uint8_t s) {
  // _tft->setTextSize(s);
  Serial.print("[ST7789] Set text size to ");
  Serial.print(s);
  Serial.println(" (simulated).");
}

size_t ST7789Display::write(uint8_t c) {
  // _tft->write(c);
  Serial.print("[ST7789] Wrote char '");
  Serial.print((char)c);
  Serial.println("' (simulated).");
  return 1; // Simulate success
}

void ST7789Display::updateDisplay() {
  // This is where the software frame buffer is pushed to the actual display hardware.
  // For a real library like TFT_eSPI, you might use `pushImage` or `pushColors`
  // to transfer the entire buffer efficiently.
  // Example: _tft->pushImage(0, 0, _width, _height, _frameBuffer);
  Serial.println("[ST7789] Display updated from frame buffer (simulated).");
}

void ST7789Display::drawBitmap(int16_t x, int16_t y, const uint16_t *bitmap, int16_t w, int16_t h) {
    // This function draws a 16-bit color bitmap directly to the frame buffer.
    // It's crucial for rendering sprites and textures in games.
    for (int16_t j = 0; j < h; j++) {
        for (int16_t i = 0; i < w; i++) {
            drawPixel(x + i, y + j, bitmap[j * w + i]);
        }
    }
}

// --- LCD16x2I2C Implementation ---
Lcd16x2I2C::Lcd16x2I2C(uint8_t lcd_addr, uint8_t lcd_cols, uint8_t lcd_rows)
  : _lcd(lcd_addr, lcd_cols, lcd_rows), _isConfigured(false) {}

void Lcd16x2I2C::init() {
  _lcd.init();
  _lcd.backlight();
  _isConfigured = true;
  Serial.println("[LCD16x2I2C] Initialized.");
}

void Lcd16x2I2C::print(const String& message) {
  if (_isConfigured) {
    _lcd.print(message);
  }
}

void Lcd16x2I2C::setCursor(uint8_t col, uint8_t row) {
  if (_isConfigured) {
    _lcd.setCursor(col, row);
  }
}

void Lcd16x2I2C::clear() {
  if (_isConfigured) {
    _lcd.clear();
  }
}