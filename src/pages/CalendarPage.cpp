#include "CalendarPage.h"
#include <Arduino.h>
#include <ctime>
#include "RTC_SAMD51.h"
#include "DateTime.h"

// Global RTC instance
RTC_SAMD51 rtc;

CalendarPage::CalendarPage() {
    titleLabel = nullptr;
    timeLabel = nullptr;
    monthLabel = nullptr;
    calendarGrid = nullptr;

    for (int i = 0; i < 7; i++) {
        dayLabels[i] = nullptr;
    }

    for (int i = 0; i < 42; i++) {
        dateLabels[i] = nullptr;
    }

    // Initialize RTC and get current time
    initializeRTC();

    // ???????????????
    lastTimeUpdate = 0;
}

CalendarPage::~CalendarPage() {
    // Destructor - cleanup handled by PageManager
}

void CalendarPage::initializeRTC() {
    Serial.println("Initializing RTC...");

    if (!rtc.begin()) {
        Serial.println("Couldn't find RTC");
        // Use default date if RTC fails
        currentYear = 2024;
        currentMonth = 12;
        currentDay = 21;
        currentHour = 12;
        currentMinute = 0;
        return;
    }

    // Check if RTC has valid time
    DateTime now = rtc.now();
    if (now.year() < 2020) {
        // RTC not set, set to compile time
        Serial.println("RTC not set, setting to compile time");
        DateTime compileTime = DateTime(F(__DATE__), F(__TIME__));
        rtc.adjust(compileTime);
        now = rtc.now();
    }

    // Get current time from RTC
    currentYear = now.year();
    currentMonth = now.month();
    currentDay = now.day();
    currentHour = now.hour();
    currentMinute = now.minute();

    Serial.printf("RTC initialized: %04d-%02d-%02d %02d:%02d\n",
                  currentYear, currentMonth, currentDay, currentHour, currentMinute);
}

void CalendarPage::updateCurrentTime() {
    DateTime now = rtc.now();
    currentYear = now.year();
    currentMonth = now.month();
    currentDay = now.day();
    currentHour = now.hour();
    currentMinute = now.minute();

    // Update time display
    if (timeLabel) {
        lv_label_set_text_fmt(timeLabel, "%02d:%02d", currentHour, currentMinute);
    }

    Serial.printf("Time updated: %04d-%02d-%02d %02d:%02d\n",
                  currentYear, currentMonth, currentDay, currentHour, currentMinute);
}

void CalendarPage::onViewLoad() {
    Serial.println("CalendarPage: onViewLoad");
    
    // Set background color
    lv_obj_set_style_bg_color(_root, lv_color_hex(0xFFF8F0), 0); // Light orange
    lv_obj_set_style_bg_opa(_root, LV_OPA_COVER, 0);
}

void CalendarPage::onViewDidLoad() {
    Serial.println("CalendarPage: onViewDidLoad");
    createCalendarUI();
}

void CalendarPage::onViewWillAppear() {
    Serial.println("CalendarPage: onViewWillAppear");
    updateCurrentTime();
    updateCalendar();
}

void CalendarPage::onViewDidAppear() {
    Serial.println("CalendarPage: onViewDidAppear");
}

void CalendarPage::onViewWillDisappear() {
    Serial.println("CalendarPage: onViewWillDisappear");
}

void CalendarPage::onViewDidDisappear() {
    Serial.println("CalendarPage: onViewDidDisappear");
}

void CalendarPage::onViewUnload() {
    Serial.println("CalendarPage: onViewUnload");
    // Objects will be automatically deleted with _root
}

void CalendarPage::onKey(lv_dir_t direction) {
    Serial.printf("CalendarPage: Key pressed - direction: %d\n", direction);

    switch (direction) {
        case LV_DIR_LEFT:
            // Previous month
            currentMonth--;
            if (currentMonth < 1) {
                currentMonth = 12;
                currentYear--;
            }
            updateCalendar();
            break;

        case LV_DIR_RIGHT:
            // Next month
            currentMonth++;
            if (currentMonth > 12) {
                currentMonth = 1;
                currentYear++;
            }
            updateCalendar();
            break;

        case LV_DIR_TOP:
            // Previous year
            currentYear--;
            updateCalendar();
            break;

        case LV_DIR_BOTTOM:
            // Next year
            currentYear++;
            updateCalendar();
            break;

        case LV_DIR_NONE:
            // Button press - could be used for selection
            Serial.println("Calendar: Button pressed");
            break;
    }
}

void CalendarPage::onButton(bool pressed) {
    if (pressed) {
        Serial.println("CalendarPage: Button pressed - updating time");
        // Update to current time from RTC
        updateCurrentTime();
        updateCalendar();
    }
}

void CalendarPage::update() {
    // ???????????????????
    unsigned long currentMillis = millis();
    if (currentMillis - lastTimeUpdate >= TIME_UPDATE_INTERVAL) {
        lastTimeUpdate = currentMillis;

        // ??????????
        DateTime now = rtc.now();
        int newHour = now.hour();
        int newMinute = now.minute();

        // ??ù?????????????????
        if (newHour != currentHour || newMinute != currentMinute) {
            currentHour = newHour;
            currentMinute = newMinute;

            // ?????????
            if (timeLabel) {
                lv_label_set_text_fmt(timeLabel, "%02d:%02d", currentHour, currentMinute);
            }

            // ?????????????????????????
            int newYear = now.year();
            int newMonth = now.month();
            int newDay = now.day();

            if (newYear != currentYear || newMonth != currentMonth || newDay != currentDay) {
                currentYear = newYear;
                currentMonth = newMonth;
                currentDay = newDay;
                updateCalendar(); // ???ù???????
            }
        }
    }
}

