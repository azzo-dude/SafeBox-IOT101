// =================================================================
//                      LIBRARIES & INCLUDES
// =================================================================
#include <WiFi.h>
#include <WebServer.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>
#include <Adafruit_Fingerprint.h>
#include <EEPROM.h>
#include "SafeBoxConfig.h"

// =================================================================
//                      OBJECT INITIALIZATION
// =================================================================

// --- Keypad ---
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3', 'A'},
  {'4','5','6', 'B'},
  {'7','8','9', 'C'},
  {'*','0','#', 'D'}
};
byte rowPins[ROWS] = {KEYPAD_R1, KEYPAD_R2, KEYPAD_R3, KEYPAD_R4};
byte colPins[COLS] = {KEYPAD_C1, KEYPAD_C2, KEYPAD_C3, KEYPAD_C4};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// --- LCD ---
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- Servo ---
Servo lockServo;

// --- Fingerprint Sensor ---
// Use Serial2 for the fingerprint sensor to avoid conflicts with USB programming
HardwareSerial fingerprintSerial(2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerprintSerial);

// --- Web Server ---
WebServer server(80);


// =================================================================
//                      GLOBAL VARIABLES
// =================================================================
// MODIFIED: Renamed IDLE to STATE_IDLE to avoid conflict with Keypad library
enum SystemState { STATE_IDLE, WAITING_INPUT, LOCKED_OUT, CONFIG_MENU, AP_MODE };
SystemState currentState = STATE_IDLE;

String password = "";
String inputBuffer = "";
int failedAttempts = 0;
unsigned long lockStartTime = 0;
bool isApOn = false;

// =================================================================
//                      FUNCTION FORWARD DECLARATIONS
// =================================================================
void beep(int duration_ms);
void updateLcd(String line1, String line2);
void savePassword();
void loadPassword();
void grantAccess();
void denyAccess();
void checkPassword();
void handleKeypadInput(char key);
void handleApModeKeypad();
void toggleApMode();
void handleRoot();
void handleSave();
void handleAddFinger();
void handleDeleteFinger();
void enterConfigMenu();
void runAddFingerMenu();
void runDeleteFingerMenu();
void runChangePasswordMenu();
int getFingerprintID();
void enrollFingerprint(uint8_t id);
void deleteFingerprint(uint8_t id);


