#include <Arduino.h>
#include <TFT_eSPI.h>
#include "lvgl.h"
#include "theme/style_manager.h"
#include "core/AppManager.h"

// Global app manager
AppManager* appManager = nullptr;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf1[320 * 10];
static lv_disp_drv_t disp_drv;
static TFT_eSPI tft = TFT_eSPI();

// Joystick pins (Wio Terminal)
#define JOYSTICK_UP    WIO_5S_UP
#define JOYSTICK_DOWN  WIO_5S_DOWN
#define JOYSTICK_LEFT  WIO_5S_LEFT
#define JOYSTICK_RIGHT WIO_5S_RIGHT
#define JOYSTICK_PRESS WIO_5S_PRESS

// Physical button pins
#define KEY_A_PIN      WIO_KEY_A
#define KEY_B_PIN      WIO_KEY_B
#define KEY_C_PIN      WIO_KEY_C

// Input state tracking
static bool lastJoystickState[5] = {false, false, false, false, false};
static bool lastKeyState[3] = {false, false, false};
static unsigned long lastJoystickTime = 0;
static unsigned long lastKeyTime = 0;
static const unsigned long INPUT_DEBOUNCE = 200; // ms

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t*)&color_p->full, w * h, true);
    tft.endWrite();

    lv_disp_flush_ready(disp);
}

void handleJoystickInput() {
    unsigned long currentTime = millis();
    if (currentTime - lastJoystickTime < INPUT_DEBOUNCE) {
        return; // Debounce protection
    }

    // Wio Terminal joystick is active LOW (LOW when pressed)
    bool currentState[5] = {
        !digitalRead(JOYSTICK_UP),    // Invert because it's active LOW
        !digitalRead(JOYSTICK_DOWN),
        !digitalRead(JOYSTICK_LEFT),
        !digitalRead(JOYSTICK_RIGHT),
        !digitalRead(JOYSTICK_PRESS)
    };

    // Detect button press events (from false to true)
    if (!lastJoystickState[0] && currentState[0]) { // UP
        Serial.println("Joystick UP pressed");
        // Up/Down directions can be used for in-page navigation, not handled for now
        lastJoystickTime = currentTime;
    }
    else if (!lastJoystickState[1] && currentState[1]) { // DOWN
        Serial.println("Joystick DOWN pressed");
        // Up/Down directions can be used for in-page navigation, not handled for now
        lastJoystickTime = currentTime;
    }
    else if (!lastJoystickState[2] && currentState[2]) { // LEFT
        Serial.println("Joystick LEFT pressed");
        if (appManager) appManager->handleInput(LV_DIR_LEFT);
        lastJoystickTime = currentTime;
    }
    else if (!lastJoystickState[3] && currentState[3]) { // RIGHT
        Serial.println("Joystick RIGHT pressed");
        if (appManager) appManager->handleInput(LV_DIR_RIGHT);
        lastJoystickTime = currentTime;
    }
    else if (!lastJoystickState[4] && currentState[4]) { // PRESS
        Serial.println("Joystick PRESS pressed");
        if (appManager) appManager->handleInput(LV_DIR_NONE); // Press means selection
        lastJoystickTime = currentTime;
    }

    // Update state
    for (int i = 0; i < 5; i++) {
        lastJoystickState[i] = currentState[i];
    }
}

void handleKeyInput() {
    unsigned long currentTime = millis();
    if (currentTime - lastKeyTime < INPUT_DEBOUNCE) {
        return; // Debounce protection
    }

    // Wio Terminal keys are active LOW (LOW when pressed)
    bool currentKeyState[3] = {
        !digitalRead(KEY_A_PIN),
        !digitalRead(KEY_B_PIN),
        !digitalRead(KEY_C_PIN)
    };

    // Detect key press events (from false to true)
    if (!lastKeyState[0] && currentKeyState[0]) { // Key A
        Serial.println("Key A pressed");
        if (appManager) appManager->handleKeyA(true);
        lastKeyTime = currentTime;
    }
    else if (!lastKeyState[1] && currentKeyState[1]) { // Key B
        Serial.println("Key B pressed");
        if (appManager) appManager->handleKeyB(true);
        lastKeyTime = currentTime;
    }
    else if (!lastKeyState[2] && currentKeyState[2]) { // Key C
        Serial.println("Key C pressed");
        if (appManager) appManager->handleKeyC(true);
        lastKeyTime = currentTime;
    }

    // Update state
    for (int i = 0; i < 3; i++) {
        lastKeyState[i] = currentKeyState[i];
    }
}

void loop() {
    lv_timer_handler();
    handleJoystickInput();
    handleKeyInput();

    // Update timers and alarms periodically
    static unsigned long lastUpdate = 0;
    unsigned long currentTime = millis();
    if (currentTime - lastUpdate > 1000) { // Update every 1 second
        if (appManager) appManager->update();
        lastUpdate = currentTime;
    }

    // Force screen refresh periodically
    static unsigned long lastRefresh = 0;
    if (currentTime - lastRefresh > 100) { // Refresh every 100ms
        lv_refr_now(NULL);
        lastRefresh = currentTime;
    }

    delay(5);
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("Starting Wio Terminal LVGL App...");

    // Initialize joystick pins
    Serial.println("Initializing joystick...");
    pinMode(JOYSTICK_UP, INPUT_PULLUP);
    pinMode(JOYSTICK_DOWN, INPUT_PULLUP);
    pinMode(JOYSTICK_LEFT, INPUT_PULLUP);
    pinMode(JOYSTICK_RIGHT, INPUT_PULLUP);
    pinMode(JOYSTICK_PRESS, INPUT_PULLUP);
    Serial.println("Joystick initialized");

    // Initialize physical button pins
    Serial.println("Initializing physical buttons...");
    pinMode(KEY_A_PIN, INPUT_PULLUP);
    pinMode(KEY_B_PIN, INPUT_PULLUP);
    pinMode(KEY_C_PIN, INPUT_PULLUP);
    Serial.println("Physical buttons initialized");

    // Initialize TFT
    Serial.println("Initializing TFT...");
    tft.begin();
    tft.setRotation(3);
    tft.fillScreen(TFT_BLACK);
    Serial.println("TFT initialized");

    // Initialize LVGL
    Serial.println("Initializing LVGL...");
    lv_init();
    lv_disp_draw_buf_init(&draw_buf, buf1, NULL, 320 * 10);

    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = 320;
    disp_drv.ver_res = 240;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);
    Serial.println("LVGL display driver registered");

    // Initialize style manager
    Serial.println("Initializing styles...");
    StyleManager::init();
    Serial.println("Styles initialized");

    // Create main container
    lv_obj_t* main_container = lv_obj_create(lv_scr_act());
    lv_obj_set_size(main_container, LV_HOR_RES, LV_VER_RES);
    lv_obj_align(main_container, LV_ALIGN_CENTER, 0, 0);
    lv_obj_clear_flag(main_container, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(main_container, COLOR_BACKGROUND, 0); // Use style manager background color

    // Initialize App Manager (X-TRACK style)
    Serial.println("Initializing App Manager...");
    appManager = new AppManager();
    if (!appManager->init()) {
        Serial.println("Failed to initialize App Manager!");
        return;
    }
    Serial.println("App Manager initialized successfully!");

    Serial.println("Setup completed!");
}
