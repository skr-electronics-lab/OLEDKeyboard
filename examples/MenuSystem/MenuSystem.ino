/*
  MenuSystem Example
  
  This example demonstrates how to integrate OLEDKeyboard into a menu system.
  Shows how to use the keyboard for various input types in different menu options.
  
  Features:
  - Multi-level menu system
  - Text input using keyboard
  - Settings storage
  - Different input modes for different menu items
  
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
#include <EEPROM.h>

// Initialize display
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// Pin definitions
#define UP_PIN 2
#define DOWN_PIN 3
#define SELECT_PIN 4

// Initialize keyboard
OLEDKeyboard keyboard(&u8g2, UP_PIN, DOWN_PIN, SELECT_PIN);

// Menu System States
enum MenuState {
  STATE_MAIN_MENU,
  STATE_SETTINGS_MENU,
  STATE_TEXT_INPUT,
  STATE_DISPLAY_INFO,
  STATE_ABOUT
};

// Main Menu Options
enum MainMenuOption {
  MAIN_SETTINGS,
  MAIN_USER_INFO,
  MAIN_DEVICE_INFO,
  MAIN_ABOUT,
  MAIN_COUNT
};

// Settings Menu Options
enum SettingsOption {
  SETTINGS_USERNAME,
  SETTINGS_DEVICE_NAME,
  SETTINGS_RESET,
  SETTINGS_BACK,
  SETTINGS_COUNT
};

// Menu strings
const char* mainMenuItems[] = {
  "Settings",
  "User Info",
  "Device Info", 
  "About"
};

const char* settingsMenuItems[] = {
  "Username",
  "Device Name",
  "Reset All",
  "< Back"
};

// Global variables
MenuState currentState = STATE_MAIN_MENU;
MainMenuOption selectedMainMenu = MAIN_SETTINGS;
SettingsOption selectedSettingsMenu = SETTINGS_USERNAME;
int currentMenuSelection = 0;
unsigned long lastButtonPress = 0;
const unsigned long DEBOUNCE_DELAY = 200;

// Settings storage
struct Settings {
  char username[21];
  char deviceName[21];
  bool initialized;
};

Settings settings;
String tempInput = "";
int inputType = 0; // 0 = username, 1 = device name

// EEPROM addresses
const int EEPROM_SIZE = 512;
const int SETTINGS_ADDRESS = 0;

void setup() {
  Serial.begin(115200);
  
  // Initialize EEPROM
  EEPROM.begin(EEPROM_SIZE);
  
  // Initialize display
  u8g2.begin();
  
  // Initialize keyboard
  keyboard.begin();
  keyboard.setMaxLength(20);
  
  // Initialize buttons
  pinMode(UP_PIN, INPUT_PULLUP);
  pinMode(DOWN_PIN, INPUT_PULLUP);
  pinMode(SELECT_PIN, INPUT_PULLUP);
  
  // Load settings from EEPROM
  loadSettings();
  
  Serial.println("Menu System with OLEDKeyboard");
  Serial.println("Navigate with UP/DOWN, SELECT to choose");
  
  displayMainMenu();
}

void loop() {
  switch (currentState) {
    case STATE_MAIN_MENU:
      handleMainMenu();
      break;
      
    case STATE_SETTINGS_MENU:
      handleSettingsMenu();
      break;
      
    case STATE_TEXT_INPUT:
      handleTextInput();
      break;
      
    case STATE_DISPLAY_INFO:
      handleDisplayInfo();
      break;
      
    case STATE_ABOUT:
      handleAbout();
      break;
  }
  
  delay(10);
}

void handleMainMenu() {
  if (digitalRead(UP_PIN) == LOW && (millis() - lastButtonPress > DEBOUNCE_DELAY)) {
    lastButtonPress = millis();
    selectedMainMenu = (MainMenuOption)((selectedMainMenu - 1 + MAIN_COUNT) % MAIN_COUNT);
    displayMainMenu();
  }
  
  if (digitalRead(DOWN_PIN) == LOW && (millis() - lastButtonPress > DEBOUNCE_DELAY)) {
    lastButtonPress = millis();
    selectedMainMenu = (MainMenuOption)((selectedMainMenu + 1) % MAIN_COUNT);
    displayMainMenu();
  }
  
  if (digitalRead(SELECT_PIN) == LOW && (millis() - lastButtonPress > DEBOUNCE_DELAY)) {
    lastButtonPress = millis();
    executeMainMenuOption();
  }
}

void displayMainMenu() {
  u8g2.clearBuffer();
  u8g2.drawStr(0, 12, "Main Menu");
  u8g2.drawLine(0, 15, 128, 15);
  
  for (int i = 0; i < MAIN_COUNT; i++) {
    int yPos = 28 + i * 11;
    if (i == selectedMainMenu) {
      u8g2.setDrawColor(1);
      u8g2.drawBox(0, yPos - 8, 128, 10);
      u8g2.setDrawColor(0);
      u8g2.drawStr(2, yPos, mainMenuItems[i]);
      u8g2.setDrawColor(1);
    } else {
      u8g2.drawStr(2, yPos, mainMenuItems[i]);
    }
  }
  
  u8g2.sendBuffer();
}

void executeMainMenuOption() {
  switch (selectedMainMenu) {
    case MAIN_SETTINGS:
      currentState = STATE_SETTINGS_MENU;
      selectedSettingsMenu = SETTINGS_USERNAME;
      displaySettingsMenu();
      break;
      
    case MAIN_USER_INFO:
      currentState = STATE_DISPLAY_INFO;
      displayUserInfo();
      break;
      
    case MAIN_DEVICE_INFO:
      currentState = STATE_DISPLAY_INFO;
      displayDeviceInfo();
      break;
      
    case MAIN_ABOUT:
      currentState = STATE_ABOUT;
      displayAbout();
      break;
  }
}

void handleSettingsMenu() {
  if (digitalRead(UP_PIN) == LOW && (millis() - lastButtonPress > DEBOUNCE_DELAY)) {
    lastButtonPress = millis();
    selectedSettingsMenu = (SettingsOption)((selectedSettingsMenu - 1 + SETTINGS_COUNT) % SETTINGS_COUNT);
    displaySettingsMenu();
  }
  
  if (digitalRead(DOWN_PIN) == LOW && (millis() - lastButtonPress > DEBOUNCE_DELAY)) {
    lastButtonPress = millis();
    selectedSettingsMenu = (SettingsOption)((selectedSettingsMenu + 1) % SETTINGS_COUNT);
    displaySettingsMenu();
  }
  
  if (digitalRead(SELECT_PIN) == LOW && (millis() - lastButtonPress > DEBOUNCE_DELAY)) {
    lastButtonPress = millis();
    executeSettingsOption();
  }
}

void displaySettingsMenu() {
  u8g2.clearBuffer();
  u8g2.drawStr(0, 12, "Settings");
  u8g2.drawLine(0, 15, 128, 15);
  
  for (int i = 0; i < SETTINGS_COUNT; i++) {
    int yPos = 28 + i * 11;
    if (i == selectedSettingsMenu) {
      u8g2.setDrawColor(1);
      u8g2.drawBox(0, yPos - 8, 128, 10);
      u8g2.setDrawColor(0);
      u8g2.drawStr(2, yPos, settingsMenuItems[i]);
      u8g2.setDrawColor(1);
    } else {
      u8g2.drawStr(2, yPos, settingsMenuItems[i]);
    }
  }
  
  u8g2.sendBuffer();
}

void executeSettingsOption() {
  switch (selectedSettingsMenu) {
    case SETTINGS_USERNAME:
      inputType = 0;
      startTextInput("Enter Username:", settings.username);
      break;
      
    case SETTINGS_DEVICE_NAME:
      inputType = 1;
      startTextInput("Enter Device Name:", settings.deviceName);
      break;
      
    case SETTINGS_RESET:
      resetSettings();
      displaySettingsMenu();
      break;
      
    case SETTINGS_BACK:
      currentState = STATE_MAIN_MENU;
      displayMainMenu();
      break;
  }
}

void startTextInput(const char* title, const char* currentValue) {
  currentState = STATE_TEXT_INPUT;
  keyboard.reset();
  keyboard.clearInput();
  
  // Pre-fill with current value if exists
  if (strlen(currentValue) > 0) {
    // Note: This is a simplified approach. In a real implementation,
    // you might want to simulate key presses to pre-fill the text
  }
  
  u8g2.clearBuffer();
  u8g2.drawStr(0, 12, title);
  u8g2.drawStr(0, 24, "Current:");
  u8g2.drawStr(50, 24, currentValue);
  u8g2.sendBuffer();
  delay(1500);
}

void handleTextInput() {
  if (keyboard.update()) {
    tempInput = keyboard.getInputText();
    
    if (tempInput.length() > 0) {
      // Save the input based on type
      if (inputType == 0) {
        // Username
        tempInput.toCharArray(settings.username, 21);
      } else {
        // Device name
        tempInput.toCharArray(settings.deviceName, 21);
      }
      
      settings.initialized = true;
      saveSettings();
      
      // Show confirmation
      u8g2.clearBuffer();
      u8g2.drawStr(0, 12, "Saved!");
      u8g2.drawStr(0, 28, tempInput.c_str());
      u8g2.sendBuffer();
      delay(1500);
    }
    
    // Return to settings menu
    currentState = STATE_SETTINGS_MENU;
    displaySettingsMenu();
  }
}

void handleDisplayInfo() {
  // Wait for button press to return
  if (!digitalRead(UP_PIN) || !digitalRead(DOWN_PIN) || !digitalRead(SELECT_PIN)) {
    delay(300);
    while (!digitalRead(UP_PIN) || !digitalRead(DOWN_PIN) || !digitalRead(SELECT_PIN)) {
      delay(10);
    }
    currentState = STATE_MAIN_MENU;
    displayMainMenu();
  }
}

void displayUserInfo() {
  u8g2.clearBuffer();
  u8g2.drawStr(0, 12, "User Information");
  u8g2.drawLine(0, 15, 128, 15);
  
  u8g2.drawStr(0, 28, "Username:");
  if (strlen(settings.username) > 0) {
    u8g2.drawStr(0, 40, settings.username);
  } else {
    u8g2.drawStr(0, 40, "Not set");
  }
  
  u8g2.drawStr(0, 58, "Press any button");
  u8g2.sendBuffer();
}

void displayDeviceInfo() {
  u8g2.clearBuffer();
  u8g2.drawStr(0, 12, "Device Information");
  u8g2.drawLine(0, 15, 128, 15);
  
  u8g2.drawStr(0, 28, "Device Name:");
  if (strlen(settings.deviceName) > 0) {
    u8g2.drawStr(0, 40, settings.deviceName);
  } else {
    u8g2.drawStr(0, 40, "Not set");
  }
  
  u8g2.drawStr(0, 58, "Press any button");
  u8g2.sendBuffer();
}

void handleAbout() {
  // Wait for button press to return
  if (!digitalRead(UP_PIN) || !digitalRead(DOWN_PIN) || !digitalRead(SELECT_PIN)) {
    delay(300);
    while (!digitalRead(UP_PIN) || !digitalRead(DOWN_PIN) || !digitalRead(SELECT_PIN)) {
      delay(10);
    }
    currentState = STATE_MAIN_MENU;
    displayMainMenu();
  }
}

void displayAbout() {
  u8g2.clearBuffer();
  u8g2.drawStr(0, 12, "About");
  u8g2.drawLine(0, 15, 128, 15);
  u8g2.drawStr(0, 28, "OLEDKeyboard Demo");
  u8g2.drawStr(0, 40, "by SKR Electronics");
  u8g2.drawStr(0, 52, "Version 1.0");
  u8g2.drawStr(0, 64, "Press any button");
  u8g2.sendBuffer();
}

void resetSettings() {
  strcpy(settings.username, "");
  strcpy(settings.deviceName, "");
  settings.initialized = false;
  saveSettings();
  
  u8g2.clearBuffer();
  u8g2.drawStr(0, 12, "Settings Reset!");
  u8g2.drawStr(0, 28, "All data cleared");
  u8g2.sendBuffer();
  delay(2000);
}

void loadSettings() {
  EEPROM.get(SETTINGS_ADDRESS, settings);
  
  if (!settings.initialized) {
    // Initialize with defaults
    strcpy(settings.username, "");
    strcpy(settings.deviceName, "");
    settings.initialized = false;
  }
  
  Serial.println("Settings loaded");
  Serial.print("Username: ");
  Serial.println(settings.username);
  Serial.print("Device Name: ");
  Serial.println(settings.deviceName);
}

void saveSettings() {
  EEPROM.put(SETTINGS_ADDRESS, settings);
  EEPROM.commit();
  
  Serial.println("Settings saved");
}