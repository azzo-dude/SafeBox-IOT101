// A2UI.cpp (Skeletal implementation - YOU WILL NEED TO FILL THIS IN)
#include "A2UI.h"
#include "Arduino.h" // For millis(), Serial, etc.

// --- Static Member Definitions ---
A2MenuUI::MenuState A2MenuUI::currentMenuState = MAIN_MENU;
String A2MenuUI::currentInputBuffer = "";
uint8_t A2MenuUI::cursorPosition = 0;
unsigned long A2MenuUI::lastCursorToggleTime = 0;

void A2MenuUI::init() {
    // Initialize serial for debug output in UI if not already done by A2Config::DEBUG
    // Serial.begin(9600);
    // A2Config::DEBUG() will handle this.

    // Clear LCD and set initial state
    if (A2Config::lcd16x2I2CConfigured) {
        A2Config::getLcd().clear();
        A2Config::getLcd().home();
        A2Config::getLcd().print("Welcome!");
        A2Config::getLcd().display();
    }
    currentMenuState = MAIN_MENU;
}

void A2MenuUI::menu() {
    // This function might be used to navigate through menu items and update the display.
    // It's likely called repeatedly in the main loop().
    updateDisplay();
    handleInput();
}

void A2MenuUI::handleInput() {
    char key = A2Config::keypadInstance.readKey(); // Non-blocking read

    // Handle cursor blinking (non-blocking)
    if (millis() - lastCursorToggleTime >= cursorBlinkInterval) {
        lastCursorToggleTime = millis();
        if (A2Config::lcd16x2I2CConfigured) {
            if (currentMenuState == INPUT_MODE) {
                // Toggle cursor visibility directly on the LCD
                static bool cursorVisible = true;
                if (cursorVisible) {
                    A2Config::getLcd().cursor();
                } else {
                    A2Config::getLcd().noCursor();
                }
                cursorVisible = !cursorVisible;
            } else {
                 A2Config::getLcd().noCursor(); // Hide cursor in other modes
            }
        }
    }


    if (key != NO_KEY) {
        // Play a short beep on key press
        if (A2Config::buzzerConfigured) {
            A2Config::buzzerInstance.beep(500, 50);
        }

        switch (currentMenuState) {
            case MAIN_MENU:
                if (key == '1') {
                    currentMenuState = SUB_MENU;
                } else if (key == '2') {
                    currentMenuState = SETTINGS;
                } else if (key == '3') {
                    currentMenuState = ABOUT;
                } else if (key == '*') { // Example: Enter input mode
                    currentMenuState = INPUT_MODE;
                    clearInputBuffer();
                    // Cursor will be turned on by updateDisplay and blinking logic
                }
                break;
            case SUB_MENU:
                if (key == '#') { // Back to main menu
                    currentMenuState = MAIN_MENU;
                }
                break;
            case SETTINGS:
                if (key == '#') {
                    currentMenuState = MAIN_MENU;
                }
                break;
            case ABOUT:
                if (key == '#') {
                    currentMenuState = MAIN_MENU;
                }
                break;
            case INPUT_MODE:
                if ((key >= '0' && key <= '9') || (key >= 'A' && key <= 'D')) { // Digits and some letters
                    insertChar(key, A2Config::getLcd().getCols() * A2Config::getLcd().getRows() - 1); // Limit input to LCD screen size (e.g., 31 for 16x2, leaving space for "Enter Text:")
                } else if (key == '*') { // Delete key
                    deleteChar();
                } else if (key == 'D') { // Right cursor
                    moveCursorRight();
                } else if (key == 'A') { // Left cursor
                    moveCursorLeft();
                }
                else if (key == '#') { // Enter/Terminate input
                    A2Config::getLcd().noCursor(); // Hide cursor
                    currentMenuState = MAIN_MENU;
                    // Process currentInputBuffer here (e.g., save it, display it)
                    if (A2Config::lcd16x2I2CConfigured) {
                        A2Config::getLcd().clear();
                        A2Config::getLcd().print("Input: " + currentInputBuffer);
                        A2Config::getLcd().display(); // Ensure it's displayed
                    }
                    // For demonstration, you might clear it after processing
                    // clearInputBuffer();
                }
                break;
        }
    }
}

