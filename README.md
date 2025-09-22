# OLEDKeyboard Library

A feature-rich on-screen keyboard library for OLED displays with support for uppercase, lowercase, symbols, and asynchronous input handling. Perfect for ESP32, ESP8266, and Arduino projects.

![OLEDKeyboard Demo](https://img.shields.io/badge/Arduino-Library-blue) ![Version](https://img.shields.io/badge/version-1.0.0-green) ![License](https://img.shields.io/badge/license-MIT-yellow)

## Features

- **Multi-layer keyboard support**: Uppercase, lowercase, and symbols
- **Asynchronous input handling**: Non-blocking operation for multitasking
- **Flexible display compatibility**: Works with any U8g2-compatible OLED display
- **Customizable layout**: Adjustable key size, spacing, and position
- **Easy integration**: Simple API for seamless project integration
- **Memory efficient**: Optimized for microcontroller environments
- **Professional examples**: Including WiFi manager and menu systems

## Hardware Requirements

- **Microcontroller**: ESP32, ESP8266, or Arduino compatible board
- **Display**: SSD1306 OLED Display (128x64 or 128x32) - I2C
- **Input**: 3 Push buttons (UP, DOWN, SELECT)

## Supported Displays

- SSD1306 128x64
- SSD1306 128x32  
- SH1106 128x64
- Any U8g2-compatible OLED display

## Installation

### Arduino IDE

1. Download the library as a ZIP file
2. In Arduino IDE, go to **Sketch > Include Library > Add .ZIP Library**
3. Select the downloaded ZIP file
4. The library will be installed and ready to use

### PlatformIO

Add to your `platformio.ini` file:

```ini
lib_deps = 
    https://github.com/skr-electronics-lab/OLEDKeyboard.git
    olikraus/U8g2
```

## Wiring

### ESP32
```
OLED   -> ESP32
SDA    -> GPIO 21
SCL    -> GPIO 22
VCC    -> 3.3V
GND    -> GND

UP     -> GPIO 15
DOWN   -> GPIO 4
SELECT -> GPIO 16
```

### ESP8266
```
OLED   -> ESP8266
SDA    -> GPIO 4 (D2)
SCL    -> GPIO 5 (D1)
VCC    -> 3.3V
GND    -> GND

UP     -> GPIO 12 (D6)
DOWN   -> GPIO 13 (D7)
SELECT -> GPIO 14 (D5)
```

### Arduino Uno
```
OLED   -> Arduino
SDA    -> A4
SCL    -> A5
VCC    -> 5V
GND    -> GND

UP     -> D2
DOWN   -> D3
SELECT -> D4
```

## Usage

Here's a basic example of how to use the OLEDKeyboard library:

```cpp
#include <Arduino.h>
#include <U8g2lib.h>
#include <OLEDKeyboard.h>

// OLED display settings
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// Pin definitions
#define UP_PIN    15
#define DOWN_PIN  4
#define SELECT_PIN 16

// Create a keyboard instance
OLEDKeyboard keyboard(&u8g2, UP_PIN, DOWN_PIN, SELECT_PIN);

void setup() {
  Serial.begin(115200);
  u8g2.begin();
  keyboard.begin();
}

void loop() {
  if (keyboard.update()) {
    // Input is complete, get the text
    String inputText = keyboard.getInputText();
    Serial.print("Input: ");
    Serial.println(inputText);

    // Clear the input for the next entry
    keyboard.clearInput();
  }
}
```

## API Reference

### `OLEDKeyboard(U8G2* display, int upPin, int downPin, int selectPin)`
Constructor for the OLEDKeyboard class.

- `display`: A pointer to the U8g2 display object.
- `upPin`: The pin connected to the UP button.
- `downPin`: The pin connected to the DOWN button.
- `selectPin`: The pin connected to the SELECT button.

### `void begin()`
Initializes the keyboard and the display.

### `bool update()`
Updates the keyboard state, handles input, and draws the keyboard on the display. Returns `true` when the user has finished entering text.

### `String getInputText() const`
Returns the text entered by the user.

### `void clearInput()`
Clears the current input text.

### `void reset()`
Resets the keyboard to its initial state.

### `void setMaxLength(int maxLen)`
Sets the maximum length of the input text.

### `void setPosition(int x, int y)`
Sets the position of the keyboard on the display.

### `void setDebounceDelay(unsigned long delay)`
Sets the debounce delay for the buttons.

### `void setCursorBlinkInterval(unsigned long interval)`
Sets the cursor blink interval.

### `void setInputAreaHeight(int height)`
Sets the height of the input area.

### `void setKeySize(int width, int height)`
Sets the size of the keys.

### `void setKeySpacing(int horizontal, int vertical)`
Sets the spacing between the keys.

## Examples

The library includes the following examples:

- **BasicKeyboard**: A simple example demonstrating the basic functionality of the keyboard.
- **AsyncKeyboard**: Shows how to use the keyboard in a non-blocking way.
- **MenuSystem**: A more advanced example that integrates the keyboard with a menu system.
- **WiFiManager**: A practical example of how to use the keyboard to enter WiFi credentials.

## Contributing

Contributions are welcome! If you find a bug or have a feature request, please open an issue on the [GitHub repository](https://github.com/skr-electronics-lab/OLEDKeyboard).

## License

This library is licensed under the MIT License. See the [LICENSE](LICENSE) file for more details.
