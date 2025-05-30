#include "IOManager.h"
#include <stdarg.h> // Required for va_list, va_start, va_end for printf-like functionality

// --- Led Implementation ---
uint8_t Led::getPin() const {
  return pin;
}

void Led::setPin(uint8_t pin) {
  this->pin = pin;
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW); // Ensure LED is off initially
}

void Led::on() {
  digitalWrite(getPin(), HIGH);
}

void Led::off() {
  digitalWrite(getPin(), LOW);
}

void Led::blink(uint16_t ms) {
  on();
  delay(ms);
  off();
  delay(ms);
}

// --- Buzzer Implementation ---
uint8_t Buzzer::getPin() const {
  return pin;
}

void Buzzer::setPin(uint8_t pin) {
  this->pin = pin;
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW); // Ensure buzzer is off initially
}

void Buzzer::on(uint16_t hz) {
  tone(getPin(), hz);
}

void Buzzer::off() {
  noTone(getPin());
}

void Buzzer::beep(uint16_t hz, uint16_t ms) {
  on(hz);
  delay(ms);
  off();
  delay(ms);
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
  return keypad.getKey(); // Returns NO_KEY if no key is pressed
}

String Keypad4x4::readStringBlocking(char terminator, char delKey) {
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

// --- Lcd16x2I2C Implementation ---
uint8_t Lcd16x2I2C::findAddress() {
  // Common I2C addresses for 16x2 LCDs
  uint8_t addresses[] = {0x27, 0x3F}; // Often 0x27 or 0x3F
  Wire.begin(); // Ensure Wire library is initialized before scanning
  for (uint8_t i = 0; i < sizeof(addresses); i++) {
    Wire.beginTransmission(addresses[i]);
    if (Wire.endTransmission() == 0) { // If transmission is successful, address is found
      return addresses[i];
    }
  }
  return 0x00; // Return 0x00 if no address is found
}

void Lcd16x2I2C::begin() {
  _address = findAddress(); // Attempt to find the I2C address
  if (_address != 0x00) {
    // Re-initialize lcd object with found address and stored dimensions
    lcd = LiquidCrystal_I2C(_address, _cols, _rows);
    lcd.init(); // Initialize the LCD module (specific to LiquidCrystal_I2C)
    lcd.backlight(); // Turn on backlight
    _isConfigured = true;
  } else {
    _isConfigured = false;
  }
}

void Lcd16x2I2C::setDimensions(uint8_t cols, uint8_t rows) {
    _cols = cols;
    _rows = rows;
    // Note: If LCD is already 'begun', this won't re-initialize it.
    // Call begin() again if you change dimensions after initial setup.
}

void Lcd16x2I2C::print(const String& text) {
  buffer += text; // Append text to the internal buffer

  // Automatically adjust vertical scroll to show latest content
  // Assuming no explicit newlines in buffer for simplicity in line calculation
  int totalLines = (buffer.length() + _cols - 1) / _cols;
  if (totalLines > _rows) {
    scrollVerticalIndex = totalLines - _rows;
  } else {
    scrollVerticalIndex = 0; // Reset if content fits
  }
  scrollHorizontalIndex = 0; // Reset horizontal scroll on new print
  renderBufferToLcd(); // Immediately update LCD after printing
}

void Lcd16x2I2C::clear() {
  buffer = ""; // Clear internal buffer
  scrollVerticalIndex = 0; // Reset vertical scroll
  scrollHorizontalIndex = 0; // Reset horizontal scroll
  lcd.clear(); // Clear physical LCD
}

void Lcd16x2I2C::clearBuffer() {
    buffer = ""; // Clear only the internal buffer
    scrollVerticalIndex = 0;
    scrollHorizontalIndex = 0;
}

void Lcd16x2I2C::scrollUp() {
  if (scrollVerticalIndex > 0) {
    scrollVerticalIndex--;
    display(); // Update physical LCD after scroll
  }
}

void Lcd16x2I2C::scrollDown() {
  int totalLines = (buffer.length() + _cols - 1) / _cols;
  if (totalLines > _rows && scrollVerticalIndex < totalLines - _rows) { // Ensure there are more lines than display can show
    scrollVerticalIndex++;
    display(); // Update physical LCD after scroll
  }
}

void Lcd16x2I2C::scrollRight() {
    // Determine the maximum possible horizontal scroll for the current view
    // This is a simplification; a more complex buffer might track per-line width
    int maxHorizontalScroll = 0;
    // Iterate through the lines currently visible on the LCD
    for (uint8_t row = 0; row < _rows; ++row) {
        int currentBufferLineIdx = scrollVerticalIndex + row;
        int lineStartCharIdx = currentBufferLineIdx * _cols;
        
        // Ensure we don't go out of bounds of the buffer
        if (lineStartCharIdx < static_cast<int>(buffer.length())) {
            String lineContent = buffer.substring(lineStartCharIdx);
            // Only consider content up to the LCD's column limit for max scroll calculation
            if (lineContent.length() > _cols) {
                maxHorizontalScroll = max(maxHorizontalScroll, (int)lineContent.length() - _cols);
            }
        }
    }
    
    if (scrollHorizontalIndex < maxHorizontalScroll) {
        scrollHorizontalIndex++;
        display(); // Update physical LCD after scroll
    }
}

void Lcd16x2I2C::scrollLeft() {
    if (scrollHorizontalIndex > 0) {
        scrollHorizontalIndex--;
        display(); // Update physical LCD after scroll
    }
}

void Lcd16x2I2C::renderBufferToLcd() {
  if (!_isConfigured) return; // Don't try to render if LCD isn't configured

  lcd.clear(); // Clear the physical display before redrawing
  // Calculate total logical lines in buffer, assuming no explicit newlines in buffer
  int totalLines = (buffer.length() + _cols - 1) / _cols;

  for (uint8_t row = 0; row < _rows; row++) {
    int currentBufferLineIdx = scrollVerticalIndex + row; // Calculate which logical line to display on this physical row
    
    if (currentBufferLineIdx >= totalLines) {
      break; // No more content to display
    }

    lcd.setCursor(0, row); // Set cursor to the beginning of the current physical row

    int lineStartCharIdx = currentBufferLineIdx * _cols; // Calculate start index in buffer for this logical line
    String lineToDisplay = buffer.substring(lineStartCharIdx); // Get the rest of the buffer from this line's start

    // Apply horizontal scroll and trim line to fit LCD width
    if (lineToDisplay.length() > _cols) {
        if (static_cast<size_t>(scrollHorizontalIndex) < lineToDisplay.length()) {
            lineToDisplay = lineToDisplay.substring(scrollHorizontalIndex);
        } else {
            lineToDisplay = ""; // If scrolled past content, show empty
        }
    }
    
    // Ensure the line doesn't exceed LCD column width
    if (lineToDisplay.length() > _cols) {
        lineToDisplay = lineToDisplay.substring(0, _cols);
    }
    
    lcd.print(lineToDisplay); // Print the prepared line to the LCD
  }
}

void Lcd16x2I2C::display() {
    renderBufferToLcd(); // Call the internal rendering function
}

void Lcd16x2I2C::home() {
    lcd.home(); // Move physical cursor to 0,0
}

void Lcd16x2I2C::setCursor(uint8_t col, uint8_t row) {
    lcd.setCursor(col, row); // Set physical cursor
}

bool Lcd16x2I2C::isConfigured() const {
    return _isConfigured;
}

void Lcd16x2I2C::cursor() {
  lcd.cursor();
}

void Lcd16x2I2C::noCursor() {
  lcd.noCursor();
}

void Lcd16x2I2C::blink() {
  lcd.blink();
}

void Lcd16x2I2C::noBlink() {
  lcd.noBlink();
}