/*
  BasicKeyboard Example
  
  This example demonstrates the basic usage of the OLEDKeyboard library.
  It creates a simple text input interface using an OLED display.
  
  Hardware Requirements:
  - ESP32/ESP8266 or Arduino compatible board
  - SSD1306 OLED Display (128x64) - I2C
  - 3 Push buttons (UP, DOWN, SELECT)
  
  Connections:
  - OLED SDA -> GPIO 21 (ESP32) or D2 (ESP8266)
  - OLED SCL -> GPIO 22 (ESP32) or D1 (ESP8266)
  - UP Button -> GPIO 2
  - DOWN Button -> GPIO 3  
  - SELECT Button -> GPIO 4
  
  Created by Sk Raihan, SKR Electronics Lab
  https://github.com/skr-electronics-lab
*/

#include <U8g2lib.h>
#include <OLEDKeyboard.h>

// Initialize display (adjust constructor for your display type)
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// Pin definitions (change according to your wiring)
#define UP_PIN 2
#define DOWN_PIN 3
#define SELECT_PIN 4

// Initialize keyboard
OLEDKeyboard keyboard(&u8g2, UP_PIN, DOWN_PIN, SELECT_PIN);

void setup() {
  Serial.begin(115200);
  
  // Initialize display
  u8g2.begin();
  
  // Initialize keyboard
  keyboard.begin();
  
  Serial.println("OLEDKeyboard Basic Example");
  Serial.println("Use UP/DOWN to navigate, SELECT to choose");
  Serial.println("Enter text using the on-screen keyboard");
}

void loop() {
  // Update keyboard (non-blocking)
  if (keyboard.update()) {
    // Input is complete
    String userInput = keyboard.getInputText();
    
    // Display result
    u8g2.clearBuffer();
    u8g2.drawStr(0, 12, "Input Complete:");
    u8g2.drawStr(0, 28, userInput.c_str());
    u8g2.drawStr(0, 44, "Press any button");
    u8g2.drawStr(0, 58, "to continue...");
    u8g2.sendBuffer();
    
    // Print to serial
    Serial.print("User entered: ");
    Serial.println(userInput);
    
    // Wait for button press
    while (digitalRead(UP_PIN) && digitalRead(DOWN_PIN) && digitalRead(SELECT_PIN)) {
      delay(10);
    }
    
    // Wait for button release
    delay(300);
    while (!digitalRead(UP_PIN) || !digitalRead(DOWN_PIN) || !digitalRead(SELECT_PIN)) {
      delay(10);
    }
    
    // Reset keyboard for next input
    keyboard.reset();
    
    Serial.println("Ready for next input...");
  }
  
  delay(10); // Small delay to prevent excessive CPU usage
}