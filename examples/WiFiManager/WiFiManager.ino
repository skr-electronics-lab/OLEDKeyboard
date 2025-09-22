/*
  WiFiManager Example
  
  This example demonstrates using OLEDKeyboard to create a WiFi manager.
  Users can scan for networks, select one, and enter the password using
  the on-screen keyboard.
  
  Features:
  - Scan for available WiFi networks
  - Select network from list
  - Enter password using on-screen keyboard
  - Connect to selected network
  - Display connection status
  
  Hardware Requirements:
  - ESP32 or ESP8266
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

#include <WiFi.h>  // Use <ESP8266WiFi.h> for ESP8266
#include <U8g2lib.h>
#include <OLEDKeyboard.h>

// Initialize display
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// Pin definitions
#define UP_PIN 2
#define DOWN_PIN 3
#define SELECT_PIN 4

// Initialize keyboard
OLEDKeyboard keyboard(&u8g2, UP_PIN, DOWN_PIN, SELECT_PIN);

// WiFi Manager States
enum WiFiState {
  STATE_MENU,
  STATE_SCANNING,
  STATE_NETWORK_LIST,
  STATE_PASSWORD_INPUT,
  STATE_CONNECTING,
  STATE_CONNECTED,
  STATE_FAILED
};

// Menu options
enum MenuOption {
  MENU_SCAN_WIFI,
  MENU_MANUAL_CONNECT,
  MENU_STATUS,
  MENU_COUNT
};

const char* menuItems[] = {
  "Scan WiFi",
  "Manual Connect",
  "WiFi Status"
};

// Global variables
WiFiState currentState = STATE_MENU;
MenuOption selectedMenu = MENU_SCAN_WIFI;
int selectedNetwork = 0;
int networkCount = 0;
String selectedSSID = "";
String wifiPassword = "";
unsigned long lastButtonPress = 0;
unsigned long stateTimer = 0;
const unsigned long DEBOUNCE_DELAY = 200;

void setup() {
  Serial.begin(115200);
  
  // Initialize display
  u8g2.begin();
  
  // Initialize keyboard
  keyboard.begin();
  keyboard.setMaxLength(30);  // WiFi passwords can be long
  
  // Initialize buttons
  pinMode(UP_PIN, INPUT_PULLUP);
  pinMode(DOWN_PIN, INPUT_PULLUP);
  pinMode(SELECT_PIN, INPUT_PULLUP);
  
  // Initialize WiFi
  WiFi.mode(WIFI_STA);
  
  Serial.println("WiFi Manager with OLEDKeyboard");
  Serial.println("Use buttons to navigate menu");
  
  displayMenu();
}

void loop() {
  switch (currentState) {
    case STATE_MENU:
      handleMenu();
      break;
      
    case STATE_SCANNING:
      handleScanning();
      break;
      
    case STATE_NETWORK_LIST:
      handleNetworkList();
      break;
      
    case STATE_PASSWORD_INPUT:
      handlePasswordInput();
      break;
      
    case STATE_CONNECTING:
      handleConnecting();
      break;
      
    case STATE_CONNECTED:
      handleConnected();
      break;
      
    case STATE_FAILED:
      handleFailed();
      break;
  }
  
  delay(10);
}

void handleMenu() {
  // Handle button inputs for menu navigation
  if (digitalRead(UP_PIN) == LOW && (millis() - lastButtonPress > DEBOUNCE_DELAY)) {
    lastButtonPress = millis();
    selectedMenu = (MenuOption)((selectedMenu - 1 + MENU_COUNT) % MENU_COUNT);
    displayMenu();
  }
  
  if (digitalRead(DOWN_PIN) == LOW && (millis() - lastButtonPress > DEBOUNCE_DELAY)) {
    lastButtonPress = millis();
    selectedMenu = (MenuOption)((selectedMenu + 1) % MENU_COUNT);
    displayMenu();
  }
  
  if (digitalRead(SELECT_PIN) == LOW && (millis() - lastButtonPress > DEBOUNCE_DELAY)) {
    lastButtonPress = millis();
    executeMenuOption();
  }
}

void displayMenu() {
  u8g2.clearBuffer();
  u8g2.drawStr(0, 12, "WiFi Manager");
  u8g2.drawLine(0, 15, 128, 15);
  
  for (int i = 0; i < MENU_COUNT; i++) {
    if (i == selectedMenu) {
      u8g2.setDrawColor(1);
      u8g2.drawBox(0, 20 + i * 12, 128, 10);
      u8g2.setDrawColor(0);
      u8g2.drawStr(2, 28 + i * 12, menuItems[i]);
      u8g2.setDrawColor(1);
    } else {
      u8g2.drawStr(2, 28 + i * 12, menuItems[i]);
    }
  }
  
  u8g2.sendBuffer();
}

void executeMenuOption() {
  switch (selectedMenu) {
    case MENU_SCAN_WIFI:
      startWiFiScan();
      break;
      
    case MENU_MANUAL_CONNECT:
      startManualConnect();
      break;
      
    case MENU_STATUS:
      showWiFiStatus();
      break;
  }
}

void startWiFiScan() {
  currentState = STATE_SCANNING;
  stateTimer = millis();
  
  u8g2.clearBuffer();
  u8g2.drawStr(0, 12, "Scanning WiFi...");
  u8g2.sendBuffer();
  
  WiFi.scanDelete();
  WiFi.scanNetworks(true);  // Async scan
  
  Serial.println("Starting WiFi scan...");
}

void handleScanning() {
  // Animate scanning
  int dots = ((millis() - stateTimer) / 300) % 4;
  u8g2.clearBuffer();
  u8g2.drawStr(0, 12, "Scanning WiFi");
  
  for (int i = 0; i < dots; i++) {
    u8g2.drawStr(80 + i * 6, 12, ".");
  }
  u8g2.sendBuffer();
  
  // Check if scan is complete
  networkCount = WiFi.scanComplete();
  if (networkCount >= 0) {
    if (networkCount == 0) {
      u8g2.clearBuffer();
      u8g2.drawStr(0, 12, "No networks found");
      u8g2.drawStr(0, 40, "Press any button");
      u8g2.drawStr(0, 52, "to return");
      u8g2.sendBuffer();
      
      waitForButtonPress();
      currentState = STATE_MENU;
      displayMenu();
    } else {
      selectedNetwork = 0;
      currentState = STATE_NETWORK_LIST;
      displayNetworkList();
    }
    
    Serial.print("Found ");
    Serial.print(networkCount);
    Serial.println(" networks");
  }
}

void handleNetworkList() {
  if (digitalRead(UP_PIN) == LOW && (millis() - lastButtonPress > DEBOUNCE_DELAY)) {
    lastButtonPress = millis();
    selectedNetwork = (selectedNetwork - 1 + networkCount) % networkCount;
    displayNetworkList();
  }
  
  if (digitalRead(DOWN_PIN) == LOW && (millis() - lastButtonPress > DEBOUNCE_DELAY)) {
    lastButtonPress = millis();
    selectedNetwork = (selectedNetwork + 1) % networkCount;
    displayNetworkList();
  }
  
  if (digitalRead(SELECT_PIN) == LOW && (millis() - lastButtonPress > DEBOUNCE_DELAY)) {
    lastButtonPress = millis();
    selectedSSID = WiFi.SSID(selectedNetwork);
    
    if (WiFi.encryptionType(selectedNetwork) == WIFI_AUTH_OPEN) {
      // Open network, connect directly
      wifiPassword = "";
      connectToWiFi();
    } else {
      // Secured network, ask for password
      startPasswordInput();
    }
  }
}

void displayNetworkList() {
  u8g2.clearBuffer();
  u8g2.drawStr(0, 12, "Select Network:");
  u8g2.drawLine(0, 15, 128, 15);
  
  // Show up to 4 networks
  int startIndex = max(0, selectedNetwork - 1);
  int endIndex = min(networkCount, startIndex + 3);
  
  for (int i = startIndex; i < endIndex; i++) {
    int yPos = 28 + (i - startIndex) * 12;
    String ssid = WiFi.SSID(i);
    
    if (ssid.length() > 18) {
      ssid = ssid.substring(0, 15) + "...";
    }
    
    if (i == selectedNetwork) {
      u8g2.setDrawColor(1);
      u8g2.drawBox(0, yPos - 8, 128, 10);
      u8g2.setDrawColor(0);
      u8g2.drawStr(2, yPos, ssid.c_str());
      
      // Show lock icon for secured networks
      if (WiFi.encryptionType(i) != WIFI_AUTH_OPEN) {
        u8g2.drawStr(110, yPos, "*");
      }
      
      u8g2.setDrawColor(1);
    } else {
      u8g2.drawStr(2, yPos, ssid.c_str());
      if (WiFi.encryptionType(i) != WIFI_AUTH_OPEN) {
        u8g2.drawStr(110, yPos, "*");
      }
    }
  }
  
  u8g2.sendBuffer();
}

void startPasswordInput() {
  currentState = STATE_PASSWORD_INPUT;
  keyboard.reset();
  keyboard.clearInput();
  
  Serial.print("Enter password for: ");
  Serial.println(selectedSSID);
}

void handlePasswordInput() {
  if (keyboard.update()) {
    wifiPassword = keyboard.getInputText();
    connectToWiFi();
  }
}

void startManualConnect() {
  // For simplicity, this example doesn't implement manual SSID entry
  // You could extend this to ask for both SSID and password
  u8g2.clearBuffer();
  u8g2.drawStr(0, 12, "Manual Connect");
  u8g2.drawStr(0, 28, "Not implemented");
  u8g2.drawStr(0, 40, "in this example");
  u8g2.drawStr(0, 52, "Press any button");
  u8g2.sendBuffer();
  
  waitForButtonPress();
  currentState = STATE_MENU;
  displayMenu();
}

void connectToWiFi() {
  currentState = STATE_CONNECTING;
  stateTimer = millis();
  
  u8g2.clearBuffer();
  u8g2.drawStr(0, 12, "Connecting to:");
  u8g2.drawStr(0, 28, selectedSSID.c_str());
  u8g2.sendBuffer();
  
  WiFi.begin(selectedSSID.c_str(), wifiPassword.c_str());
  
  Serial.print("Connecting to ");
  Serial.print(selectedSSID);
  Serial.println("...");
}

void handleConnecting() {
  // Animate connecting
  int dots = ((millis() - stateTimer) / 300) % 4;
  u8g2.clearBuffer();
  u8g2.drawStr(0, 12, "Connecting to:");
  u8g2.drawStr(0, 28, selectedSSID.c_str());
  u8g2.drawStr(0, 44, "Please wait");
  
  for (int i = 0; i < dots; i++) {
    u8g2.drawStr(80 + i * 6, 44, ".");
  }
  u8g2.sendBuffer();
  
  // Check connection status
  if (WiFi.status() == WL_CONNECTED) {
    currentState = STATE_CONNECTED;
    Serial.println("WiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else if (millis() - stateTimer > 15000) {
    // Timeout after 15 seconds
    currentState = STATE_FAILED;
    Serial.println("Connection failed!");
  }
}

void handleConnected() {
  u8g2.clearBuffer();
  u8g2.drawStr(0, 12, "Connected!");
  u8g2.drawStr(0, 24, "SSID:");
  u8g2.drawStr(30, 24, WiFi.SSID().c_str());
  u8g2.drawStr(0, 36, "IP:");
  u8g2.drawStr(20, 36, WiFi.localIP().toString().c_str());
  u8g2.drawStr(0, 52, "Press any button");
  u8g2.sendBuffer();
  
  waitForButtonPress();
  currentState = STATE_MENU;
  displayMenu();
}

void handleFailed() {
  u8g2.clearBuffer();
  u8g2.drawStr(0, 12, "Connection Failed");
  u8g2.drawStr(0, 28, "Check password");
  u8g2.drawStr(0, 40, "and try again");
  u8g2.drawStr(0, 52, "Press any button");
  u8g2.sendBuffer();
  
  waitForButtonPress();
  currentState = STATE_MENU;
  displayMenu();
}

void showWiFiStatus() {
  u8g2.clearBuffer();
  u8g2.drawStr(0, 12, "WiFi Status:");
  
  if (WiFi.status() == WL_CONNECTED) {
    u8g2.drawStr(0, 24, "Connected");
    u8g2.drawStr(0, 36, WiFi.SSID().c_str());
    u8g2.drawStr(0, 48, WiFi.localIP().toString().c_str());
  } else {
    u8g2.drawStr(0, 24, "Disconnected");
  }
  
  u8g2.drawStr(0, 60, "Press any button");
  u8g2.sendBuffer();
  
  waitForButtonPress();
  currentState = STATE_MENU;
  displayMenu();
}

void waitForButtonPress() {
  // Wait for any button press
  while (digitalRead(UP_PIN) && digitalRead(DOWN_PIN) && digitalRead(SELECT_PIN)) {
    delay(10);
  }
  
  // Wait for button release
  delay(300);
  while (!digitalRead(UP_PIN) || !digitalRead(DOWN_PIN) || !digitalRead(SELECT_PIN)) {
    delay(10);
  }
}