void CalendarPage::createCalendarUI() {
    // Page title
    titleLabel = lv_label_create(_root);
    lv_label_set_text_fmt(titleLabel, "%s CALENDAR", LV_SYMBOL_CALL);
    lv_obj_align(titleLabel, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_set_style_text_font(titleLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(titleLabel, lv_color_hex(0xFF6600), 0);

    // Time display (HH:MM) in top right
    timeLabel = lv_label_create(_root);
    lv_label_set_text(timeLabel, "00:00");
    lv_obj_align(timeLabel, LV_ALIGN_TOP_RIGHT, -10, 5);
    lv_obj_set_style_text_font(timeLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(timeLabel, lv_color_hex(0x007AFF), 0);

    // Month/Year display
    monthLabel = lv_label_create(_root);
    lv_obj_align(monthLabel, LV_ALIGN_TOP_MID, 0, 25);
    lv_obj_set_style_text_font(monthLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(monthLabel, lv_color_hex(0x333333), 0);

    // Calendar grid container
    calendarGrid = lv_obj_create(_root);
    lv_obj_set_size(calendarGrid, 280, 160);
    lv_obj_align(calendarGrid, LV_ALIGN_TOP_MID, 0, 60);
    lv_obj_set_style_bg_color(calendarGrid, lv_color_white(), 0);
    lv_obj_set_style_border_width(calendarGrid, 1, 0);
    lv_obj_set_style_border_color(calendarGrid, lv_color_hex(0xDDDDDD), 0);
    lv_obj_set_style_radius(calendarGrid, 5, 0);
    lv_obj_set_style_pad_all(calendarGrid, 8, 0);
    lv_obj_clear_flag(calendarGrid, LV_OBJ_FLAG_SCROLLABLE);

    // Day headers
    const char* dayNames[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    for (int i = 0; i < 7; i++) {
        dayLabels[i] = lv_label_create(calendarGrid);
        lv_label_set_text(dayLabels[i], dayNames[i]);
        lv_obj_set_pos(dayLabels[i], i * 35 + 8, 5);
        lv_obj_set_style_text_font(dayLabels[i], &lv_font_montserrat_14, 0);
        lv_obj_set_style_text_color(dayLabels[i], lv_color_hex(0x666666), 0);
    }

    // Date labels (6 rows * 7 columns)
    for (int i = 0; i < 42; i++) {
        dateLabels[i] = lv_label_create(calendarGrid);
        lv_label_set_text(dateLabels[i], "");
        
        int row = i / 7;
        int col = i % 7;
        lv_obj_set_pos(dateLabels[i], col * 35 + 12, 25 + row * 20);
        lv_obj_set_style_text_font(dateLabels[i], &lv_font_montserrat_14, 0);
        lv_obj_set_style_text_color(dateLabels[i], lv_color_hex(0x333333), 0);
    }

    // No instructions needed
}

void CalendarPage::updateCalendar() {
    if (!monthLabel) return;

    // Update month/year display
    const char* monthNames[] = {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };
    
    lv_label_set_text_fmt(monthLabel, "%s %d", monthNames[currentMonth - 1], currentYear);

    // Calculate first day of month and number of days
    struct tm timeinfo = {0};
    timeinfo.tm_year = currentYear - 1900;
    timeinfo.tm_mon = currentMonth - 1;
    timeinfo.tm_mday = 1;
    
    time_t firstDay = mktime(&timeinfo);
    struct tm* firstDayInfo = localtime(&firstDay);
    int startDayOfWeek = firstDayInfo->tm_wday; // 0 = Sunday
    
    // Days in month
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    // Check for leap year
    bool isLeapYear = (currentYear % 4 == 0 && currentYear % 100 != 0) || (currentYear % 400 == 0);
    if (isLeapYear) {
        daysInMonth[1] = 29;
    }
    
    int daysThisMonth = daysInMonth[currentMonth - 1];

    // Clear all date labels
    for (int i = 0; i < 42; i++) {
        lv_label_set_text(dateLabels[i], "");
        lv_obj_set_style_text_color(dateLabels[i], lv_color_hex(0x333333), 0);
    }

    // Fill in the dates
    for (int day = 1; day <= daysThisMonth; day++) {
        int position = startDayOfWeek + day - 1;
        if (position < 42) {
            lv_label_set_text_fmt(dateLabels[position], "%d", day);
            
            // Highlight today (using RTC current date)
            DateTime now = rtc.now();
            if (currentYear == now.year() && currentMonth == now.month() && day == now.day()) {
                lv_obj_set_style_text_color(dateLabels[position], lv_color_hex(0x007AFF), 0);
                lv_obj_set_style_bg_color(dateLabels[position], lv_color_hex(0xE6F3FF), 0);
                lv_obj_set_style_bg_opa(dateLabels[position], LV_OPA_COVER, 0);
                lv_obj_set_style_radius(dateLabels[position], 3, 0);
            }
        }
    }

    Serial.printf("Calendar updated: %s %d\n", monthNames[currentMonth - 1], currentYear);
}
