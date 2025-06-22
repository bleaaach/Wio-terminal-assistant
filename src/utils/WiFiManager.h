#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>

#ifndef SIMULATOR_BUILD
#include <WiFi.h>
// Note: Wio Terminal doesn't support Preferences library
// We'll use simple variables for now
#endif

class WiFiManager {
public:
    static WiFiManager& getInstance();
    
    // WiFi operations
    bool begin();
    bool connect(const String& ssid, const String& password);
    bool connectWithSavedCredentials();
    void disconnect();
    bool isConnected();
    
    // Network information
    String getSSID();
    String getLocalIP();
    int getSignalStrength();
    int scanNetworks();
    String getScannedSSID(int index);
    int getScannedRSSI(int index);
    
    // Configuration
    void saveCredentials(const String& ssid, const String& password);
    void clearCredentials();
    bool hasCredentials();
    
    // Status
    String getStatusString();
    bool isScanning();
    bool isInitialized();

private:
    WiFiManager() = default;
    ~WiFiManager() = default;
    WiFiManager(const WiFiManager&) = delete;
    WiFiManager& operator=(const WiFiManager&) = delete;
    
    // Configuration storage (simple variables for Wio Terminal)
    String savedSSID;
    String savedPassword;
    bool hasValidCredentials;
    
    // State
    bool initialized = false;
    bool scanning = false;
    unsigned long lastScanTime = 0;
    
    // Helper functions
    void initializeWiFi();
    void handleWiFiEvent();
    void loadCredentialsFromConfig();
};

// Global instance access
#define WiFiMgr WiFiManager::getInstance()

#endif // WIFI_MANAGER_H
