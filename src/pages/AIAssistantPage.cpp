#include "AIAssistantPage.h"

AIAssistantPage::AIAssistantPage() {
    titleLabel = nullptr;
    aiContainer = nullptr;
    aiAvatar = nullptr;
    stateLabel = nullptr;
    responseContainer = nullptr;
    responseText = nullptr;
    modeIndicator = nullptr;
    connectionStatus = nullptr;
    instructionLabel = nullptr;
    volumeIndicator = nullptr;
    statusBar = nullptr;
    inputText = nullptr;
    inputContainer = nullptr;
    
    currentState = AI_IDLE;
    currentMode = AI_VOICE_MODE;
    stateStartTime = 0;
    lastActivityTime = 0;
    isConnected = false;
    
    // Initialize microphone variables (基于Echo-Mate的VAD设计)
    micValue = 0;
    micThreshold = 150;  // 语音检测阈值 (调整为更敏感)
    isRecording = false;
    recordingStartTime = 0;
    lastMicUpdate = 0;
    soundLevel = 0;
    maxSoundLevel = 1024;

    // 延迟初始化，提高页面加载速度
    micInitialized = false;
    voiceEnabled = true;
    voiceVolume = 80;

    // Initialize lastResponse
    lastResponse.text = "";
    lastResponse.action = "none";
    lastResponse.isValid = false;
    lastResponse.timestamp = 0;
}

AIAssistantPage::~AIAssistantPage() {
    // Destructor - cleanup handled by PageManager
}

void AIAssistantPage::onViewLoad() {
    Serial.println("AIAssistantPage: onViewLoad");
}

void AIAssistantPage::onViewWillAppear() {
    Serial.println("AIAssistantPage: onViewWillAppear");

    // 延迟初始化麦克风和配置 (优化加载速度)
    if (!micInitialized) {
        pinMode(WIO_MIC, INPUT);
        loadAIConfig();
        micInitialized = true;
        Serial.println("AI: Microphone and config initialized");
    }

    // 只在UI未创建时创建，避免重复创建
    if (!titleLabel) {
        createAIUI();
        setState(AI_IDLE);
        updateConnectionStatus();
    }
}

void AIAssistantPage::onViewDidAppear() {
    Serial.println("AIAssistantPage: onViewDidAppear");
    lastActivityTime = millis();
}

void AIAssistantPage::onViewWillDisappear() {
    Serial.println("AIAssistantPage: onViewWillDisappear");
    if (currentState == AI_LISTENING) {
        stopListening();
    }
}

void AIAssistantPage::onViewDidDisappear() {
    Serial.println("AIAssistantPage: onViewDidDisappear");
}

void AIAssistantPage::update() {
    unsigned long currentTime = millis();
    
    // Update microphone (基于Echo-Mate的VAD实现)
    updateMicrophone();
    
    // Handle state timeouts
    switch (currentState) {
        case AI_LISTENING:
            if (currentTime - stateStartTime > LISTENING_TIMEOUT) {
                Serial.println("AI: Listening timeout");
                setState(AI_IDLE);
            }
            break;
            
        case AI_IDLE:
            if (currentTime - lastActivityTime > IDLE_TIMEOUT) {
                // Show idle animation or tips
                animateAvatar();
                lastActivityTime = currentTime;
            }
            break;
            
        default:
            break;
    }
    
    // Update UI animations
    animateAvatar();
    updateSoundVisualization();
}

// 麦克风功能实现 (基于Echo-Mate的VAD和语音检测)
void AIAssistantPage::updateMicrophone() {
    unsigned long currentTime = millis();
    
    // 每50ms更新一次麦克风读数 (类似Echo-Mate的实时检测)
    if (currentTime - lastMicUpdate >= 50) {
        lastMicUpdate = currentTime;
        
        micValue = analogRead(WIO_MIC);
        
        // 计算声音级别 (0-100) - 参考Echo-Mate的VAD算法
        soundLevel = map(micValue, 0, maxSoundLevel, 0, 100);
        soundLevel = constrain(soundLevel, 0, 100);
        
        // 语音端点检测 (VAD) - 类似Echo-Mate的FSMN-VAD功能
        if (currentState == AI_IDLE && soundLevel > micThreshold) {
            Serial.printf("AI: Voice Activity Detected! Level: %d\n", soundLevel);
            // 可以选择自动触发或手动触发
            // autoStartListening(); // 取消注释以启用自动唤醒
        }
        
        // 在监听状态时显示声音级别和进行语音处理
        if (currentState == AI_LISTENING) {
            processVoiceInput(soundLevel);
        }
    }
}

