// A2UI.h (No changes from your original, as it was already well-defined for a UI class)
#ifndef A2MenuUI_H
#define A2MenuUI_H

#include "A2Config.h" // Include A2Config for access to peripheral instances

// --- A2MenuUI Class ---
// Manages the user interface, specifically menu navigation and display.
// Inherits from A2Config to easily access configured peripherals.
class A2MenuUI : public A2Config {
    private:
        // Enum to define different states of the menu.
        enum MenuState {
            MAIN_MENU,
            SUB_MENU,
            SETTINGS,
            ABOUT,
            INPUT_MODE // State for text input with cursor movement
        };
        // Static variable to keep track of the current menu state.
        static MenuState currentMenuState;
        // Static string to hold the current input being typed via keypad.
        static String currentInputBuffer;
        // Static variable to track the cursor position within currentInputBuffer.
        static uint8_t cursorPosition;
        // Timer for cursor blinking
        static unsigned long lastCursorToggleTime;
        static const unsigned long cursorBlinkInterval = 250; // milliseconds

    protected: 
        // No protected static members needed here for now.

    public:
        // Initializes the UI system (e.g., LCD, Serial debug).
        // Assumes peripherals are already configured via A2Config in main.ino.
        static void init();
        // Displays the main menu or current menu state.
        static void menu();
        // Handles input and updates the menu state.
        static void handleInput();
        // Updates the display based on the current menu state.
        static void updateDisplay();

        // Inserts a character into the current input buffer at the cursor position.
        // Automatically moves the cursor right.
        static void insertChar(char key, uint8_t limit);
        // Deletes the character at the cursor position (or before it if at end).
        // Automatically moves the cursor left if a character is deleted.
        static void deleteChar();
        // Moves the cursor left by one position.
        static void moveCursorLeft();
        // Moves the cursor right by one position.
        static void moveCursorRight();
        // Clears the current input buffer and resets cursor position.
        static void clearInputBuffer();


        // Generic service function (from original code, kept for compatibility).
        // Allows passing a function to be executed, returning its result.
        template<typename Func>
        static auto service(Func func) -> decltype(func()) {
            auto result = func();
            return result;
        }
};

#endif