#include "AppManager.h"
#include <Arduino.h>
#include "../utils/WiFiManager.h"
#include "../pages/WeatherPage.h"

// Static data
const char* AppManager::pageNames[] = {"Calendar", "Music", "Alarm", "Memo", "Timer", "Weather", "AI"};
const char* AppManager::pageIcons[] = {LV_SYMBOL_CALL, LV_SYMBOL_AUDIO, LV_SYMBOL_BELL, LV_SYMBOL_EDIT, LV_SYMBOL_REFRESH, LV_SYMBOL_WIFI, LV_SYMBOL_SETTINGS};
const int AppManager::pageCount = 7;
AppManager* AppManager::instance = nullptr;

AppManager::AppManager() {
    instance = this;

    // UI Elements
    statusBar = nullptr;
    timeLabel = nullptr;
    batteryLabel = nullptr;
    navBar = nullptr;
    navIndicator = nullptr;

    for (int i = 0; i < 7; i++) {
        navButtons[i] = nullptr;
        navLabels[i] = nullptr;
    }
    currentNavIndex = 0;

    // Pages
    calendarPage = nullptr;
    musicPage = nullptr;
    alarmPage = nullptr;
    memoPage = nullptr;
    timerPage = nullptr;
    weatherPage = nullptr;
    aiAssistantPage = nullptr;
}

AppManager::~AppManager() {
    if (calendarPage) delete calendarPage;
    if (musicPage) delete musicPage;
    if (alarmPage) delete alarmPage;
    if (memoPage) delete memoPage;
    if (timerPage) delete timerPage;
    if (weatherPage) delete weatherPage;
    if (aiAssistantPage) delete aiAssistantPage;
}

bool AppManager::init() {
    Serial.println("AppManager: Initializing X-TRACK style UI...");

    // Initialize WiFi Manager
    Serial.println("AppManager: Initializing WiFi...");
    WiFiManager& wifiMgr = WiFiManager::getInstance();
    if (wifiMgr.begin()) {
        Serial.println("AppManager: WiFi initialized successfully");
    } else {
        Serial.println("AppManager: WiFi initialization failed");
    }

    // Create UI elements
    createUI();

    // Register pages
    registerPages();

    // Start with calendar page
    pageManager.Push("Calendar");

    Serial.println("AppManager: Initialized successfully");
    return true;
}

void AppManager::registerPages() {
    Serial.println("AppManager: Registering pages...");

    // Create and register Calendar page
    calendarPage = new CalendarPage();
    if (!pageManager.Register(calendarPage, "Calendar")) {
        Serial.println("Error: Failed to register Calendar page");
        return;
    }

    // Create and register Music page
    musicPage = new MusicPage();
    if (!pageManager.Register(musicPage, "Music")) {
        Serial.println("Error: Failed to register Music page");
        return;
    }

    // Create and register Alarm page
    alarmPage = new AlarmPage();
    if (!pageManager.Register(alarmPage, "Alarm")) {
        Serial.println("Error: Failed to register Alarm page");
        return;
    }

    // Create and register Memo page
    memoPage = new MemoPage();
    if (!pageManager.Register(memoPage, "Memo")) {
        Serial.println("Error: Failed to register Memo page");
        return;
    }

    // Create and register Timer page
    timerPage = new TimerPage();
    if (!pageManager.Register(timerPage, "Timer")) {
        Serial.println("Error: Failed to register Timer page");
        return;
    }

    // Create and register Weather page
    Serial.println("Creating WeatherPage...");
    weatherPage = new WeatherPage();
    Serial.println("WeatherPage created, registering...");
    if (!pageManager.Register(weatherPage, "Weather")) {
        Serial.println("Error: Failed to register Weather page");
        return;
    }
    Serial.println("WeatherPage registered successfully!");

    // Create and register AI Assistant page
    aiAssistantPage = new AIAssistantPage();
    if (!pageManager.Register(aiAssistantPage, "AI")) {
        Serial.println("Error: Failed to register AI Assistant page");
        return;
    }

    Serial.println("AppManager: All pages registered successfully");
}