// 自动语音唤醒功能 (参考Echo-Mate的VAD触发)
void AIAssistantPage::autoStartListening() {
    if (currentState == AI_IDLE) {
        setState(AI_LISTENING);
        startMicrophoneRecording();
        Serial.println("AI: Auto-triggered by voice activity");
        displayResponse("Voice detected! Listening...");
    }
}

// 语音输入处理 (参考Echo-Mate的SenseVoice ASR)
void AIAssistantPage::processVoiceInput(int level) {
    static unsigned long silenceStart = 0;
    static bool wasSpeaking = false;
    
    // 检测语音结束 (类似Echo-Mate的端点检测)
    if (level < micThreshold) {
        if (wasSpeaking && silenceStart == 0) {
            silenceStart = millis();
        }
        
        // 静音超过2秒，认为语音结束
        if (silenceStart > 0 && millis() - silenceStart > 2000) {
            Serial.println("AI: Voice input completed (silence detected)");
            stopListening();
            silenceStart = 0;
            wasSpeaking = false;
        }
    } else {
        // 检测到声音，重置静音计时
        silenceStart = 0;
        wasSpeaking = true;
        Serial.printf("AI: Recording... Sound level: %d\n", level);
    }
}

void AIAssistantPage::startMicrophoneRecording() {
    isRecording = true;
    recordingStartTime = millis();
    Serial.println("AI: Started microphone recording");
}

void AIAssistantPage::stopMicrophoneRecording() {
    isRecording = false;
    Serial.println("AI: Stopped microphone recording");
}

int AIAssistantPage::getMicrophoneLevel() {
    return soundLevel;
}

void AIAssistantPage::updateSoundVisualization() {
    // 根据声音级别更新头像边框 (类似Echo-Mate的视觉反馈)
    if (currentState == AI_LISTENING && aiContainer) {
        // 声音级别越高，边框越粗，颜色越亮
        int borderWidth = map(soundLevel, 0, 100, 2, 8);
        uint32_t color = soundLevel > micThreshold ? 0x00FF00 : 0x2196F3; // 绿色表示检测到语音
        
        lv_obj_set_style_border_width(aiContainer, borderWidth, 0);
        lv_obj_set_style_border_color(aiContainer, lv_color_hex(color), 0);
    } else if (aiContainer) {
        // 非监听状态时恢复正常边框
        lv_obj_set_style_border_width(aiContainer, 0, 0);
    }
}

void AIAssistantPage::onButton(bool pressed) {
    if (!pressed) return;

    // A键: 手动开始/停止监听
    if (currentState == AI_IDLE) {
        startListening();
    } else if (currentState == AI_LISTENING) {
        stopListening();
    }

    Serial.printf("AI: Button pressed - State: %s\n", getStateText(currentState));
}

void AIAssistantPage::onKey(lv_dir_t direction) {
    switch (direction) {
        case LV_DIR_TOP:
            // 摇杆按键: 手动开始/停止监听
            if (currentState == AI_IDLE) {
                startListening();
            } else if (currentState == AI_LISTENING) {
                stopListening();
            }
            break;

        case LV_DIR_LEFT:
            // B键: 快速天气查询 (参考Echo-Mate的指令分类)
            if (currentState == AI_IDLE) {
                executeSmartCommand("weather");
            }
            break;

        case LV_DIR_RIGHT:
            // C键: 快速时间查询
            if (currentState == AI_IDLE) {
                executeSmartCommand("time");
            }
            break;

        default:
            break;
    }
}

void AIAssistantPage::startListening() {
    if (currentState != AI_IDLE) return;
    
    setState(AI_LISTENING);
    startMicrophoneRecording();
    Serial.println("AI: Manual listening started");
    
    displayResponse("Listening... Speak now!");
}

