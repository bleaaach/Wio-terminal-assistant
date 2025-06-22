#include "WiFiManager.h"
#include "../config/wifi_config.h"

WiFiManager& WiFiManager::getInstance() {
    static WiFiManager instance;
    return instance;
}

bool WiFiManager::begin() {
    if (initialized) return true;

#ifndef SIMULATOR_BUILD
    Serial.println("WiFiManager: Initializing...");

    // Load credentials from config file
    loadCredentialsFromConfig();

    // Initialize WiFi
    WiFi.mode(WIFI_STA);
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);

    initialized = true;
    Serial.println("WiFiManager: Initialized successfully");

    // Try to connect with saved credentials
    if (hasCredentials()) {
        Serial.println("WiFiManager: Found saved credentials, attempting connection...");
        return connectWithSavedCredentials();
    }
#else
    Serial.println("WiFiManager: Simulator mode - WiFi always available");
    initialized = true;
    hasValidCredentials = true;
    savedSSID = "SimulatedWiFi";
    savedPassword = "password";
#endif

    return true;
}

bool WiFiManager::connect(const String& ssid, const String& password) {
#ifndef SIMULATOR_BUILD
    if (!initialized) begin();
    
    Serial.printf("WiFiManager: Connecting to %s...\n", ssid.c_str());
    
    WiFi.begin(ssid.c_str(), password.c_str());
    
    // Wait for connection with timeout
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println();
        Serial.printf("WiFiManager: Connected! IP: %s\n", WiFi.localIP().toString().c_str());
        saveCredentials(ssid, password);
        return true;
    } else {
        Serial.println();
        Serial.println("WiFiManager: Connection failed");
        return false;
    }
#else
    Serial.printf("WiFiManager: Simulated connection to %s\n", ssid.c_str());
    return true;
#endif
}

bool WiFiManager::connectWithSavedCredentials() {
#ifndef SIMULATOR_BUILD
    if (!hasCredentials()) return false;

    if (savedSSID.length() == 0) return false;

    return connect(savedSSID, savedPassword);
#else
    return true;
#endif
}

void WiFiManager::disconnect() {
#ifndef SIMULATOR_BUILD
    WiFi.disconnect();
    Serial.println("WiFiManager: Disconnected");
#else
    Serial.println("WiFiManager: Simulated disconnect");
#endif
}

bool WiFiManager::isConnected() {
#ifndef SIMULATOR_BUILD
    return WiFi.status() == WL_CONNECTED;
#else
    return true; // Always connected in simulator
#endif
}

String WiFiManager::getSSID() {
#ifndef SIMULATOR_BUILD
    return WiFi.SSID();
#else
    return "SimulatedWiFi";
#endif
}

String WiFiManager::getLocalIP() {
#ifndef SIMULATOR_BUILD
    return WiFi.localIP().toString();
#else
    return "192.168.1.100";
#endif
}

int WiFiManager::getSignalStrength() {
#ifndef SIMULATOR_BUILD
    return WiFi.RSSI();
#else
    return -45; // Good signal strength
#endif
}

int WiFiManager::scanNetworks() {
#ifndef SIMULATOR_BUILD
    if (scanning) return -1;
    
    scanning = true;
    Serial.println("WiFiManager: Scanning networks...");
    
    int n = WiFi.scanNetworks();
    
    scanning = false;
    lastScanTime = millis();
    
    Serial.printf("WiFiManager: Found %d networks\n", n);
    return n;
#else
    Serial.println("WiFiManager: Simulated network scan");
    return 3; // Return 3 simulated networks
#endif
}

String WiFiManager::getScannedSSID(int index) {
#ifndef SIMULATOR_BUILD
    return WiFi.SSID(index);
#else
    // Return simulated network names
    switch (index) {
        case 0: return "HomeWiFi";
        case 1: return "OfficeNetwork";
        case 2: return "PublicWiFi";
        default: return "Unknown";
    }
#endif
}

int WiFiManager::getScannedRSSI(int index) {
#ifndef SIMULATOR_BUILD
    return WiFi.RSSI(index);
#else
    // Return simulated signal strengths
    switch (index) {
        case 0: return -35; // Strong
        case 1: return -55; // Medium
        case 2: return -75; // Weak
        default: return -90;
    }
#endif
}

void WiFiManager::saveCredentials(const String& ssid, const String& password) {
#ifndef SIMULATOR_BUILD
    savedSSID = ssid;
    savedPassword = password;
    hasValidCredentials = true;
    Serial.printf("WiFiManager: Credentials saved for %s\n", ssid.c_str());
#else
    Serial.printf("WiFiManager: Simulated save credentials for %s\n", ssid.c_str());
#endif
}

void WiFiManager::clearCredentials() {
#ifndef SIMULATOR_BUILD
    savedSSID = "";
    savedPassword = "";
    hasValidCredentials = false;
    Serial.println("WiFiManager: Credentials cleared");
#else
    Serial.println("WiFiManager: Simulated clear credentials");
#endif
}

bool WiFiManager::hasCredentials() {
#ifndef SIMULATOR_BUILD
    return hasValidCredentials && savedSSID.length() > 0;
#else
    return true; // Always have credentials in simulator
#endif
}

String WiFiManager::getStatusString() {
#ifndef SIMULATOR_BUILD
    switch (WiFi.status()) {
        case WL_CONNECTED: return "Connected";
        case WL_NO_SSID_AVAIL: return "SSID not found";
        case WL_CONNECT_FAILED: return "Connection failed";
        case WL_CONNECTION_LOST: return "Connection lost";
        case WL_DISCONNECTED: return "Disconnected";
        case WL_IDLE_STATUS: return "Idle";
        default: return "Unknown";
    }
#else
    return "Connected (Simulated)";
#endif
}

bool WiFiManager::isScanning() {
    return scanning;
}

bool WiFiManager::isInitialized() {
    return initialized;
}

void WiFiManager::loadCredentialsFromConfig() {
    // Load WiFi credentials from config file
    savedSSID = WIFI_SSID;
    savedPassword = WIFI_PASSWORD;
    hasValidCredentials = (savedSSID.length() > 0 && savedPassword.length() > 0);

    Serial.printf("WiFiManager: Loaded config - SSID: %s, Password: %s\n",
                  savedSSID.c_str(),
                  savedPassword.length() > 0 ? "***" : "EMPTY");
}
