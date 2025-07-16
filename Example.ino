// Arduino.ino

#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
MFRC522 rfid(10, 9);
Servo servo;





Led redLed(4);
Led greenLed(5);
Buzzer buzzer(7);

void setup() {
  Serial.begin(9600);
  mySerial.begin(57600);
  if (finger.verifyPassword()) finger.getTemplateCount();
  SPI.begin();
  rfid.PCD_Init();
  servo.attach(8);
  servo.write(0);
}

int getFingerID() {
  unsigned long start = millis();
  while (millis() - start < 5000) {
    int p = finger.getImage();
    if (p == FINGERPRINT_OK) {
      p = finger.image2Tz();
      if (p == FINGERPRINT_OK) {
        p = finger.fingerSearch();
        if (p == FINGERPRINT_OK) return finger.fingerID;
        return -1;
      }
    }
    delay(50);
  }
  return -2;
}

String getRFIDUID() {
  unsigned long start = millis();
  while (millis() - start < 5000) {
    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
      String uid = "";
      for (byte i = 0; i < rfid.uid.size; i++) {
        if (rfid.uid.uidByte[i] < 0x10) uid += "0";
        uid += String(rfid.uid.uidByte[i], HEX);
      }
      uid.toUpperCase();
      rfid.PICC_HaltA();
      return uid;
    }
    delay(50);
  }
  return "TIMEOUT";
}

int enrollFinger(int id) {
  int p = -1;
  p = finger.getImage();
  if (p != FINGERPRINT_OK) return p;
  p = finger.image2Tz(1);
  if (p != FINGERPRINT_OK) return p;
  delay(2000);
  p = finger.getImage();
  if (p != FINGERPRINT_OK) return p;
  p = finger.image2Tz(2);
  if (p != FINGERPRINT_OK) return p;
  p = finger.createModel();
  if (p != FINGERPRINT_OK) return p;
  p = finger.storeModel(id);
  return p;
}

void openSafe() {
  greenLed.on();
  servo.write(90);
  buzzer.tone(1000, 200);
  delay(5000);
  servo.write(0);
  greenLed.off();
}

void denyAccess() {
  redLed.on();
  buzzer.tone(500, 500);
  delay(500);
  redLed.off();
}

void loop() {
  if (finger.getImage() == FINGERPRINT_OK) {
    if (finger.image2Tz() == FINGERPRINT_OK) {
      if (finger.fingerSearch() == FINGERPRINT_OK) {
        Serial.print("FINGER_ID ");
        Serial.println(finger.fingerID);
      }
    }
  }
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    String uid = getRFIDUID(); // reuse but already read
    Serial.print("RFID_UID ");
    Serial.println(uid);
    rfid.PICC_HaltA();
  }
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd == "CHECK_FINGER") {
      int id = getFingerID();
      Serial.print("FINGER_ID ");
      Serial.println(id);
    } else if (cmd == "CHECK_RFID") {
      String uid = getRFIDUID();
      Serial.print("RFID_UID ");
      Serial.println(uid);
    } else if (cmd == "OPEN") {
      openSafe();
      Serial.println("OK");
    } else if (cmd == "DENY") {
      denyAccess();
      Serial.println("OK");
    } else if (cmd.startsWith("ENROLL_FINGER ")) {
      int id = cmd.substring(14).toInt();
      int res = enrollFinger(id);
      Serial.print("ENROLL_RESULT ");
      Serial.println(res);
    } else if (cmd == "REGISTER_RFID") {
      String uid = getRFIDUID();
      Serial.print("RFID_UID ");
      Serial.println(uid);
    } else if (cmd == "GET_TEMPLATE_COUNT") {
      Serial.print("TEMPLATE_COUNT ");
      Serial.println(finger.templateCount);
    }
  }
  delay(50);
}