void AIAssistantPage::stopListening() {
    if (currentState != AI_LISTENING) return;
    
    setState(AI_PROCESSING);
    stopMicrophoneRecording();
    Serial.println("AI: Processing voice input...");
    
    displayResponse("Processing your request...");
    
    // 模拟语音识别和AI处理 (参考Echo-Mate的ASR+LLM流程)
    String recognizedText = simulateASR();
    String aiResponse = processWithAI(recognizedText);
    
    AIResponse response;
    response.text = aiResponse;
    response.action = classifyIntent(recognizedText); // 参考Echo-Mate的FastText分类
    response.isValid = true;
    response.timestamp = millis();
    
    // 使用非阻塞延迟，提高响应速度
    unsigned long processingStart = millis();
    while (millis() - processingStart < 500) {
        // 允许其他任务运行
        lv_task_handler();
        delay(10);
    }

    handleAIResponse(response);
}

// 模拟语音识别 (ASR) - 参考Echo-Mate的SenseVoice
String AIAssistantPage::simulateASR() {
    // 基于声音级别模拟识别结果
    if (soundLevel > 80) {
        return "What's the weather like today?";
    } else if (soundLevel > 60) {
        return "What time is it now?";
    } else if (soundLevel > 40) {
        return "Set a timer for 5 minutes";
    } else {
        return "Hello, how are you?";
    }
}

// AI智能处理 (参考Echo-Mate的LLM集成)
String AIAssistantPage::processWithAI(const String& input) {
    Serial.printf("AI: Processing input: %s\n", input.c_str());

    // 简单的关键词匹配和智能回复
    String lowerInput = input;
    lowerInput.toLowerCase();

    if (lowerInput.indexOf("weather") >= 0) {
        return "Today is sunny with 25°C. Perfect weather for outdoor activities!";
    } else if (lowerInput.indexOf("time") >= 0) {
        unsigned long currentTime = millis();
        unsigned long hours = (currentTime / 3600000) % 24;
        unsigned long minutes = (currentTime / 60000) % 60;
        return "Current time is " + String(hours) + ":" + String(minutes < 10 ? "0" : "") + String(minutes);
    } else if (lowerInput.indexOf("timer") >= 0) {
        return "Timer set for 5 minutes. I'll remind you when it's done!";
    } else if (lowerInput.indexOf("hello") >= 0 || lowerInput.indexOf("hi") >= 0) {
        return "Hello! I'm your AI assistant. How can I help you today?";
    } else {
        return "I heard: \"" + input + "\". Sound level was " + String(getMicrophoneLevel()) + "%. How can I assist you?";
    }
}

// 意图分类 (参考Echo-Mate的FastText分类)
String AIAssistantPage::classifyIntent(const String& input) {
    String lowerInput = input;
    lowerInput.toLowerCase();

    if (lowerInput.indexOf("weather") >= 0) return "weather";
    if (lowerInput.indexOf("time") >= 0) return "time";
    if (lowerInput.indexOf("timer") >= 0) return "timer";
    if (lowerInput.indexOf("alarm") >= 0) return "alarm";

    return "chat"; // 默认为聊天
}

// 智能命令执行 (参考Echo-Mate的指令处理)
void AIAssistantPage::executeSmartCommand(const String& command) {
    setState(AI_PROCESSING);
    displayResponse("Processing " + command + " command...");

    // 减少延迟，提高响应速度
    delay(200);

    if (command == "weather") {
        handleWeatherCommand();
    } else if (command == "time") {
        handleTimeCommand();
    } else {
        setState(AI_SPEAKING);
        displayResponse("Command: " + command + " executed!");
        delay(1500);
        setState(AI_IDLE);
    }
}

void AIAssistantPage::handleAIResponse(const AIResponse& response) {
    if (!response.isValid) {
        setState(AI_ERROR);
        displayResponse("Sorry, I didn't understand that.");
        delay(1500);
        setState(AI_IDLE);
        return;
    }

    lastResponse = response;
    setState(AI_SPEAKING);
    displayResponse(response.text);

    // 执行任何动作
    if (response.action != "none" && response.action != "chat") {
        // 减少延迟，提高响应速度
        delay(500);
        executeCommand(response.action, "");
    } else {
        // 说话后返回idle状态，减少等待时间
        delay(2000);
        setState(AI_IDLE);
    }
}

