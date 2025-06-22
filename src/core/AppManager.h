#pragma once

#include "PageManager.h"
#include "../pages/CalendarPage.h"
#include "../pages/MusicPage.h"
#include "../pages/AlarmPage.h"
#include "../pages/MemoPage.h"
#include "../pages/TimerPage.h"
#include "../pages/WeatherPage.h"
#include "../pages/AIAssistantPage.h"

class AppManager {
public:
    AppManager();
    ~AppManager();

    bool init();
    void handleInput(lv_dir_t direction);
    void handleEncoder(int32_t diff);
    void handleButton(bool pressed);
    void handleKeyA(bool pressed);
    void handleKeyB(bool pressed);
    void handleKeyC(bool pressed);
    void update(); // Periodic update for timers and alarms

    PageManager* getPageManager() { return &pageManager; }

private:
    void registerPages();
    void createUI();
    void createStatusBar();
    void createNavigationBar();
    void updateNavigationBar();
    void switchToPage(int index);

    static void onNavButtonClick(lv_event_t* e);
    static AppManager* instance; // For static callback

private:
    PageManager pageManager;

    // UI Elements
    lv_obj_t* statusBar;
    lv_obj_t* timeLabel;
    lv_obj_t* batteryLabel;
    lv_obj_t* navBar;
    lv_obj_t* navButtons[7];
    lv_obj_t* navLabels[7];
    lv_obj_t* navIndicator;

    int currentNavIndex;

    // Pages
    CalendarPage* calendarPage;
    MusicPage* musicPage;
    AlarmPage* alarmPage;
    MemoPage* memoPage;
    TimerPage* timerPage;
    WeatherPage* weatherPage;
    AIAssistantPage* aiAssistantPage;

    // Page configuration
    static const char* pageNames[];
    static const char* pageIcons[];
    static const int pageCount;
};
