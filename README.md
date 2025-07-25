# ESP32-Powered Smart SafeBox

This project details the construction of a secure, feature-rich SafeBox using an ESP32 microcontroller. It combines multiple layers of security, including a keypad password, a fingerprint scanner, and a web-based interface for advanced configuration. It's an excellent project for intermediate to advanced electronics hobbyists interested in IoT, security systems, and embedded programming.

![Asgore](https://media.tenor.com/bx0TiX7KlkwAAAAM/asgore-undertale.gif)
![BlackHacker](https://media.tenor.com/CgGUXc-LDc4AAAAM/hacker-pc.gif)
![SafeBox](image/Pakalu_Papito.jpg)

## Features

* **Dual Authentication:** Access can be granted via a numeric password or a registered fingerprint.
* **Secure Lockout:** The system locks down for a configurable duration after multiple failed password attempts to prevent brute-force attacks.
* **On-the-Fly Configuration:** A keypad-driven menu allows users to change the password, add new fingerprints, and delete existing ones without reprogramming.
* **Web-Based Management:** An integrated Wi-Fi Access Point (AP) mode hosts a web page for easy configuration of the password and fingerprint slots from any web browser.
* **Visual & Audible Feedback:** An I2C LCD, LEDs (green/red), and a buzzer provide clear status updates, guidance, and alerts.
* **Persistent Storage:** The password is securely stored in the ESP32's EEPROM, ensuring it's retained even after a power loss.
* **Robust State Machine:** The code is built around a clean, non-blocking state machine, making it reliable and easy to expand.

## Hardware Requirements

| Component              | Description                                      |
| ---------------------- | ------------------------------------------------ |
| **Microcontroller** | ESP32 Dev Kit C (or similar)                     |
| **Fingerprint Sensor** | FPM10A / R307 Optical Fingerprint Module         |
| **Keypad** | 4x4 Matrix Membrane Keypad                       |
| **Display** | 16x2 I2C LCD Display                             |
| **Servo Motor** | SG90 or MG90S Micro Servo                        |
| **LEDs** | 1x Green 5mm LED, 1x Red 5mm LED                 |
| **Buzzer** | 5V Passive or Active Buzzer                      |
| **Resistors** | 2x 220Ω (for LEDs)                               |
| **Power Supply** | 5V DC Power Supply (e.g., USB or external adapter) |
| **Breadboard & Wires** | For prototyping and connections                  |

## Software & Libraries

This project is built using the Arduino IDE. You will need to install the following libraries through the Library Manager:

* `Keypad` by Mark Stanley, Alexander Brevig
* `LiquidCrystal_I2C` by Frank de Brabander
* `ESP32Servo` by Kevin Harrington
* `Adafruit Fingerprint Sensor Library` by Adafruit
* `EEPROM` (Comes with the ESP32 board package)

## Wiring & Pinout

All pin connections are defined in the `SafeBoxConfig.h` file. Connect the components to the ESP32 as follows:

| Component Pin        | ESP32 Pin      | Notes                                    |
| -------------------- | -------------- | ---------------------------------------- |
| **I2C LCD - SDA** | GPIO 21        | Default I2C SDA pin                      |
| **I2C LCD - SCL** | GPIO 22        | Default I2C SCL pin                      |
| **Servo - Signal** | GPIO 13        | `SERVO_PIN`                              |
| **Green LED - Anode**| GPIO 25        | `GREEN_LED_PIN` (via 220Ω resistor)      |
| **Red LED - Anode** | GPIO 26        | `RED_LED_PIN` (via 220Ω resistor)        |
| **Buzzer - Positive**| GPIO 27        | `BUZZER_PIN`                             |
| **Keypad - R1** | GPIO 32        | `KEYPAD_R1`                              |
| **Keypad - R2** | GPIO 33        | `KEYPAD_R2`                              |
| **Keypad - R3** | GPIO 15        | `KEYPAD_R3`                              |
| **Keypad - R4** | GPIO 4         | `KEYPAD_R4`                              |
| **Keypad - C1** | GPIO 16        | `KEYPAD_C1`                              |
| **Keypad - C2** | GPIO 17        | `KEYPAD_C2`                              |
| **Keypad - C3** | GPIO 18        | `KEYPAD_C3`                              |
| **Keypad - C4** | GPIO 19        | `KEYPAD_C4`                              |
| **Fingerprint - TX** | GPIO 14 (RX2)  | `FINGERPRINT_RX` (Connect to Sensor's RX)|
| **Fingerprint - RX** | GPIO 12 (TX2)  | `FINGERPRINT_TX` (Connect to Sensor's TX)|

**Note:** The fingerprint sensor's TX pin connects to the ESP32's RX pin, and vice-versa. We use `Serial2` to avoid conflicts with the USB programming port (`Serial`).

## Setup and Installation

1.  **Install Arduino IDE:** Download and install the latest version of the [Arduino IDE](https://www.arduino.cc/en/software).
2.  **Add ESP32 Board Support:** Follow the instructions [here](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html) to add the ESP32 board manager to your IDE.
3.  **Install Libraries:** Open the Arduino IDE, go to `Sketch -> Include Library -> Manage Libraries...` and install all the libraries listed above.
4.  **Create Project Files:**
    * Create a new sketch and name it `SafeBox.ino`. Copy the main code into this file.
    * Create a new tab in the sketch (`Ctrl+Shift+N`) and name it `SafeBoxConfig.h`. This file will hold all your custom configurations.
5.  **Configure `SafeBoxConfig.h`:**

    ```cpp
    // SafeBoxConfig.h

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
    #define GREEN_LED_PIN 23
    #define RED_LED_PIN 19
    #define BUZZER_PIN 5
    #define SERVO_PIN 4

    #define FINGERPRINT_RX 16
    #define FINGERPRINT_TX 12

    // Keypad
    #define KEYPAD_R1 32
    #define KEYPAD_R2 33
    #define KEYPAD_R3 25
    #define KEYPAD_R4 26
    #define KEYPAD_C1 27
    #define KEYPAD_C2 14
    #define KEYPAD_C3 12
    #define KEYPAD_C4 13
    
    #endif
    ```

6.  **Upload Code:** Select your ESP32 board from the `Tools -> Board` menu, choose the correct COM port, and click the "Upload" button.

## How to Use

### Normal Operation

1.  **Idle State:** On startup, the LCD will display "SafeBox Ready" and "Pass or Fngr". The safe is locked.
2.  **Open with Fingerprint:** Place a registered finger on the sensor. If it's a match, the safe will unlock, and the LCD will show "Access Granted!".
3.  **Open with Password:**
    * Press any number on the keypad to begin. The LCD will prompt "Enter & press #:".
    * Enter your password. The input will be shown as asterisks (`*`).
    * Press `#` to confirm.
    * Press `*` at any time to cancel and return to the idle state.
4.  **Locking the Safe:** After the safe is opened, the LCD will show "Press # to Lock". Press the `#` key to re-engage the servo lock. The safe will also auto-lock after a timeout period.

### Configuration Menus (While Unlocked)

After granting access, you can press function keys (`A`, `B`, `C`, `D`) before pressing `#` to lock the safe.

* **Press `C` for Config Menu:**
    * The LCD will show "Config Menu: 1:Add 2:Del 3:PW".
    * Press `1` to **Add a Fingerprint**. Follow the on-screen prompts to select a slot and enroll your finger.
    * Press `2` to **Delete a Fingerprint**. Enter the slot number of the fingerprint you wish to remove.
    * Press `3` to **Change the Password**. You will be prompted to enter the old password, then the new password twice for confirmation.
* **Press `A` for AP Mode:**
    * This toggles the Wi-Fi Access Point. The LCD will display "AP Mode Active" and the ESP32's IP address (usually `192.168.4.1`).
    * Connect your phone or computer to the "SafeBox_Config" Wi-Fi network using the password defined in `SafeBoxConfig.h`.
    * Open a web browser and navigate to the IP address shown on the LCD.
    * Use the web interface to change the password or manage fingerprints.
    * Press `A` again on the keypad to turn off AP mode.

## Code Overview

The code is structured around a state machine managed by the `SystemState` enum. This ensures the main `loop()` is clean and logic is separated into distinct handlers.

* **`SystemState` Enum:** Defines the possible states:
    * `STATE_IDLE`: Waiting for user interaction (fingerprint or key press).
    * `WAITING_INPUT`: Actively collecting password digits from the keypad.
    * `LOCKED_OUT`: Temporarily disabled after too many failed attempts.
    * `CONFIG_MENU`: A blocking state for handling on-device configuration.
    * `AP_MODE`: Wi-Fi Access Point is active, and the web server is running.
* **`setup()`:** Initializes all hardware components, loads the password from EEPROM, and verifies the fingerprint sensor connection.
* **`loop()`:** A simple `switch` statement that calls the appropriate handler function based on the `currentState`.
* **State Handlers (`handle...State()`):** Functions that contain the logic for each system state.
* **Core Functions (`grantAccess()`, `denyAccess()`):** Manage the logic for successful and failed authentication attempts.
* **Keypad & Menus:** The `handleKeypadInput()` function processes key presses, while the `run...Menu()` functions contain the logic for the configuration menus.
* **Web Server (`handle...()`):** Functions that serve the HTML page and process form submissions from the web interface.
* **Fingerprint & EEPROM:** Helper functions for enrolling/deleting fingerprints and saving/loading the password.

## Troubleshooting

* **"Sensor Error!" on LCD:** The ESP32 cannot communicate with the fingerprint sensor. Check the wiring (especially RX/TX) and ensure the sensor is powered correctly.
* **Web Page Not Loading:** Ensure your device is connected to the correct "SafeBox_Config" Wi-Fi network and that you are navigating to the correct IP address (`192.168.4.1`).
* **Password Not Saving:** Check the serial monitor for any EEPROM-related errors. Ensure `EEPROM.begin()` is called correctly.
* **Servo Not Turning:** Verify the servo has power (5V) and that the signal pin is connected correctly to the ESP32. Check the `SERVO_LOCKED_ANGLE` and `SERVO_UNLOCKED_ANGLE` values.

📄 License
MIT License — feel free to use, modify, and distribute.