void AppManager::createUI() {
    Serial.println("AppManager: Creating X-TRACK style UI...");

    createStatusBar();
    createNavigationBar();

    Serial.println("AppManager: UI created successfully");
}

void AppManager::createStatusBar() {
    // Create status bar (X-TRACK style)
    statusBar = lv_obj_create(lv_scr_act());
    lv_obj_set_size(statusBar, LV_HOR_RES, 20);
    lv_obj_align(statusBar, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(statusBar, lv_color_hex(0x1E1E1E), 0); // Dark background
    lv_obj_set_style_border_width(statusBar, 0, 0);
    lv_obj_set_style_pad_all(statusBar, 2, 0);
    lv_obj_clear_flag(statusBar, LV_OBJ_FLAG_SCROLLABLE);

    // Time label
    timeLabel = lv_label_create(statusBar);
    lv_label_set_text(timeLabel, "14:30");
    lv_obj_align(timeLabel, LV_ALIGN_LEFT_MID, 5, 0);
    lv_obj_set_style_text_font(timeLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(timeLabel, lv_color_white(), 0);

    // Battery label
    batteryLabel = lv_label_create(statusBar);
    lv_label_set_text_fmt(batteryLabel, "%s 85%%", LV_SYMBOL_BATTERY_3);
    lv_obj_align(batteryLabel, LV_ALIGN_RIGHT_MID, -5, 0);
    lv_obj_set_style_text_font(batteryLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(batteryLabel, lv_color_white(), 0);
}

void AppManager::createNavigationBar() {
    // Create navigation bar (X-TRACK style - more modern)
    navBar = lv_obj_create(lv_scr_act());
    lv_obj_set_size(navBar, LV_HOR_RES, 50);
    lv_obj_align(navBar, LV_ALIGN_TOP_MID, 0, 20); // Below status bar
    lv_obj_set_style_bg_color(navBar, lv_color_hex(0x2A2A2A), 0); // Dark theme
    lv_obj_set_style_border_width(navBar, 0, 0);
    lv_obj_set_style_pad_all(navBar, 5, 0);
    lv_obj_clear_flag(navBar, LV_OBJ_FLAG_SCROLLABLE);

    // Create navigation indicator (sliding indicator) - adjusted for 7 pages
    navIndicator = lv_obj_create(navBar);
    lv_obj_set_size(navIndicator, 45, 4);
    lv_obj_set_pos(navIndicator, 2, 42);
    lv_obj_set_style_bg_color(navIndicator, lv_color_hex(0x00D4FF), 0); // Cyan accent
    lv_obj_set_style_border_width(navIndicator, 0, 0);
    lv_obj_set_style_radius(navIndicator, 2, 0);

    // Create navigation buttons (X-TRACK style) - adjusted for 7 pages
    for (int i = 0; i < pageCount; i++) {
        navButtons[i] = lv_obj_create(navBar);
        lv_obj_set_size(navButtons[i], 45, 35);
        lv_obj_set_pos(navButtons[i], i * 46 + 2, 5);
        lv_obj_set_style_bg_opa(navButtons[i], LV_OPA_TRANSP, 0); // Transparent background
        lv_obj_set_style_border_width(navButtons[i], 0, 0);
        lv_obj_add_event_cb(navButtons[i], onNavButtonClick, LV_EVENT_CLICKED, (void*)(intptr_t)i);
        lv_obj_clear_flag(navButtons[i], LV_OBJ_FLAG_SCROLLABLE);

        // Icon
        lv_obj_t* icon = lv_label_create(navButtons[i]);
        lv_label_set_text(icon, pageIcons[i]);
        lv_obj_align(icon, LV_ALIGN_TOP_MID, 0, 2);
        lv_obj_set_style_text_font(icon, &lv_font_montserrat_14, 0);
        lv_obj_set_style_text_color(icon, lv_color_hex(0x888888), 0);

        // Label
        navLabels[i] = lv_label_create(navButtons[i]);
        lv_label_set_text(navLabels[i], pageNames[i]);
        lv_obj_align(navLabels[i], LV_ALIGN_BOTTOM_MID, 0, -2);
        lv_obj_set_style_text_font(navLabels[i], &lv_font_montserrat_14, 0);
        lv_obj_set_style_text_color(navLabels[i], lv_color_hex(0x888888), 0);
    }

    updateNavigationBar();
}

void AppManager::updateNavigationBar() {
    // Update indicator position (X-TRACK style sliding animation)
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, navIndicator);
    lv_anim_set_values(&a, lv_obj_get_x(navIndicator), currentNavIndex * 46 + 2);
    lv_anim_set_time(&a, 200);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_start(&a);

    // Update button states
    for (int i = 0; i < pageCount; i++) {
        lv_obj_t* icon = lv_obj_get_child(navButtons[i], 0);
        lv_obj_t* label = navLabels[i];

        if (i == currentNavIndex) {
            // Active state - bright colors
            if (icon) {
                lv_obj_set_style_text_color(icon, lv_color_hex(0x00D4FF), 0); // Cyan
            }
            if (label) {
                lv_obj_set_style_text_color(label, lv_color_white(), 0);
            }
        } else {
            // Inactive state - dim colors
            if (icon) {
                lv_obj_set_style_text_color(icon, lv_color_hex(0x666666), 0);
            }
            if (label) {
                lv_obj_set_style_text_color(label, lv_color_hex(0x888888), 0);
            }
        }
    }

    Serial.printf("Navigation updated to index: %d\n", currentNavIndex);
}

void AppManager::switchToPage(int index) {
    if (index < 0 || index >= pageCount) return;
    if (index == currentNavIndex) return; // Already on this page

    currentNavIndex = index;
    updateNavigationBar();

    // Switch to corresponding page (all pages are now implemented)
    pageManager.Replace(pageNames[index]);

    Serial.printf("Switched to page: %s\n", pageNames[index]);
}

void AppManager::onNavButtonClick(lv_event_t* e) {
    int buttonIndex = (int)(intptr_t)lv_event_get_user_data(e);
    Serial.printf("AppManager: Nav button %d clicked\n", buttonIndex);

    if (instance) {
        instance->switchToPage(buttonIndex);
    }
}

void AppManager::handleInput(lv_dir_t direction) {
    switch (direction) {
        case LV_DIR_LEFT:
            if (currentNavIndex > 0) {
                switchToPage(currentNavIndex - 1);
            }
            break;

        case LV_DIR_RIGHT:
            if (currentNavIndex < pageCount - 1) {
                switchToPage(currentNavIndex + 1);
            }
            break;

        default:
            // Pass other inputs to current page
            pageManager.HandleInput(direction);
            break;
    }
}

void AppManager::handleEncoder(int32_t diff) {
    pageManager.HandleEncoder(diff);
}

void AppManager::handleButton(bool pressed) {
    pageManager.HandleButton(pressed);
}

void AppManager::handleKeyA(bool pressed) {
    Serial.printf("AppManager: Key A %s\n", pressed ? "pressed" : "released");

    if (!pressed) return; // Only handle press, not release

    // A button: ????????/??????
    PageBase* currentPage = pageManager.GetCurrentPage();
    if (currentPage) {
        const char* currentPageName = pageManager.GetCurrentPageName();
        if (currentPageName) {
            if (strcmp(currentPageName, "Alarm") == 0) {
                // ????????????????????
                currentPage->onKey(LV_DIR_BOTTOM);
            } else if (strcmp(currentPageName, "Memo") == 0) {
                // ??????????????????????
                currentPage->onKey(LV_DIR_BOTTOM);
            } else if (strcmp(currentPageName, "Music") == 0) {
                // ????????????/???
                static_cast<MusicPage*>(currentPage)->onButton(pressed);
            } else if (strcmp(currentPageName, "Timer") == 0) {
                // A???????/???
                static_cast<TimerPage*>(currentPage)->onButtonA(pressed);
            } else if (strcmp(currentPageName, "Weather") == 0) {
                // A???????????
                currentPage->onButton(pressed);
            } else if (strcmp(currentPageName, "AI") == 0) {
                // A?: AI??????/??
                static_cast<AIAssistantPage*>(currentPage)->onButton(pressed);
            }
        }
    }
}

void AppManager::handleKeyB(bool pressed) {
    Serial.printf("AppManager: Key B %s\n", pressed ? "pressed" : "released");

    if (!pressed) return; // Only handle press, not release

    // B button: ??????/??????
    PageBase* currentPage = pageManager.GetCurrentPage();
    if (currentPage) {
        const char* currentPageName = pageManager.GetCurrentPageName();
        if (currentPageName) {
            if (strcmp(currentPageName, "Alarm") == 0) {
                // ????????????????
                AlarmPage* alarmPagePtr = static_cast<AlarmPage*>(currentPage);
                alarmPagePtr->editMode = !alarmPagePtr->editMode;
                alarmPagePtr->updateAlarmList();
                Serial.printf("Alarm edit mode: %s\n", alarmPagePtr->editMode ? "ON" : "OFF");
            } else if (strcmp(currentPageName, "Timer") == 0) {
                // ???????????????
                static_cast<TimerPage*>(currentPage)->onButtonB(true);
            } else if (strcmp(currentPageName, "Memo") == 0) {
                // ???????????????/????
                static_cast<MemoPage*>(currentPage)->onButton(pressed);
            } else if (strcmp(currentPageName, "Music") == 0) {
                // ??????????????
                currentPage->onKey(LV_DIR_RIGHT);
            } else if (strcmp(currentPageName, "Weather") == 0) {
                // B???????????????????????
                Serial.println("Weather: Display mode switch not implemented yet");
            } else if (strcmp(currentPageName, "AI") == 0) {
                // B?: AI??????
                static_cast<AIAssistantPage*>(currentPage)->onKey(LV_DIR_LEFT);
            }
        }
    }
}

void AppManager::handleKeyC(bool pressed) {
    Serial.printf("AppManager: Key C %s\n", pressed ? "pressed" : "released");

    if (!pressed) return; // Only handle press, not release

    // C button: ????/??????
    PageBase* currentPage = pageManager.GetCurrentPage();
    if (currentPage) {
        const char* currentPageName = pageManager.GetCurrentPageName();
        if (currentPageName) {
            if (strcmp(currentPageName, "Timer") == 0) {
                // C???????????
                static_cast<TimerPage*>(currentPage)->onButtonC(pressed);
            } else if (strcmp(currentPageName, "Alarm") == 0) {
                // ????????????????
                static_cast<AlarmPage*>(currentPage)->onButton(pressed);
            } else if (strcmp(currentPageName, "Music") == 0) {
                // ??????????????
                currentPage->onKey(LV_DIR_LEFT);
            } else if (strcmp(currentPageName, "Memo") == 0) {
                // ???????????????????
                currentPage->onKey(LV_DIR_TOP);
            } else if (strcmp(currentPageName, "Weather") == 0) {
                // C?????????????????
                Serial.println("Weather: Settings not implemented yet");
            } else if (strcmp(currentPageName, "AI") == 0) {
                // C?: AI??????
                static_cast<AIAssistantPage*>(currentPage)->onKey(LV_DIR_RIGHT);
            }
        }
    }
}

void AppManager::update() {
    // ??????Calendar??????????????????
    if (calendarPage) {
        calendarPage->update();
    }

    // Periodic update for current page
    PageBase* currentPage = pageManager.GetCurrentPage();
    if (currentPage) {
        const char* currentPageName = pageManager.GetCurrentPageName();
        if (currentPageName) {
            if (strcmp(currentPageName, "Timer") == 0) {
                // Update timer display
                static_cast<TimerPage*>(currentPage)->updateTimer();
            } else if (strcmp(currentPageName, "Alarm") == 0) {
                // Check alarms
                static_cast<AlarmPage*>(currentPage)->checkAlarms();
            } else if (strcmp(currentPageName, "Music") == 0) {
                // Update music playback progress
                static_cast<MusicPage*>(currentPage)->updatePlaybackProgress();
            } else if (strcmp(currentPageName, "Weather") == 0) {
                // Update weather data
                static_cast<WeatherPage*>(currentPage)->update();
            } else if (strcmp(currentPageName, "AI") == 0) {
                // Update AI assistant
                static_cast<AIAssistantPage*>(currentPage)->update();
            }
            // Calendar????update?????????????????????????
        }
    }
}