void A2MenuUI::updateDisplay() {
    if (!A2Config::lcd16x2I2CConfigured) {
        // Serial.println("[ERROR] LCD not configured, cannot update display.");
        return;
    }

    A2Config::getLcd().clearBuffer(); // Clear internal buffer, not physical LCD yet

    switch (currentMenuState) {
        case MAIN_MENU:
            A2Config::getLcd().printf("1.Sub Menu\n2.Settings\n");
            A2Config::getLcd().noCursor(); // Ensure cursor is off in menu mode
            break;
        case SUB_MENU:
            A2Config::getLcd().printf("Sub Menu Active\n#.Back\n");
            A2Config::getLcd().noCursor();
            break;
        case SETTINGS:
            A2Config::getLcd().printf("Settings Menu\n#.Back\n");
            A2Config::getLcd().noCursor();
            break;
        case ABOUT:
            A2Config::getLcd().printf("About This App\nVer 1.0\n");
            A2Config::getLcd().noCursor();
            break;
        case INPUT_MODE: {
            A2Config::getLcd().print("Enter Text:\n");
            A2Config::getLcd().print(currentInputBuffer); // Print the buffer content

            // Calculate actual cursor position on LCD relative to the start of input text
            // "Enter Text:\n" takes up one line (or two if it wraps, but for 16x2 it likely fits)
            // Assuming "Enter Text:" is on line 0, input starts on line 1.
            uint8_t lcd_start_row = 1; // Input starts on the second row
            uint8_t lcd_start_col = 0;

            // Calculate cursor position based on input buffer length and LCD column width
            uint8_t effectiveCursorPosition = cursorPosition; // Position within currentInputBuffer
            uint8_t cols = A2Config::getLcd().getCols();

            uint8_t cursor_row = lcd_start_row + (effectiveCursorPosition / cols);
            uint8_t cursor_col = lcd_start_col + (effectiveCursorPosition % cols);

            // Ensure cursor doesn't go off screen if buffer is very long
            if (cursor_row >= A2Config::getLcd().getRows()) {
                cursor_row = A2Config::getLcd().getRows() -1;
                cursor_col = A2Config::getLcd().getCols() -1; // Clamp to last visible char
            }


            A2Config::getLcd().setCursor(cursor_col, cursor_row);
            A2Config::getLcd().cursor(); // Turn on cursor
            break;
        }
    }
    A2Config::getLcd().display(); // Render the buffer to the physical LCD
}

void A2MenuUI::insertChar(char key, uint8_t limit) {
    if (currentInputBuffer.length() < limit) {
        // Insert character at cursor position
        currentInputBuffer.reserve(currentInputBuffer.length() + 1); // Pre-allocate memory
        currentInputBuffer = currentInputBuffer.substring(0, cursorPosition) + key + currentInputBuffer.substring(cursorPosition);
        cursorPosition++;
        updateDisplay(); // Update display immediately for better feedback
    }
}

void A2MenuUI::deleteChar() {
    if (cursorPosition > 0) {
        currentInputBuffer.remove(cursorPosition - 1, 1);
        cursorPosition--;
        updateDisplay(); // Update display immediately for better feedback
    }
}

void A2MenuUI::moveCursorLeft() {
    if (cursorPosition > 0) {
        cursorPosition--;
        updateDisplay(); // Update display to reposition cursor
    }
}

void A2MenuUI::moveCursorRight() {
    if (cursorPosition < currentInputBuffer.length()) {
        cursorPosition++;
        updateDisplay(); // Update display to reposition cursor
    }
}

void A2MenuUI::clearInputBuffer() {
    currentInputBuffer = "";
    cursorPosition = 0;
}