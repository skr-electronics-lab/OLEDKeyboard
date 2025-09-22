/*
  OLEDKeyboard.h - On-screen keyboard library for OLED displays
  
  Created by Sk Raihan, SKR Electronics Lab
  https://github.com/skr-electronics-lab
  
  This library provides an easy-to-use on-screen keyboard for OLED displays
  using the U8g2 library. Supports uppercase, lowercase, and symbol modes
  with asynchronous input handling.
*/

#ifndef OLEDKEYBOARD_H
#define OLEDKEYBOARD_H

#include <Arduino.h>
#include <U8g2lib.h>

// Keyboard states
enum KeyboardState {
  STATE_UPPERCASE,
  STATE_LOWERCASE,
  STATE_SYMBOLS
};

class OLEDKeyboard {
  public:
    // Constructor
    OLEDKeyboard(U8G2* display, int upPin, int downPin, int selectPin);
    
    // Main functions
    void begin();
    bool update();                    // Non-blocking update, returns true if input complete
    void handleInput();              // Process button inputs
    void draw();                     // Draw keyboard interface
    
    // Input management
    bool isInputComplete() const;    // Check if input is finished
    String getInputText() const;     // Get entered text
    void clearInput();               // Clear current input
    void reset();                    // Reset to initial state
    
    // Configuration
    void setMaxLength(int maxLen);   // Set maximum input length
    void setPosition(int x, int y);  // Set keyboard position
    void setDebounceDelay(unsigned long delay); // Set button debounce delay
    void setCursorBlinkInterval(unsigned long interval); // Set cursor blink speed
    
    // Display settings
    void setInputAreaHeight(int height);
    void setKeySize(int width, int height);
    void setKeySpacing(int horizontal, int vertical);
    
  private:
    // Display and pins
    U8G2* _display;
    int _upPin, _downPin, _selectPin;
    
    // Keyboard layout constants
    static const int KEY_ROWS = 4;
    static const int KEY_COLS = 8;
    static const int KEY_COUNT = KEY_ROWS * KEY_COLS;
    
    // Display dimensions and layout
    int _screenWidth, _screenHeight;
    int _inputAreaHeight;
    int _keyWidth, _keyHeight;
    int _hSpacing, _vSpacing;
    int _keyboardX, _keyboardY;
    int _maxInputLength;
    
    // State variables
    KeyboardState _currentState;
    String _inputText;
    bool _inputComplete;
    bool _cursorVisible;
    int _selectedKeyIndex;
    
    // Timing variables
    unsigned long _lastCursorBlink;
    unsigned long _lastUpPress;
    unsigned long _lastDownPress;
    unsigned long _lastSelectPress;
    unsigned long _debounceDelay;
    unsigned long _cursorBlinkInterval;
    
    // Keyboard layouts
    static const char* const _keysUpper[KEY_COUNT];
    static const char* const _keysLower[KEY_COUNT];
    static const char* const _keysSymbols[KEY_COUNT];
    
    // Private methods
    void _calculateLayout();
    void _drawInputArea();
    void _drawKeyboard();
    const char* const* _getCurrentKeys() const;
    void _processKeyPress(const char* key);
    bool _isSpecialKey(const char* key) const;
    void _handleSpecialKey(const char* key);
};

#endif