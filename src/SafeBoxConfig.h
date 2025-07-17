#ifndef SAFEBOX_CONFIG_H
#define SAFEBOX_CONFIG_H

// Defines cho số lần thử mật khẩu và thời gian khóa (lưu vào EEPROM cho mật khẩu, lock time dựa trên millis)
#define MAX_ATTEMPTS 3
#define LOCK_TIME 300000UL  // 5 minutes in milliseconds)

// Mật khẩu mặc định và độ dài tối đa (mật khẩu có thể biến, nhưng giới hạn để lưu trữ)
#define DEFAULT_PASSWORD "1234"
#define MAX_PASSWORD_LENGTH 10  // Giới hạn để lưu EEPROM

// Defines cho WiFi config mode
#define AP_SSID "SafeBoxConfig"
#define AP_PASS "admin1234"  // Password for AP, can be changed in config if needed

// Defines cho serial communication commands (protocol đơn giản giữa master và slave)
#define CMD_OPEN_SERVO "OPEN_SERVO"
#define CMD_CLOSE_SERVO "CLOSE_SERVO"
#define CMD_BLINK_RED "BLINK_RED"
#define CMD_BLINK_GREEN "BLINK_GREEN"
#define CMD_BEEP_SUCCESS "BEEP_SUCCESS"
#define CMD_BEEP_ERROR "BEEP_ERROR"
#define CMD_VERIFY_RFID "VERIFY_RFID"
#define CMD_VERIFY_FINGER "VERIFY_FINGER"
#define CMD_ENROLL_RFID "ENROLL_RFID"
#define CMD_ENROLL_FINGER "ENROLL_FINGER"
#define RSP_OK "YES"
#define RSP_FAIL "NO"
#define STATUS_PREFIX "MSG_"  // Prefix for status messages from slave (e.g., "MSG_PLACE_FINGER")

// EEPROM addresses for master (ESP32)
#define EEPROM_SIZE 512
#define ADDR_ENABLE_RFID 0
#define ADDR_ENABLE_FINGER 1
#define ADDR_PASSWORD_LEN 2
#define ADDR_PASSWORD 3  // Starting address for password

// For slave (Arduino)
#define ADDR_UID_RFID 0  // 4 bytes for UID

// Dev mode
#define DEBUG_MODE TRUE

#endif // SAFEBOX_CONFIG_H