// =================================================================
//                              SETUP
// =================================================================
void setup() {
    Serial.begin(115200);
    finger.begin(57600);
    while (!Serial) {; // wait for serial port to connect.
    }
    Serial.println("\n\n[SETUP] Booting SafeBox...");

    // --- Initialize Hardware ---
    Serial.println("[SETUP] Initializing hardware pins...");
    pinMode(GREEN_LED_PIN, OUTPUT);
    pinMode(RED_LED_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    
    Serial.println("[SETUP] Attaching servo motor...");
    lockServo.attach(SERVO_PIN);
    lockServo.write(SERVO_LOCKED_ANGLE);

    Serial.println("[SETUP] Servo locked.");

    Serial.println("[SETUP] Initializing I2C LCD...");
    lcd.init();
    lcd.backlight();
    updateLcd("SafeBox v1.0", "Initializing...");
    
    // --- Load Configuration from EEPROM ---
    Serial.println("[SETUP] Initializing EEPROM...");
    EEPROM.begin(32); // Allocate 32 bytes for password
    loadPassword();
    
    // --- Initialize Fingerprint Sensor ---
    Serial.println("[SETUP] Initializing Fingerprint Sensor on Serial2...");
    fingerprintSerial.begin(57600, SERIAL_8N1, FINGERPRINT_RX, FINGERPRINT_TX);
    delay(100);
    if (finger.verifyPassword()) {
        Serial.println("[SETUP] Fingerprint sensor found!");
        updateLcd("SafeBox Ready", "Pass or Fngr");
    } else {
        Serial.println("[SETUP] CRITICAL: Did not find fingerprint sensor!");
        updateLcd("Sensor Error!", "Please check wiring");
        while(1) { delay(10); } // Halt on sensor error
    }

    beep(100);
    digitalWrite(GREEN_LED_PIN, LOW);
    digitalWrite(RED_LED_PIN, HIGH);
    
    Serial.println("[SETUP] System initialization complete. Entering IDLE state.");
    currentState = STATE_IDLE;
}


// =================================================================
//                           MAIN LOOP
// =================================================================
void loop() {
    // The main loop delegates all logic to state handlers.
    // This keeps the loop clean and organizes code based on the system's current mode.
    switch (currentState) {
        case STATE_IDLE:
            handleIdleState();
            break;
        case WAITING_INPUT:
            handleWaitingInputState();
            break;
        case LOCKED_OUT:
            handleLockedOutState();a
            break;
        case AP_MODE:
            server.handleClient(); // Handle incoming web requests
            handleApModeKeypad();
            break;
        case CONFIG_MENU:
            // This state is handled by its own blocking function, so no code is needed here.
            break;
    }
}

// =================================================================
//                         STATE HANDLERS
// =================================================================

void handleIdleState() {
    // In the idle state, we are waiting for either a fingerprint or a keypad press.
    int fingerID = getFingerprintID();
    if (fingerID >= 0) {
        Serial.printf("[IDLE] Fingerprint match found for ID #%d.\n", fingerID);
        grantAccess();
        return; // Return immediately after granting access
    }

    char key = keypad.getKey();
    if (key) {
        Serial.printf("[IDLE] Key '%c' pressed. Switching to WAITING_INPUT state.\n", key);
        beep(50);
        inputBuffer = "";
        currentState = WAITING_INPUT;
        updateLcd("Enter & press #:", "");
        handleKeypadInput(key); // Process the first key immediately
    }
}

void handleWaitingInputState() {
    // This state is active after the first key is pressed. It collects subsequent key presses.
    char key = keypad.getKey();
    if (key) {
        beep(50);
        handleKeypadInput(key);
    }
}

void handleLockedOutState() {
    // This state is active after too many failed password attempts.
    unsigned long elapsed = millis() - lockStartTime;
    if (elapsed >= LOCK_TIME) {
        Serial.println("[LOCKOUT] Lockout time finished. Returning to IDLE state.");
        currentState = STATE_IDLE;
        failedAttempts = 0;
        updateLcd("SafeBox Ready", "Pass or Fngr");
        digitalWrite(RED_LED_PIN, LOW);
    } else {
        long timeLeft = (LOCK_TIME - elapsed) / 1000;
        Serial.printf("[LOCKOUT] System locked. Time left: %ld seconds.\n", timeLeft);
        updateLcd("LOCKED OUT", "Wait: " + String(timeLeft) + "s");
        delay(1000); // Update the countdown every second
    }
}

// =================================================================
//                       CORE FUNCTIONALITY
// =================================================================

void grantAccess() {
    Serial.println("[ACCESS] Access GRANTED.");
    updateLcd("Access Granted!", "");
    digitalWrite(GREEN_LED_PIN, HIGH);
    beep(150); delay(100); beep(150);

    Serial.println("[SERVO] Unlocking safe...");
    lockServo.write(SERVO_UNLOCKED_ANGLE);
    failedAttempts = 0; // Reset failed attempts on success

    updateLcd("Press # to Lock", "");
    Serial.println("[ACCESS] Waiting for '#' key to lock the safe.");

    // Timeout to avoid infinite blocking
    unsigned long startTime = millis();
    const unsigned long timeout = 100000; // 15 seconds

    while (true) {
        char key = keypad.getKey();
        if (key) {
            if (key == '#') {
                beep(100);
                break;
            } else if (key == 'C') {
                beep(100);
                Serial.println("[ACCESS] 'C' pressed. Entering config menu.");
                enterConfigMenu();
                updateLcd("Press # to Lock", "");
                startTime = millis(); // Reset timeout
            } else if (key == 'D') {
                beep(100);
                Serial.println("[ACCESS] 'D' pressed. Entering delete finger menu.");
                runDeleteFingerMenu();
                updateLcd("Press # to Lock", "");
                startTime = millis(); // Reset timeout
            } else if (key == 'A') {
                beep(100);
                Serial.println("[ACCESS] 'A' pressed. Toggling AP mode.");
                toggleApMode();
                updateLcd("Press # to Lock", "");
                startTime = millis(); // Reset timeout
            } else if (key == 'B') {
                beep(100);
                Serial.println("[ACCESS] 'B' pressed. (Chức năng tùy chỉnh ở đây)");
                // Thêm chức năng cho B nếu muốn
                updateLcd("Press # to Lock", "");
                startTime = millis(); // Reset timeout
            } else {
                Serial.printf("[ACCESS] Unhandled key '%c'. Ignored.\n", key);
            }
        }

        // Nếu chờ quá lâu thì tự động khóa két lại
        if (millis() - startTime > timeout) {
            Serial.println("[ACCESS] Timeout. Auto-locking safe.");
            break;
        }

        delay(10);
    }

    // Sau khi thoát khỏi vòng while thì tiến hành khóa két
    Serial.println("[SERVO] Locking safe...");
    lockServo.write(SERVO_LOCKED_ANGLE);
    digitalWrite(GREEN_LED_PIN, LOW);

    Serial.println("[ACCESS] Safe locked. Returning to IDLE state.");
    updateLcd("SafeBox Ready", "Pass or Fngr");
    currentState = STATE_IDLE;
    inputBuffer = "";
}

void denyAccess() {
    failedAttempts++;
    Serial.printf("[ACCESS] Access DENIED. Attempt %d of %d.\n", failedAttempts, MAX_ATTEMPTS);
    updateLcd("Access Denied!", "");
    digitalWrite(RED_LED_PIN, HIGH);
    beep(500);
    
    inputBuffer = "";
    
    delay(1500);
    digitalWrite(RED_LED_PIN, LOW);
    
    if (failedAttempts >= MAX_ATTEMPTS) {
        Serial.printf("[LOCKOUT] Maximum attempts reached. Locking system for %d seconds.\n", LOCK_TIME / 1000);
        currentState = LOCKED_OUT;
        lockStartTime = millis();
    } else {
        Serial.println("[ACCESS] Returning to IDLE state.");
        currentState = STATE_IDLE;
        updateLcd("SafeBox Ready", "Pass or Fngr");
    }
}

void checkPassword() {
    Serial.printf("[AUTH] Checking password. Input: '%s', Stored: '%s'\n", inputBuffer.c_str(), password.c_str());
    if (inputBuffer == DEFAULT_PASSWORD) {
        grantAccess();
    } else {
        denyAccess();
    }
}


// =================================================================
//                        KEYPAD HANDLING
// =================================================================

void handleKeypadInput(char key) {
    Serial.printf("[KEYPAD] Handling key '%c'. Current buffer: '%s'\n", key, inputBuffer.c_str());

    switch (key) {
        case '#':
            Serial.println("[KEYPAD] '#' pressed. Checking password...");
            checkPassword();
            return;

        case '*':
            Serial.println("[KEYPAD] '*' pressed. Cancelling input and returning to IDLE.");
            inputBuffer = "";
            currentState = STATE_IDLE;
            updateLcd("SafeBox Ready", "Pass or Fngr");
            return;

        case 'A': case 'B': case 'C': case 'D':
            Serial.printf("[KEYPAD] Key '%c' ignored. Access not granted yet.\n", key);
            beep(50);
            return;
    }

    if (inputBuffer.length() < MAX_PASSWORD_LENGTH && (isdigit(key) || isalpha(key))) {
        inputBuffer += key;
        lcd.setCursor(inputBuffer.length() - 1, 1);
        lcd.print("*");
        Serial.printf("[KEYPAD] Appended key. New buffer: '%s'\n", inputBuffer.c_str());
    }
}


void handleApModeKeypad() {
    char key = keypad.getKey();
    if (key == 'A') {
        Serial.println("[AP_MODE] 'A' key pressed. Deactivating AP mode.");
        beep(50);
        toggleApMode();
    }
}


// =================================================================
//                       WI-FI & WEB SERVER
// =================================================================

void toggleApMode() {
    isApOn = !isApOn;

    if (isApOn) {
        Serial.println("[WIFI] Starting Access Point...");

        WiFi.disconnect(true);
        delay(100);

        WiFi.softAP(AP_SSID, AP_PASS);
        delay(200);

        IPAddress ip = WiFi.softAPIP();
        Serial.print("[WIFI] AP IP address: ");
        Serial.println(ip);

        updateLcd("AP Mode Active", ip.toString());

        Serial.println("[WIFI] Setting up web server routes...");
        server.on("/", HTTP_GET, handleRoot);
        server.on("/save", HTTP_POST, handleSave);
        server.on("/addfinger", HTTP_POST, handleAddFinger);
        server.on("/deletefinger", HTTP_POST, handleDeleteFinger);
        server.begin();
        Serial.println("[WIFI] Web server started.");

        currentState = AP_MODE;
    } else {
        Serial.println("[WIFI] Stopping web server and Access Point...");
        server.stop();
        WiFi.softAPdisconnect(true);

        currentState = STATE_IDLE;
        updateLcd("AP Mode OFF", "SafeBox Ready");
        Serial.println("[WIFI] AP Mode is off. Returned to IDLE state.");
    }
}

void handleRoot() {
    Serial.println("[WEB] Received GET request for root '/'. Sending config page.");
    String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>SafeBox Config</title>
    <meta name='viewport' content='width=device-width, initial-scale=1'>
    <style>
        body { font-family: Arial, sans-serif; background-color: #f0f2f5; margin: 0; padding: 20px; }
        .container { max-width: 500px; margin: auto; background: white; padding: 20px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }
        h1, h2 { color: #333; text-align: center; }
        form { margin-bottom: 20px; }
        label { display: block; margin-bottom: 5px; font-weight: bold; }
        input[type='text'], input[type='password'], select { width: 100%; padding: 10px; margin-bottom: 10px; border: 1px solid #ddd; border-radius: 4px; box-sizing: border-box; }
        input[type='submit'] { width: 100%; background-color: #4CAF50; color: white; padding: 14px 20px; margin: 8px 0; border: none; border-radius: 4px; cursor: pointer; font-size: 16px; }
        input[type='submit']:hover { background-color: #45a049; }
        .status { background-color: #eef; padding: 10px; border-radius: 4px; text-align: center; font-weight: bold; margin-top: 15px; }
    </style>
</head>
<body>
    <div class='container'>
        <h1>SafeBox Configuration</h1>
        
        <form action='/save' method='POST'>
            <h2>Change Password</h2>
            <label for='pw'>New Password:</label>
            <input type='password' id='pw' name='pw' placeholder='Leave blank to keep current'>
            <input type='submit' value='Save Password'>
        </form>

        <form action='/addfinger' method='POST'>
            <h2>Add Fingerprint</h2>
            <label for='fid'>Select Fingerprint Slot:</label>
            <select id='fid' name='fid'>)rawliteral";
            
    for (int i = 1; i <= MAX_FINGERPRINTS; i++) {
        html += "<option value='" + String(i) + "'>Slot " + String(i) + "</option>";
    }

    html += R"rawliteral(
            </select>
            <input type='submit' value='Add Finger'>
        </form>

        <form action='/deletefinger' method='POST'>
            <h2>Delete Fingerprint</h2>
            <label for='dfid'>Select Fingerprint Slot to Delete:</label>
            <select id='dfid' name='dfid'>)rawliteral";

    for (int i = 1; i <= MAX_FINGERPRINTS; i++) {
        html += "<option value='" + String(i) + "'>Slot " + String(i) + "</option>";
    }

    html += R"rawliteral(
            </select>
            <input type='submit' value='Delete Finger' style='background-color: #f44336;'>
        </form>
    </div>
</body>
</html>
)rawliteral";
    server.send(200, "text/html", html);
}

void handleSave() {
    Serial.println("[WEB] Received POST request for /save.");
    if (server.hasArg("pw")) {
        String newPass = server.arg("pw");
        if (newPass.length() > 0 && newPass.length() <= MAX_PASSWORD_LENGTH) {
            Serial.printf("[WEB] New password received: %s. Saving...\n", newPass.c_str());
            password = newPass;
            savePassword();
            updateLcd("Password Saved!", "");
            beep(100);
        } else {
             Serial.println("[WEB] Received empty or invalid password. No changes made.");
        }
    }
    server.sendHeader("Location", "/", true);
    server.send(303, "text/plain", "");
}

void handleAddFinger() {
    Serial.println("[WEB] Received POST request for /addfinger.");
    if (server.hasArg("fid")) {
        int id = server.arg("fid").toInt();
        if (id > 0 && id <= MAX_FINGERPRINTS) {
            Serial.printf("[WEB] Starting fingerprint enrollment for ID #%d.\n", id);
            enrollFingerprint(id);
        }
    }
    server.sendHeader("Location", "/", true);
    server.send(303, "text/plain", "");
}

void handleDeleteFinger() {
    Serial.println("[WEB] Received POST request for /deletefinger.");
    if (server.hasArg("dfid")) {
        int id = server.arg("dfid").toInt();
        if (id > 0 && id <= MAX_FINGERPRINTS) {
            Serial.printf("[WEB] Deleting fingerprint for ID #%d.\n", id);
            deleteFingerprint(id);
        }
    }
    server.sendHeader("Location", "/", true);
    server.send(303, "text/plain", "");
}


// =================================================================
//                      CONFIGURATION MENUS
// =================================================================

void enterConfigMenu() {
    currentState = CONFIG_MENU;
    Serial.println("[CONFIG] Entered main configuration menu.");
    updateLcd("Config Menu:", "1:Add 2:Del 3:PW");
    
    while(true) {
        char key = keypad.getKey();
        if (key) {
            Serial.printf("[CONFIG] User selected option '%c'.\n", key);
            beep(50);
            switch(key) {
                case '1':
                    runAddFingerMenu();
                    return;
                case '2':
                    runDeleteFingerMenu();
                    return;
                case '3':
                    runChangePasswordMenu();
                    return;
                case '*':
                case '#':
                    Serial.println("[CONFIG] Exiting config menu.");
                    currentState = STATE_IDLE;
                    updateLcd("SafeBox Ready", "Pass or Fngr");
                    return;
            }
        }
    }
}

void runAddFingerMenu() {
    Serial.println("[CONFIG] Entered Add Fingerprint menu.");
    updateLcd("Place finger &", "Enter Slot (1-2)");
    int id = 0;
    while(true) {
        char key = keypad.getKey();
        if (key >= '1' && key <= '0' + MAX_FINGERPRINTS) {
            id = key - '0';
            Serial.printf("[CONFIG] Slot #%d selected for new fingerprint.\n", id);
            break;
        }
        if (key == '*' || key == '#') {
             Serial.println("[CONFIG] Add finger cancelled. Returning to IDLE.");
             currentState = STATE_IDLE;
             updateLcd("SafeBox Ready", "Pass or Fngr");
             return;
        }
    }
    enrollFingerprint(id);
    Serial.println("[CONFIG] Add finger routine finished. Returning to IDLE.");
    currentState = STATE_IDLE;
    updateLcd("SafeBox Ready", "Pass or Fngr");
}

void runDeleteFingerMenu() {
    Serial.println("[CONFIG] Entered Delete Fingerprint menu.");
    updateLcd("Delete Finger", "Enter Slot (1-2)");
    int id = 0;
    while(true) {
        char key = keypad.getKey();
        if (key >= '1' && key <= '0' + MAX_FINGERPRINTS) {
            id = key - '0';
            Serial.printf("[CONFIG] Slot #%d selected for deletion.\n", id);
            break;
        }
        if (key == '*' || key == '#') {
             Serial.println("[CONFIG] Delete finger cancelled. Returning to IDLE.");
             currentState = STATE_IDLE;
             updateLcd("SafeBox Ready", "Pass or Fngr");
             return;
        }
    }
    deleteFingerprint(id);
    Serial.println("[CONFIG] Delete finger routine finished. Returning to IDLE.");
    currentState = STATE_IDLE;
    updateLcd("SafeBox Ready", "Pass or Fngr");
}

void runChangePasswordMenu() {
    Serial.println("[CONFIG] Entered Change Password menu.");
    updateLcd("Enter Old Pass:", "");
    String oldPassInput = "";
    while(true) {
        char key = keypad.getKey();
        if(key) {
            if (key == '#') {
                Serial.printf("[CONFIG] Old password entered: '%s'.\n", oldPassInput.c_str());
                if (oldPassInput == password) {
                    Serial.println("[CONFIG] Old password correct.");
                    break;
                } else {
                    Serial.println("[CONFIG] Wrong password! Aborting change.");
                    updateLcd("Wrong Password!", "Aborting...");
                    delay(2000);
                    currentState = STATE_IDLE;
                    updateLcd("SafeBox Ready", "Pass or Fngr");
                    return;
                }
            } else if (key == '*') {
                Serial.println("[CONFIG] Password change cancelled.");
                currentState = STATE_IDLE;
                updateLcd("SafeBox Ready", "Pass or Fngr");
                return;
            } else if (oldPassInput.length() < MAX_PASSWORD_LENGTH) {
                oldPassInput += key;
                lcd.print("*");
            }
        }
    }

    Serial.println("[CONFIG] Prompting for new password.");
    updateLcd("Enter New Pass:", "");
    String newPass1 = "";
    while(true) {
        char key = keypad.getKey();
        if(key) {
            if (key == '#') break;
            if (key == '*') { /* ignore */ }
            else if (newPass1.length() < MAX_PASSWORD_LENGTH) {
                newPass1 += key;
                lcd.print("*");
            }
        }
    }
    Serial.printf("[CONFIG] New password (first entry) received: '%s'\n", newPass1.c_str());

    // Confirm new password
    Serial.println("[CONFIG] Prompting for new password confirmation.");
    updateLcd("Confirm New Pass:", "");
    String newPass2 = "";
    while(true) {
        char key = keypad.getKey();
        if(key) {
            if (key == '#') break;
            if (key == '*') { /* ignore */ }
            else if (newPass2.length() < MAX_PASSWORD_LENGTH) {
                newPass2 += key;
                lcd.print("*");
            }
        }
    }
    Serial.printf("[CONFIG] New password (second entry) received: '%s'\n", newPass2.c_str());

    if (newPass1.length() > 0 && newPass1 == newPass2) {
        Serial.println("[CONFIG] Passwords match. Saving new password.");
        password = newPass1;
        savePassword();
        updateLcd("Password Changed", "Successfully!");
        beep(100); delay(100); beep(100);
    } else {
        Serial.println("[CONFIG] Passwords do not match. Password unchanged.");
        updateLcd("Mismatch Error", "Password Unchanged");
        beep(500);
    }
    delay(2000);
    currentState = STATE_IDLE;
    updateLcd("SafeBox Ready", "Pass or Fngr");
    Serial.println("[CONFIG] Change password routine finished. Returning to IDLE.");
}

// =================================================================
//                       FINGERPRINT FUNCTIONS
// =================================================================

int getFingerprintID() {
    int result = finger.getImage();
    if (result != FINGERPRINT_OK) {
        Serial.println("[FP] Không nhận ảnh vân tay.");
        return -1;
    }

    result = finger.image2Tz();
    if (result != FINGERPRINT_OK) {
        Serial.println("[FP] Không chuyển ảnh sang mẫu.");
        return -1;
    }

    result = finger.fingerSearch();
    if (result != FINGERPRINT_OK) {
        Serial.println("[FP] Không tìm thấy vân tay khớp.");
        return -1;
    }

    Serial.print("[FP] Đã xác thực! ID: ");
    Serial.println(finger.fingerID);
    return finger.fingerID;
}

void enrollFingerprint(uint8_t id) {
    Serial.printf("[FINGER] Starting enrollment process for ID #%d.\n", id);
    updateLcd("Place finger on", "sensor...");
    digitalWrite(GREEN_LED_PIN, HIGH);
    
    Serial.println("[FINGER] Waiting for finger to enroll (image 1)...");
    while (finger.getImage() != FINGERPRINT_OK);
    beep(100);
    
    uint8_t p = finger.image2Tz(1);
    if (p != FINGERPRINT_OK) {
        Serial.printf("[FINGER] Error converting image 1: %d\n", p);
        updateLcd("Enroll Failed", "Try again");
        digitalWrite(GREEN_LED_PIN, LOW);
        delay(2000);
        return;
    }
    Serial.println("[FINGER] Image 1 converted.");

    updateLcd("Remove finger", "and place again");
    Serial.println("[FINGER] Waiting for finger to be removed...");
    delay(2000);
    while (finger.getImage() != FINGERPRINT_NOFINGER);

    updateLcd("Place same finger", "again...");
    Serial.println("[FINGER] Waiting for same finger to enroll (image 2)...");
    
    // Wait for finger again
    while (finger.getImage() != FINGERPRINT_OK);
    beep(100);

    p = finger.image2Tz(2);
    if (p != FINGERPRINT_OK) {
        Serial.printf("[FINGER] Error converting image 2: %d\n", p);
        updateLcd("Enroll Failed", "Try again");
        digitalWrite(GREEN_LED_PIN, LOW);
        delay(2000);
        return;
    }
    Serial.println("[FINGER] Image 2 converted.");

    Serial.println("[FINGER] Creating model...");
    p = finger.createModel();
    if (p != FINGERPRINT_OK) {
        Serial.printf("[FINGER] Error creating model (fingers might not match): %d\n", p);
        updateLcd("Fingers not match", "Try again");
        digitalWrite(GREEN_LED_PIN, LOW);
        delay(2000);
        return;
    }
    Serial.println("[FINGER] Model created.");

    Serial.printf("[FINGER] Storing model at ID #%d...\n", id);
    p = finger.storeModel(id);
    if (p != FINGERPRINT_OK) {
        Serial.printf("[FINGER] Error storing model: %d\n", p);
        updateLcd("Storage Error", "Try again");
        digitalWrite(GREEN_LED_PIN, LOW);
        delay(2000);
        return;
    }

    Serial.printf("[FINGER] Successfully stored fingerprint at ID #%d.\n", id);
    updateLcd("Finger Stored!", "ID: " + String(id));
    digitalWrite(GREEN_LED_PIN, LOW);
    beep(100); delay(100); beep(100);
    delay(2000);
}

void deleteFingerprint(uint8_t id) {
    Serial.printf("[FINGER] Attempting to delete fingerprint at ID #%d.\n", id);
    if (finger.deleteModel(id) == FINGERPRINT_OK) {
        Serial.printf("[FINGER] Successfully deleted fingerprint ID #%d.\n", id);
        updateLcd("Finger Deleted", "ID: " + String(id));
        beep(100);
    } else {
        Serial.printf("[FINGER] Failed to delete fingerprint ID #%d.\n", id);
        updateLcd("Delete Failed", "ID: " + String(id));
        beep(500);
    }
    delay(2000);
}

// =================================================================
//                        EEPROM FUNCTIONS
// =================================================================

void savePassword() {
    Serial.printf("[EEPROM] Saving password to EEPROM: '%s'\n", password.c_str());
    for (int i = 0; i < password.length(); i++) {
        EEPROM.write(i, password[i]);
    }
    // Add null terminator to mark the end of the string
    EEPROM.write(password.length(), 0);
    EEPROM.commit();
    Serial.println("[EEPROM] EEPROM commit successful.");
}

void loadPassword() {
    Serial.println("[EEPROM] Loading password from EEPROM...");
    password = "";
    for (int i = 0; i < MAX_PASSWORD_LENGTH + 1; i++) {
        char c = EEPROM.read(i);
        if (c == 0) { // Null terminator found
            break;
        }
        password += c;
    }
    
    if (password.length() == 0) {
        Serial.println("[EEPROM] No password found. Loading default password and saving.");
        password = DEFAULT_PASSWORD;
        savePassword();
    }
    // WARNING: Printing password to serial is a security risk in production.
    Serial.printf("[EEPROM] Password loaded: '%s'\n", password.c_str());
}

// =================================================================
//                       UTILITY FUNCTIONS
// =================================================================

void updateLcd(String line1, String line2) {
    // This function now also prints to the Serial monitor, which is
    // extremely useful for debugging without physical access to the LCD.
    Serial.printf("[LCD] Updating display -> Line 1: %s | Line 2: %s\n", line1.c_str(), line2.c_str());
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(line1);
    lcd.setCursor(0, 1);
    lcd.print(line2);
}

void beep(int duration_ms) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(duration_ms);
    digitalWrite(BUZZER_PIN, LOW);
}
