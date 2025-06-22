#ifndef AI_ASSISTANT_PAGE_H
#define AI_ASSISTANT_PAGE_H

#include "../core/PageBase.h"
#include <Arduino.h>

// AI Assistant states
enum AIState {
    AI_IDLE = 0,
    AI_LISTENING,
    AI_PROCESSING,
    AI_SPEAKING,
    AI_ERROR
};

// AI Assistant modes
enum AIMode {
    AI_VOICE_MODE = 0,
    AI_TEXT_MODE,
    AI_COMMAND_MODE
};

// AI Response structure
struct AIResponse {
    String text;
    String action;
    bool isValid;
    unsigned long timestamp;
};

class AIAssistantPage : public PageBase {
public:
    AIAssistantPage();
    virtual ~AIAssistantPage();

    // PageBase interface
    virtual void onViewLoad() override;
    virtual void onViewWillAppear() override;
    virtual void onViewDidAppear() override;
    virtual void onViewWillDisappear() override;
    virtual void onViewDidDisappear() override;

    // Button handlers (using PageBase interface)
    virtual void onButton(bool pressed) override;
    virtual void onKey(lv_dir_t direction) override;

    // Update method
    void update();

    // AI Assistant functions
    void startListening();
    void stopListening();
    void processVoiceInput();
    void sendToAI(const String& message);
    void handleAIResponse(const AIResponse& response);
    void speakResponse(const String& text);
    
    // State management
    void setState(AIState newState);
    AIState getState() const { return currentState; }
    void setMode(AIMode newMode);
    AIMode getMode() const { return currentMode; }

private:
    // UI elements
    lv_obj_t* titleLabel;
    lv_obj_t* aiContainer;
    lv_obj_t* aiAvatar;
    lv_obj_t* stateLabel;
    lv_obj_t* responseContainer;
    lv_obj_t* responseText;
    lv_obj_t* inputContainer;
    lv_obj_t* inputText;
    lv_obj_t* modeIndicator;
    lv_obj_t* instructionLabel;
    lv_obj_t* statusBar;
    lv_obj_t* volumeIndicator;
    lv_obj_t* connectionStatus;

    // AI state
    AIState currentState;
    AIMode currentMode;
    AIResponse lastResponse;
    String currentInput;
    bool isConnected;
    
    // Timing
    unsigned long stateStartTime;
    unsigned long lastActivityTime;
    static const unsigned long LISTENING_TIMEOUT = 10000; // 10 seconds
    static const unsigned long IDLE_TIMEOUT = 30000;      // 30 seconds

    // Helper functions
    void createAIUI();
    void updateStateDisplay();
    void updateModeDisplay();
    void updateConnectionStatus();
    void animateAvatar();
    void showVolumeLevel(int level);
    void displayResponse(const String& text);
    void clearResponse();
    
    // AI processing
    String processCommand(const String& command);
    bool isValidCommand(const String& command);
    
    // Voice processing (simulated)
    void startVoiceRecording();
    void stopVoiceRecording();
    String getRecordedText();
    bool isVoiceDetected();
    
    // Network communication
    bool connectToAIService();
    void disconnectFromAIService();
    String sendAIRequest(const String& message);
    
    // Built-in commands
    void executeCommand(const String& command, const String& params);
    void handleWeatherCommand();
    void handleTimeCommand();
    void handleTimerCommand(const String& params);
    void handleAlarmCommand(const String& params);
    void handleSystemCommand(const String& params);

    // Missing method declarations
    void processTextInput(const String& input);
    
    // Configuration
    String aiServiceURL;
    String aiAPIKey;
    bool voiceEnabled;
    int voiceVolume;

    // Microphone variables
    int micValue;
    int micThreshold;
    bool isRecording;
    unsigned long recordingStartTime;
    unsigned long lastMicUpdate;
    int soundLevel;
    int maxSoundLevel;
    bool micInitialized;

    void loadAIConfig();
    void saveAIConfig();

    // Microphone functions
    void updateMicrophone();
    void startMicrophoneRecording();
    void stopMicrophoneRecording();
    int getMicrophoneLevel();
    void updateSoundVisualization();

    // AI processing functions (ùùùùEcho-Mateù?ù)
    void autoStartListening();
    void processVoiceInput(int level);
    String simulateASR();
    String processWithAI(const String& input);
    String classifyIntent(const String& input);
    void executeSmartCommand(const String& command);
    
    // State colors
    lv_color_t getStateColor(AIState state);
    const char* getStateText(AIState state);
    const char* getModeText(AIMode mode);
    const char* getAvatarSymbol(AIState state);
};

#endif // AI_ASSISTANT_PAGE_H
