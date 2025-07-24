ESP32-Powered Smart SafeBoxThis project details the construction of a secure, feature-rich SafeBox using an ESP32 microcontroller. It combines multiple layers of security, including a keypad password, a fingerprint scanner, and a web-based interface for advanced configuration. It's an excellent project for intermediate to advanced electronics hobbyists interested in IoT, security systems, and embedded programming.FeaturesDual Authentication: Access can be granted via a numeric password or a registered fingerprint.Secure Lockout: The system locks down for a configurable duration after multiple failed password attempts to prevent brute-force attacks.On-the-Fly Configuration: A keypad-driven menu allows users to change the password, add new fingerprints, and delete existing ones without reprogramming.Web-Based Management: An integrated Wi-Fi Access Point (AP) mode hosts a web page for easy configuration of the password and fingerprint slots from any web browser.Visual & Audible Feedback: An I2C LCD, LEDs (green/red), and a buzzer provide clear status updates, guidance, and alerts.Persistent Storage: The password is securely stored in the ESP32's EEPROM, ensuring it's retained even after a power loss.Robust State Machine: The code is built around a clean, non-blocking state machine, making it reliable and easy to expand.Hardware RequirementsComponentDescriptionMicrocontrollerESP32 Dev Kit C (or similar)Fingerprint SensorFPM10A / R307 Optical Fingerprint ModuleKeypad4x4 Matrix Membrane KeypadDisplay16x2 I2C LCD DisplayServo MotorSG90 or MG90S Micro ServoLEDs1x Green 5mm LED, 1x Red 5mm LEDBuzzer5V Passive or Active BuzzerResistors2x 220Ω (for LEDs)Power Supply5V DC Power Supply (e.g., USB or external adapter)Breadboard & WiresFor prototyping and connectionsSoftware & LibrariesThis project is built using the Arduino IDE. You will need to install the following libraries through the Library Manager:Keypad by Mark Stanley, Alexander BrevigLiquidCrystal_I2C by Frank de BrabanderESP32Servo by Kevin HarringtonAdafruit Fingerprint Sensor Library by AdafruitEEPROM (Comes with the ESP32 board package)Wiring & PinoutAll pin connections are defined in the SafeBoxConfig.h file. Connect the components to the ESP32 as follows:Component PinESP32 PinNotesI2C LCD - SDAGPIO 21Default I2C SDA pinI2C LCD - SCLGPIO 22Default I2C SCL pinServo - SignalGPIO 13SERVO_PINGreen LED - AnodeGPIO 25GREEN_LED_PIN (via 220Ω resistor)Red LED - AnodeGPIO 26RED_LED_PIN (via 220Ω resistor)Buzzer - PositiveGPIO 27BUZZER_PINKeypad - R1GPIO 32KEYPAD_R1Keypad - R2GPIO 33KEYPAD_R2Keypad - R3GPIO 15KEYPAD_R3Keypad - R4GPIO 4KEYPAD_R4Keypad - C1GPIO 16KEYPAD_C1Keypad - C2GPIO 17KEYPAD_C2Keypad - C3GPIO 18KEYPAD_C3Keypad - C4GPIO 19KEYPAD_C4Fingerprint - TXGPIO 14 (RX2)FINGERPRINT_RX (Connect to Sensor's RX)Fingerprint - RXGPIO 12 (TX2)FINGERPRINT_TX (Connect to Sensor's TX)Note: The fingerprint sensor's TX pin connects to the ESP32's RX pin, and vice-versa. We use Serial2 to avoid conflicts with the USB programming port (Serial).Setup and InstallationInstall Arduino IDE: Download and install the latest version of the Arduino IDE.Add ESP32 Board Support: Follow the instructions here to add the ESP32 board manager to your IDE.Install Libraries: Open the Arduino IDE, go to Sketch -> Include Library -> Manage Libraries... and install all the libraries listed above.Create Project Files:Create a new sketch and name it SafeBox.ino. Copy the main code into this file.Create a new tab in the sketch (Ctrl+Shift+N) and name it SafeBoxConfig.h. This file will hold all your custom configurations.Configure SafeBoxConfig.h:// SafeBoxConfig.h

#ifndef SAFEBOX_CONFIG_H
#define SAFEBOX_CONFIG_H

// --- Wi-Fi Access Point Settings ---
#define AP_SSID "SafeBox_Config"
#define AP_PASS "12345678" // Must be at least 8 characters

// --- Security Settings ---
#define DEFAULT_PASSWORD "1234"
#define MAX_PASSWORD_LENGTH 16
#define MAX_ATTEMPTS 3
#define LOCK_TIME 30000 // 30 seconds in milliseconds

// --- Servo Settings ---
#define SERVO_LOCKED_ANGLE 0
#define SERVO_UNLOCKED_ANGLE 90

// --- Fingerprint Settings ---
#define MAX_FINGERPRINTS 10 // Max slots for fingerprints

// --- Pin Definitions ---
// LEDs & Buzzer
#define GREEN_LED_PIN 25
#define RED_LED_PIN 26
#define BUZZER_PIN 27
#define SERVO_PIN 13

// Fingerprint Sensor (Using Serial2)
#define FINGERPRINT_RX 14 // ESP32 RX2
#define FINGERPRINT_TX 12 // ESP32 TX2

// Keypad
#define KEYPAD_R1 32
#define KEYPAD_R2 33
#define KEYPAD_R3 15
#define KEYPAD_R4 4
#define KEYPAD_C1 16
#define KEYPAD_C2 17
#define KEYPAD_C3 18
#define KEYPAD_C4 19

#endif
Upload Code: Select your ESP32 board from the Tools -> Board menu, choose the correct COM port, and click the "Upload" button.How to UseNormal OperationIdle State: On startup, the LCD will display "SafeBox Ready" and "Pass or Fngr". The safe is locked.Open with Fingerprint: Place a registered finger on the sensor. If it's a match, the safe will unlock, and the LCD will show "Access Granted!".Open with Password:Press any number on the keypad to begin. The LCD will prompt "Enter & press #:".Enter your password. The input will be shown as asterisks (*).Press # to confirm.Press * at any time to cancel and return to the idle state.Locking the Safe: After the safe is opened, the LCD will show "Press # to Lock". Press the # key to re-engage the servo lock. The safe will also auto-lock after a timeout period.Configuration Menus (While Unlocked)After granting access, you can press function keys (A, B, C, D) before pressing # to lock the safe.Press C for Config Menu:The LCD will show "Config Menu: 1:Add 2:Del 3:PW".Press 1 to Add a Fingerprint. Follow the on-screen prompts to select a slot and enroll your finger.Press 2 to Delete a Fingerprint. Enter the slot number of the fingerprint you wish to remove.Press 3 to Change the Password. You will be prompted to enter the old password, then the new password twice for confirmation.Press A for AP Mode:This toggles the Wi-Fi Access Point. The LCD will display "AP Mode Active" and the ESP32's IP address (usually 192.168.4.1).Connect your phone or computer to the "SafeBox_Config" Wi-Fi network using the password defined in SafeBoxConfig.h.Open a web browser and navigate to the IP address shown on the LCD.Use the web interface to change the password or manage fingerprints.Press A again on the keypad to turn off AP mode.Code OverviewThe code is structured around a state machine managed by the SystemState enum. This ensures the main loop() is clean and logic is separated into distinct handlers.SystemState Enum: Defines the possible states:STATE_IDLE: Waiting for user interaction (fingerprint or key press).WAITING_INPUT: Actively collecting password digits from the keypad.LOCKED_OUT: Temporarily disabled after too many failed attempts.CONFIG_MENU: A blocking state for handling on-device configuration.AP_MODE: Wi-Fi Access Point is active, and the web server is running.setup(): Initializes all hardware components, loads the password from EEPROM, and verifies the fingerprint sensor connection.loop(): A simple switch statement that calls the appropriate handler function based on the currentState.State Handlers (handle...State()): Functions that contain the logic for each system state.Core Functions (grantAccess(), denyAccess()): Manage the logic for successful and failed authentication attempts.Keypad & Menus: The handleKeypadInput() function processes key presses, while the run...Menu() functions contain the logic for the configuration menus.Web Server (handle...()): Functions that serve the HTML page and process form submissions from the web interface.Fingerprint & EEPROM: Helper functions for enrolling/deleting fingerprints and saving/loading the password.Troubleshooting"Sensor Error!" on LCD: The ESP32 cannot communicate with the fingerprint sensor. Check the wiring (especially RX/TX) and ensure the sensor is powered correctly.Web Page Not Loading: Ensure your device is connected to the correct "SafeBox_Config" Wi-Fi network and that you are navigating to the correct IP address (192.168.4.1).Password Not Saving: Check the serial monitor for any EEPROM-related errors. Ensure EEPROM.begin() is called correctly.Servo Not Turning: Verify the servo has power (5V) and that the signal pin is connected correctly to the ESP32. Check the SERVO_LOCKED_ANGLE and SERVO_UNLOCKED_ANGLE values.
📄 License
MIT License — feel free to use, modify, and distribute.