void AIAssistantPage::setState(AIState newState) {
    currentState = newState;
    stateStartTime = millis();
    lastActivityTime = millis();
    updateStateDisplay();
}

void AIAssistantPage::setMode(AIMode newMode) {
    currentMode = newMode;
    updateModeDisplay();
}

void AIAssistantPage::createAIUI() {
    // Page title (紫色主题)
    titleLabel = lv_label_create(_root);
    lv_label_set_text_fmt(titleLabel, "%s AI ASSISTANT", LV_SYMBOL_SETTINGS);
    lv_obj_align(titleLabel, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_text_font(titleLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(titleLabel, lv_color_hex(0x9C27B0), 0); // Purple theme

    // AI头像 (AI特色设计 - 圆形头像)
    aiContainer = lv_obj_create(_root);
    lv_obj_set_size(aiContainer, 80, 80);
    lv_obj_align(aiContainer, LV_ALIGN_TOP_LEFT, 20, 40);
    lv_obj_set_style_bg_color(aiContainer, lv_color_hex(0x9C27B0), 0);
    lv_obj_set_style_radius(aiContainer, 40, 0); // 圆形头像
    lv_obj_set_style_border_width(aiContainer, 0, 0);
    lv_obj_clear_flag(aiContainer, LV_OBJ_FLAG_SCROLLABLE); // 禁用滚动条

    // AI Avatar图标
    aiAvatar = lv_label_create(aiContainer);
    lv_label_set_text(aiAvatar, LV_SYMBOL_SETTINGS);
    lv_obj_align(aiAvatar, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_font(aiAvatar, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(aiAvatar, lv_color_hex(0xFFFFFF), 0);

    // 状态显示 (头像旁边)
    stateLabel = lv_label_create(_root);
    lv_label_set_text(stateLabel, "Ready");
    lv_obj_align(stateLabel, LV_ALIGN_TOP_LEFT, 110, 50);
    lv_obj_set_style_text_font(stateLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(stateLabel, lv_color_hex(0x9C27B0), 0);

    // 模式指示器
    modeIndicator = lv_label_create(_root);
    lv_label_set_text(modeIndicator, "Voice Mode");
    lv_obj_align(modeIndicator, LV_ALIGN_TOP_LEFT, 110, 70);
    lv_obj_set_style_text_font(modeIndicator, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(modeIndicator, lv_color_hex(0x666666), 0);

    // 对话气泡容器 (AI特色 - 聊天界面)
    responseContainer = lv_obj_create(_root);
    lv_obj_set_size(responseContainer, 260, 80);
    lv_obj_align(responseContainer, LV_ALIGN_TOP_MID, 0, 140);
    lv_obj_set_style_bg_color(responseContainer, lv_color_hex(0xE1BEE7), 0); // 浅紫色气泡
    lv_obj_set_style_border_width(responseContainer, 0, 0);
    lv_obj_set_style_radius(responseContainer, 20, 0); // 圆润的气泡
    lv_obj_set_style_pad_all(responseContainer, 12, 0);
    lv_obj_clear_flag(responseContainer, LV_OBJ_FLAG_SCROLLABLE); // 禁用滚动条

    // 对话文本
    responseText = lv_label_create(responseContainer);
    lv_label_set_text(responseText, "Hi! I'm your AI assistant with voice recognition. Press A to talk!");
    lv_obj_align(responseText, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_font(responseText, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(responseText, lv_color_hex(0x4A148C), 0);
    lv_label_set_long_mode(responseText, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(responseText, 240);

    // 连接状态 (右上角)
    connectionStatus = lv_label_create(_root);
    lv_label_set_text(connectionStatus, LV_SYMBOL_WIFI " Offline");
    lv_obj_align(connectionStatus, LV_ALIGN_TOP_RIGHT, -10, 40);
    lv_obj_set_style_text_font(connectionStatus, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(connectionStatus, lv_color_hex(0xFF5722), 0);

    // 控制说明 (底部)
    instructionLabel = lv_label_create(_root);
    lv_label_set_text(instructionLabel, "A: Talk  B: Weather  C: Time");
    lv_obj_align(instructionLabel, LV_ALIGN_BOTTOM_MID, 0, -5);
    lv_obj_set_style_text_font(instructionLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(instructionLabel, lv_color_hex(0x9C27B0), 0);

    updateStateDisplay();
    updateModeDisplay();
}

void AIAssistantPage::updateStateDisplay() {
    if (!stateLabel) return;

    lv_label_set_text(stateLabel, getStateText(currentState));

    if (aiAvatar) {
        lv_label_set_text(aiAvatar, getAvatarSymbol(currentState));
    }

    if (aiContainer) {
        lv_obj_set_style_bg_color(aiContainer, getStateColor(currentState), 0);
    }

    // 更新指令标签 (基于Echo-Mate的状态反馈)
    if (instructionLabel) {
        switch (currentState) {
            case AI_IDLE:
                lv_label_set_text(instructionLabel, "A: Talk  B: Weather  C: Time");
                break;
            case AI_LISTENING:
                lv_label_set_text(instructionLabel, "Listening... Speak clearly!");
                break;
            case AI_PROCESSING:
                lv_label_set_text(instructionLabel, "Processing with AI...");
                break;
            case AI_SPEAKING:
                lv_label_set_text(instructionLabel, "AI is responding...");
                break;
            case AI_ERROR:
                lv_label_set_text(instructionLabel, "Error - Press A to retry");
                break;
        }
    }
}

void AIAssistantPage::updateModeDisplay() {
    if (!modeIndicator) return;

    lv_label_set_text(modeIndicator, getModeText(currentMode));
}

void AIAssistantPage::updateConnectionStatus() {
    if (!connectionStatus) return;

    if (isConnected) {
        lv_label_set_text(connectionStatus, LV_SYMBOL_WIFI " Online");
        lv_obj_set_style_text_color(connectionStatus, lv_color_hex(0x4CAF50), 0);
    } else {
        lv_label_set_text(connectionStatus, LV_SYMBOL_WIFI " Offline");
        lv_obj_set_style_text_color(connectionStatus, lv_color_hex(0xFF5722), 0);
    }
}

void AIAssistantPage::displayResponse(const String& text) {
    if (!responseText) return;

    lv_label_set_text(responseText, text.c_str());
}

void AIAssistantPage::animateAvatar() {
    // 基于状态的动画 (参考Echo-Mate的视觉反馈)
    static unsigned long lastAnimTime = 0;
    static bool animPhase = false;

    if (millis() - lastAnimTime > 1000) {
        lastAnimTime = millis();
        animPhase = !animPhase;

        if (currentState == AI_LISTENING && animPhase) {
            // 监听时的脉冲效果
            lv_obj_set_style_border_width(aiContainer, animPhase ? 5 : 3, 0);
        }
    }
}

lv_color_t AIAssistantPage::getStateColor(AIState state) {
    switch (state) {
        case AI_IDLE: return lv_color_hex(0x9C27B0);      // Purple
        case AI_LISTENING: return lv_color_hex(0x2196F3); // Blue
        case AI_PROCESSING: return lv_color_hex(0xFF9800); // Orange
        case AI_SPEAKING: return lv_color_hex(0x4CAF50);   // Green
        case AI_ERROR: return lv_color_hex(0xF44336);      // Red
        default: return lv_color_hex(0x9C27B0);
    }
}

const char* AIAssistantPage::getStateText(AIState state) {
    switch (state) {
        case AI_IDLE: return "Ready";
        case AI_LISTENING: return "Listening";
        case AI_PROCESSING: return "Processing";
        case AI_SPEAKING: return "Speaking";
        case AI_ERROR: return "Error";
        default: return "Unknown";
    }
}

const char* AIAssistantPage::getModeText(AIMode mode) {
    switch (mode) {
        case AI_VOICE_MODE: return "Voice Mode";
        case AI_TEXT_MODE: return "Text Mode";
        case AI_COMMAND_MODE: return "Command Mode";
        default: return "Unknown mode";
    }
}

const char* AIAssistantPage::getAvatarSymbol(AIState state) {
    switch (state) {
        case AI_IDLE: return LV_SYMBOL_SETTINGS;
        case AI_LISTENING: return LV_SYMBOL_AUDIO;
        case AI_PROCESSING: return LV_SYMBOL_REFRESH;
        case AI_SPEAKING: return LV_SYMBOL_VOLUME_MAX;
        case AI_ERROR: return LV_SYMBOL_WARNING;
        default: return LV_SYMBOL_SETTINGS;
    }
}

void AIAssistantPage::executeCommand(const String& command, const String& params) {
    Serial.printf("AI: Executing command: %s with params: %s\n", command.c_str(), params.c_str());

    if (command == "weather") {
        handleWeatherCommand();
    } else if (command == "time") {
        handleTimeCommand();
    } else if (command == "timer") {
        handleTimerCommand(params);
    } else if (command == "alarm") {
        handleAlarmCommand(params);
    } else if (command == "system") {
        handleSystemCommand(params);
    } else {
        setState(AI_SPEAKING);
        displayResponse("Unknown command: " + command);
        delay(2000);
        setState(AI_IDLE);
    }
}

void AIAssistantPage::handleWeatherCommand() {
    setState(AI_SPEAKING);
    displayResponse("Today: Sunny, 25°C. Perfect weather for outdoor activities!");

    delay(2000);
    setState(AI_IDLE);
}

void AIAssistantPage::handleTimeCommand() {
    // Get current time (simplified)
    unsigned long currentTime = millis();
    unsigned long seconds = currentTime / 1000;
    unsigned long minutes = seconds / 60;
    unsigned long hours = minutes / 60;

    String timeStr = String(hours % 24) + ":" +
                    String((minutes % 60) < 10 ? "0" : "") + String(minutes % 60);

    setState(AI_SPEAKING);
    displayResponse("Current time is " + timeStr);

    delay(2000);
    setState(AI_IDLE);
}

void AIAssistantPage::handleTimerCommand(const String& params) {
    setState(AI_SPEAKING);
    displayResponse("Timer command: " + params);
    delay(2000);
    setState(AI_IDLE);
}

void AIAssistantPage::handleAlarmCommand(const String& params) {
    setState(AI_SPEAKING);
    displayResponse("Alarm command: " + params);
    delay(2000);
    setState(AI_IDLE);
}

void AIAssistantPage::handleSystemCommand(const String& params) {
    setState(AI_SPEAKING);
    displayResponse("System command: " + params);
    delay(2000);
    setState(AI_IDLE);
}

void AIAssistantPage::processTextInput(const String& input) {
    Serial.printf("AI: Processing text input: %s\n", input.c_str());
    setState(AI_PROCESSING);
    displayResponse("Processing: " + input);

    delay(500);

    String response = processWithAI(input);
    AIResponse aiResponse;
    aiResponse.text = response;
    aiResponse.action = classifyIntent(input);
    aiResponse.isValid = true;
    aiResponse.timestamp = millis();

    handleAIResponse(aiResponse);
}

void AIAssistantPage::loadAIConfig() {
    aiServiceURL = "ws://localhost:8080";
    aiAPIKey = "demo_key";
    voiceEnabled = true;
    voiceVolume = 80;
    isConnected = false; // Simulate offline mode for now
}

void AIAssistantPage::saveAIConfig() {
    // Save configuration to EEPROM or file system
    Serial.println("AI: Configuration saved");
}

// 无参数版本的processVoiceInput (调用有参数版本)
void AIAssistantPage::processVoiceInput() {
    processVoiceInput(soundLevel);
}

// AI服务通信
void AIAssistantPage::sendToAI(const String& message) {
    Serial.printf("AI: Sending to AI service: %s\n", message.c_str());

    if (!isConnected) {
        Serial.println("AI: Not connected to AI service, using local processing");
        String response = processWithAI(message);
        AIResponse aiResponse;
        aiResponse.text = response;
        aiResponse.action = classifyIntent(message);
        aiResponse.isValid = true;
        aiResponse.timestamp = millis();
        handleAIResponse(aiResponse);
        return;
    }

    // TODO: Implement actual AI service communication
    String response = sendAIRequest(message);
    AIResponse aiResponse;
    aiResponse.text = response;
    aiResponse.action = classifyIntent(message);
    aiResponse.isValid = (response.length() > 0);
    aiResponse.timestamp = millis();
    handleAIResponse(aiResponse);
}

// 语音合成 (TTS)
void AIAssistantPage::speakResponse(const String& text) {
    Serial.printf("AI: Speaking: %s\n", text.c_str());

    if (!voiceEnabled) {
        Serial.println("AI: Voice output disabled");
        return;
    }

    // TODO: Implement actual TTS
    // For now, just display the text and simulate speaking time
    displayResponse(text);
    setState(AI_SPEAKING);

    // Simulate speaking duration based on text length
    unsigned long speakingTime = text.length() * 50; // 50ms per character
    speakingTime = constrain(speakingTime, 1000, 5000); // 1-5 seconds

    delay(speakingTime);
    setState(AI_IDLE);
}

// 音量显示
void AIAssistantPage::showVolumeLevel(int level) {
    if (!volumeIndicator) return;

    // Update volume indicator (if exists)
    String volumeText = "Vol: " + String(level) + "%";
    lv_label_set_text(volumeIndicator, volumeText.c_str());

    Serial.printf("AI: Volume level: %d%%\n", level);
}

// 清除回复
void AIAssistantPage::clearResponse() {
    if (responseText) {
        lv_label_set_text(responseText, "");
    }
    Serial.println("AI: Response cleared");
}

// 命令处理
String AIAssistantPage::processCommand(const String& command) {
    Serial.printf("AI: Processing command: %s\n", command.c_str());

    String lowerCommand = command;
    lowerCommand.toLowerCase();

    if (lowerCommand.indexOf("weather") >= 0) {
        return "Getting weather information...";
    } else if (lowerCommand.indexOf("time") >= 0) {
        return "Getting current time...";
    } else if (lowerCommand.indexOf("timer") >= 0) {
        return "Setting timer...";
    } else if (lowerCommand.indexOf("alarm") >= 0) {
        return "Setting alarm...";
    } else {
        return "Processing command: " + command;
    }
}

// 命令验证
bool AIAssistantPage::isValidCommand(const String& command) {
    if (command.length() == 0) return false;

    String lowerCommand = command;
    lowerCommand.toLowerCase();

    // Check for known commands
    return (lowerCommand.indexOf("weather") >= 0 ||
            lowerCommand.indexOf("time") >= 0 ||
            lowerCommand.indexOf("timer") >= 0 ||
            lowerCommand.indexOf("alarm") >= 0 ||
            lowerCommand.indexOf("hello") >= 0 ||
            lowerCommand.indexOf("help") >= 0);
}

// 语音录制开始 (简化版本，调用麦克风录制)
void AIAssistantPage::startVoiceRecording() {
    startMicrophoneRecording();
    Serial.println("AI: Voice recording started");
}

// 语音录制停止 (简化版本，调用麦克风停止)
void AIAssistantPage::stopVoiceRecording() {
    stopMicrophoneRecording();
    Serial.println("AI: Voice recording stopped");
}

// 获取录制文本 (模拟ASR结果)
String AIAssistantPage::getRecordedText() {
    if (!isRecording) {
        return "";
    }

    // 使用模拟ASR获取文本
    return simulateASR();
}

// 语音检测
bool AIAssistantPage::isVoiceDetected() {
    return soundLevel > micThreshold;
}

// 连接AI服务
bool AIAssistantPage::connectToAIService() {
    Serial.printf("AI: Connecting to AI service: %s\n", aiServiceURL.c_str());

    // TODO: Implement actual connection logic
    // For now, simulate connection
    isConnected = false; // Keep offline for demo

    if (isConnected) {
        Serial.println("AI: Connected to AI service");
        updateConnectionStatus();
        return true;
    } else {
        Serial.println("AI: Failed to connect to AI service (offline mode)");
        updateConnectionStatus();
        return false;
    }
}

// 断开AI服务
void AIAssistantPage::disconnectFromAIService() {
    Serial.println("AI: Disconnecting from AI service");
    isConnected = false;
    updateConnectionStatus();
}

// 发送AI请求
String AIAssistantPage::sendAIRequest(const String& message) {
    Serial.printf("AI: Sending request: %s\n", message.c_str());

    if (!isConnected) {
        Serial.println("AI: Not connected, using local processing");
        return processWithAI(message);
    }

    // TODO: Implement actual HTTP/WebSocket request
    // For now, use local processing
    return processWithAI(message);
}
