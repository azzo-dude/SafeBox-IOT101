#include "Components.h"

//================================================================================
// Led Component Implementation
//================================================================================
Led::Led(uint8_t pin)
  : pin_(pin) {
  pinMode(pin_, OUTPUT);
  digitalWrite(pin_, LOW);
}

uint8_t Led::getPin() const {
  return pin_;
}

void Led::setPin(uint8_t pin) {
  pin_ = pin;
  pinMode(pin_, OUTPUT);
  digitalWrite(pin_, LOW);
}

void Led::on() {
  digitalWrite(pin_, HIGH);
}

void Led::off() {
  digitalWrite(pin_, LOW);
}

void Led::blink(uint64_t ms) {
  on();
  delay(ms);
  off();
  delay(ms);
}

void Led::blink(uint64_t ms, uint64_t count) {
  while (count--) {
    on();
    delay(ms);
    off();
    delay(ms);
  }
}

//================================================================================
// Buzzer Component Implementation
//================================================================================
Buzzer::Buzzer(uint8_t pin)
  : pin_(pin) {
  pinMode(pin_, OUTPUT);
  digitalWrite(pin_, LOW);
}
uint8_t Buzzer::getPin() const {
  return pin_;
}
void Buzzer::setPin(uint8_t pin) {
  pin_ = pin;
  pinMode(pin_, OUTPUT);
  digitalWrite(pin_, LOW);
}
void Buzzer::tone(int16_t hz) {
  ::tone(pin_, hz);
}
void Buzzer::tone(int16_t hz, int64_t dur) {
  ::tone(pin_, hz, dur);
}
void Buzzer::noTone() {
  ::noTone(pin_);
}


//================================================================================
// Keypad 3x4 Component Implementation
//================================================================================
Keypad3x4::Keypad3x4(byte rowPins[ROWS], byte colPins[COLS]) {
  // Copy pin numbers to internal arrays
  for (int r = 0; r < ROWS; r++) rowPins_[r] = rowPins[r];
  for (int c = 0; c < COLS; c++) colPins_[c] = colPins[c];

  // Define the keymap
  char defaultKeys[ROWS][COLS] = {
    { '1', '2', '3' },
    { '4', '5', '6' },
    { '7', '8', '9' },
    { '*', '0', '#' }
  };
  for (int r = 0; r < ROWS; r++) {
    for (int c = 0; c < COLS; c++) {
      keys_[r][c] = defaultKeys[r][c];
    }
  }

  keypad_ = new Keypad(makeKeymap(keys_), rowPins_, colPins_, ROWS, COLS);
}

Keypad3x4::~Keypad3x4() {
  delete keypad_;
}

char Keypad3x4::getKey() {
  return keypad_->getKey();
}

//================================================================================
// AS608 Fingerprint Sensor Component Implementation
//================================================================================
AS608::AS608(HardwareSerial* serial) {
  finger_ = new Adafruit_Fingerprint(serial);
}

AS608::~AS608() {
  delete finger_;
}

bool AS608::begin() {
  finger_->begin(57600);
  return verifyPassword();
}

bool AS608::verifyPassword() {
  return finger_->verifyPassword();
}

int16_t AS608::getFingerprintID() {
  uint8_t p = finger_->getImage();
  if (p != FINGERPRINT_OK) return -1;

  p = finger_->image2Tz();
  if (p != FINGERPRINT_OK) return -1;

  p = finger_->fingerSearch();
  if (p != FINGERPRINT_OK) return -1;

  // found a match!
  return finger_->fingerID;
}


//================================================================================
// NFC MFRC522 Reader/Writer Component Implementation
//================================================================================

NFC_MFRC522::NFC_MFRC522(uint8_t ssPin, uint8_t rstPin)
  : ssPin_(ssPin), rstPin_(rstPin) {
  mfrc522_ = new MFRC522(ssPin_, rstPin_);
}

NFC_MFRC522::~NFC_MFRC522() {
  delete mfrc522_;
}

void NFC_MFRC522::begin() {
  SPI.begin();
  mfrc522_->PCD_Init();
}

bool NFC_MFRC522::isNewCardPresent() {
  return mfrc522_->PICC_IsNewCardPresent();
}

String NFC_MFRC522::readCardSerial() {
  if (mfrc522_->PICC_ReadCardSerial()) {
    String content = "";
    for (byte i = 0; i < mfrc522_->uid.size; i++) {
      content.concat(String(mfrc522_->uid.uidByte[i] < 0x10 ? " 0" : " "));
      content.concat(String(mfrc522_->uid.uidByte[i], HEX));
    }
    content.toUpperCase();
    mfrc522_->PICC_HaltA();
    return content.substring(1);
  }
  return "";
}

//================================================================================
// Lcd16x2I2C Component
//================================================================================
Lcd16x2I2C::Lcd16x2I2C(uint8_t address, uint8_t cols, uint8_t rows)
    : address_(address), cols_(cols), rows_(rows) {
    lcd_ = new LiquidCrystal_I2C(address_, cols_, rows_);
}

Lcd16x2I2C::~Lcd16x2I2C() {
    delete lcd_;
}

void Lcd16x2I2C::begin() {
    lcd_->init();
    lcd_->backlight();
}

void Lcd16x2I2C::clear() {
    lcd_->clear();
}

void Lcd16x2I2C::backlight() {
    lcd_->backlight();
}

void Lcd16x2I2C::noBacklight() {
    lcd_->noBacklight();
}

void Lcd16x2I2C::setCursor(uint8_t col, uint8_t row) {
    lcd_->setCursor(col, row);
}

void Lcd16x2I2C::print(const String& text) {
    lcd_->print(text);
}

void Lcd16x2I2C::print(const String& text, uint8_t col, uint8_t row) {
    lcd_->setCursor(col, row);
    lcd_->print(text);
}
