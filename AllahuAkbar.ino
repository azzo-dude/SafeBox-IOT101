//Allahu Akbar

#include <Keypad.h>
#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int buzzerPin = 13;
const int ledPin = 11;

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

const int PASSWORD_ADDRESS = 0;
const int MAX_PASSWORD_LENGTH = 16;

String correctCode = "";
String enteredCode = "";
int wrongAttempts = 0;
const int maxWrongAttempts = 3;

bool changingPassword = false;

void setup() {
  Serial.begin(9600);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  lcd.init();
  lcd.backlight();
  lcd.clear();

  loadPasswordFromEEPROM();

  lcd.setCursor(0, 0);
  lcd.print("Enter code:");
  Serial.println("Enter code:");
}

void loop() {
  char key = keypad.getKey();

  if (key) {
    tone(buzzerPin, 2000, 50);
    delay(60);
    noTone(buzzerPin);

    if (key == '#') {
      if (changingPassword) {
        if (enteredCode.length() > 0) {
          savePasswordToEEPROM(enteredCode);
          correctCode = enteredCode;
          lcd.clear();
          lcd.print("Pass changed to:");
          lcd.setCursor(0, 1);
          lcd.print("****");
          Serial.print("Password changed to: ");
          Serial.println(correctCode);
          beepfunction(2, 1500, 100);
        } else {
          lcd.clear();
          lcd.print("New pass empty!");
          Serial.println("New password cannot be empty.");
          beepfunction(1, 500, 300);
        }
        changingPassword = false;
        enteredCode = "";
        delay(1500);
        lcd.clear();
        lcd.print("Enter code:");
        Serial.println("Enter code:");
      } else {
        lcd.clear();
        lcd.print("Entered code:");
        lcd.setCursor(0, 1);
        lcd.print("****");
        Serial.print("Entered code: ");
        Serial.println(enteredCode);

        if (enteredCode == correctCode) {
          lcd.clear();
          lcd.print("Correct code!");
          Serial.println("Correct code!");
          wrongAttempts = 0;
          startTimerSequence();
          lcd.clear();
          lcd.print("Enter code:");
          Serial.println("Enter code:");
        } else {
          wrongAttempts++;
          lcd.clear();
          lcd.print("Incorrect code!");
          Serial.println("Incorrect code!");

          if (wrongAttempts >= maxWrongAttempts) {
            lcd.setCursor(0, 1);
            lcd.print("Locked 5s");
            Serial.println("Locked 5s");
            beepfunction(wrongAttempts, 500, 200);
            delay(5000);
            wrongAttempts = 0;
            lcd.clear();
            lcd.print("Enter code:");
            Serial.println("Enter code:");
          } else {
            lcd.setCursor(0, 1);
            lcd.print("Try again");
            Serial.println("Try again");
            beepfunction(wrongAttempts, 600, 200);
          }
        }
        enteredCode = "";
      }
    }
    else if (key == '*') {
      enteredCode = "";
      digitalWrite(ledPin, LOW);
      changingPassword = false;
      lcd.clear();
      lcd.print("Cleared");
      Serial.println("Cleared");
      delay(1000);
      lcd.clear();
      lcd.print("Enter code:");
      Serial.println("Enter code:");
    }
    else if (key == 'D') {
      changingPassword = true;
      enteredCode = "";
      beepfunction(2, 2500, 100);
      lcd.clear();
      lcd.print("Change password:");
      lcd.setCursor(0, 1);
      lcd.print("New code then #");
      Serial.println("Changing password. Enter new code then press #:");
    }
    else {
      if (enteredCode.length() < MAX_PASSWORD_LENGTH) {
        enteredCode += key;
        lcd.setCursor(enteredCode.length() - 1, 1);
        lcd.print('*');
        Serial.print('*');
      }
    }
  }
}

void loadPasswordFromEEPROM() {
  char tempCode[MAX_PASSWORD_LENGTH + 1];
  for (int i = 0; i < MAX_PASSWORD_LENGTH; i++) {
    tempCode[i] = EEPROM.read(PASSWORD_ADDRESS + i);
  }
  tempCode[MAX_PASSWORD_LENGTH] = '\0';

  if (String(tempCode).length() == 0 || String(tempCode).indexOf(255) != -1) {
    correctCode = "1234";
    savePasswordToEEPROM(correctCode);
    lcd.clear();
    lcd.print("No pass found.");
    lcd.setCursor(0, 1);
    lcd.print("Default: 1234");
    Serial.println("No password found in EEPROM. Defaulting to '1234'.");
    delay(2000);
  } else {
    correctCode = String(tempCode);
    lcd.clear();
    lcd.print("Pass loaded:");
    lcd.setCursor(0, 1);
    lcd.print("****");
    Serial.print("Loaded password from EEPROM: ");
    Serial.println(correctCode);
    delay(2000);
  }
}

void savePasswordToEEPROM(String newPassword) {
  for (int i = 0; i < MAX_PASSWORD_LENGTH; i++) {
    EEPROM.write(PASSWORD_ADDRESS + i, 0);
  }
  for (int i = 0; i < newPassword.length(); i++) {
    EEPROM.write(PASSWORD_ADDRESS + i, newPassword.charAt(i));
  }
  lcd.clear();
  lcd.print("Password saved.");
  Serial.println("Password saved to EEPROM.");
  delay(1000);
}

void startTimerSequence() {
  lcd.clear();
  lcd.print("Countdown:");
  Serial.println("Countdown started: 45 seconds!");

  for (int i = 45; i > 0; i--) {
    lcd.setCursor(0, 1);
    lcd.print("Time left: ");
    if (i < 10) lcd.print(" ");
    lcd.print(i);
    Serial.print("Time left: ");
    Serial.println(i);

    int onTime = 100;
    int offTime = 900;
    int beepFreq = 2000;
    int beepDuration = 100;

    if (i <= 10 && i > 5) {
      onTime = 50;
      offTime = 450;
      beepFreq = 2500;
      beepDuration = 50;
    } else if (i <= 5) {
      onTime = 25;
      offTime = 225;
      beepFreq = 3000;
      beepDuration = 25;
    }

    tone(buzzerPin, beepFreq, beepDuration);
    digitalWrite(ledPin, HIGH);
    delay(onTime);
    noTone(buzzerPin);
    digitalWrite(ledPin, LOW);
    delay(offTime);
  }

  lcd.clear();
  lcd.print("Allahu Akbar");
  Serial.println("Allahu Akbar");
  for (int k = 0; k < 5; k++) {
    tone(buzzerPin, 4000);
    digitalWrite(ledPin, HIGH);
    delay(100);
    noTone(buzzerPin);
    digitalWrite(ledPin, LOW);
    delay(100);
  }
  delay(500);
  beepfunction(3, 500, 200);
  delay(1500);
}

void beepfunction(int count, int hz, int delayTime) {
  for (int i = 0; i < count; i++) {
    tone(buzzerPin, hz);
    digitalWrite(ledPin, HIGH);
    delay(delayTime);
    noTone(buzzerPin);
    digitalWrite(ledPin, LOW);
    delay(delayTime);
  }
}
