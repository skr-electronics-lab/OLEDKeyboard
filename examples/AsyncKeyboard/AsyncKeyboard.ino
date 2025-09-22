/*
  AsyncKeyboard Example
  
  This example demonstrates asynchronous usage of the OLEDKeyboard library.
  The keyboard runs in the background while other tasks can be performed.
  
  Hardware Requirements:
  - ESP32/ESP8266 or Arduino compatible board
  - SSD1306 OLED Display (128x64) - I2C
  - 3 Push buttons (UP, DOWN, SELECT)
  - LED (optional)
  
  Connections:
  - OLED SDA -> GPIO 21 (ESP32) or D2 (ESP8266)
  - OLED SCL -> GPIO 22 (ESP32) or D1 (ESP8266)
  - UP Button -> GPIO 2
  - DOWN Button -> GPIO 3
  - SELECT Button -> GPIO 4
  - LED -> GPIO 13 (optional)
  
  Created by Sk Raihan, SKR Electronics Lab
  https://github.com/skr-electronics-lab
*/

#include <U8g2lib.h>
#include <OLEDKeyboard.h>

// Initialize display
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// Pin definitions
#define UP_PIN 2
#define DOWN_PIN 3
#define SELECT_PIN 4
#define LED_PIN 13

// Initialize keyboard
OLEDKeyboard keyboard(&u8g2, UP_PIN, DOWN_PIN, SELECT_PIN);

// Background task variables
unsigned long lastLedToggle = 0;
bool ledState = false;
unsigned long lastSerialUpdate = 0;
int counter = 0;

// Application states
enum AppState {
  STATE_INPUT,
  STATE_PROCESSING,
  STATE_RESULT
};

AppState currentState = STATE_INPUT;
String lastInput = "";
unsigned long stateChangeTime = 0;

void setup() {
  Serial.begin(115200);
  
  // Initialize LED pin
  pinMode(LED_PIN, OUTPUT);
  
  // Initialize display
  u8g2.begin();
  
  // Initialize keyboard with custom settings
  keyboard.begin();
  keyboard.setMaxLength(15);  // Limit input to 15 characters
  keyboard.setCursorBlinkInterval(300);  // Faster cursor blink
  
  Serial.println("OLEDKeyboard Async Example");
  Serial.println("Keyboard runs in background while LED blinks");
  Serial.println("and serial counter updates");
}

void loop() {
  // Background tasks that run continuously
  backgroundTasks();
  
  // Handle application states
  switch (currentState) {
    case STATE_INPUT:
      handleInputState();
      break;
      
    case STATE_PROCESSING:
      handleProcessingState();
      break;
      
    case STATE_RESULT:
      handleResultState();
      break;
  }
  
  delay(10);
}

void backgroundTasks() {
  // Toggle LED every 500ms
  if (millis() - lastLedToggle >= 500) {
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
    lastLedToggle = millis();
  }
  
  // Update serial counter every second
  if (millis() - lastSerialUpdate >= 1000) {
    counter++;
    Serial.print("Background counter: ");
    Serial.print(counter);
    Serial.print(" | Current input: \"");
    Serial.print(keyboard.getInputText());
    Serial.println("\"");
    lastSerialUpdate = millis();
  }
}

void handleInputState() {
  // Update keyboard (non-blocking)
  if (keyboard.update()) {
    // Input completed
    lastInput = keyboard.getInputText();
    currentState = STATE_PROCESSING;
    stateChangeTime = millis();
    
    Serial.println("Input completed! Processing...");
  }
}

void handleProcessingState() {
  // Show processing screen
  u8g2.clearBuffer();
  u8g2.drawStr(0, 12, "Processing...");
  u8g2.drawStr(0, 28, lastInput.c_str());
  
  // Animated dots
  int dots = ((millis() - stateChangeTime) / 300) % 4;
  for (int i = 0; i < dots; i++) {
    u8g2.drawStr(80 + i * 6, 12, ".");
  }
  
  u8g2.sendBuffer();
  
  // Simulate processing time
  if (millis() - stateChangeTime >= 2000) {
    currentState = STATE_RESULT;
    stateChangeTime = millis();
  }
}

void handleResultState() {
  // Show result screen
  u8g2.clearBuffer();
  u8g2.drawStr(0, 12, "Result:");
  u8g2.drawStr(0, 28, "Text length:");
  
  String lengthStr = String(lastInput.length());
  u8g2.drawStr(80, 28, lengthStr.c_str());
  
  u8g2.drawStr(0, 44, "Press any button");
  u8g2.drawStr(0, 58, "for new input");
  u8g2.sendBuffer();
  
  // Check for button press
  if (!digitalRead(UP_PIN) || !digitalRead(DOWN_PIN) || !digitalRead(SELECT_PIN)) {
    // Wait for button release
    while (!digitalRead(UP_PIN) || !digitalRead(DOWN_PIN) || !digitalRead(SELECT_PIN)) {
      delay(10);
    }
    
    // Reset for new input
    keyboard.reset();
    currentState = STATE_INPUT;
    
    Serial.println("Ready for new input...");
  }
}