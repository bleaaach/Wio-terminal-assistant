#include "AlarmPage.h"
#include <Arduino.h>
#include <time.h>

// Static alarm data
AlarmPage::Alarm AlarmPage::alarms[5] = {
    {7, 30, "Wake Up", true, {false, true, true, true, true, true, false}},
    {12, 0, "Lunch Break", false, {false, true, true, true, true, true, false}},
    {18, 30, "Dinner Time", true, {true, true, true, true, true, true, true}},
    {22, 0, "Sleep Reminder", false, {false, true, true, true, true, true, false}},
    {6, 0, "Morning Jog", false, {false, false, false, false, false, false, true}}
};

const int AlarmPage::alarmCount = 5;

AlarmPage::AlarmPage() {
    titleLabel = nullptr;
    alarmList = nullptr;
    addButton = nullptr;
    
    for (int i = 0; i < 5; i++) {
        alarmItems[i] = nullptr;
        timeLabels[i] = nullptr;
        nameLabels[i] = nullptr;
        toggleSwitches[i] = nullptr;
    }
    
    selectedAlarm = 0;
    editMode = false;
}

AlarmPage::~AlarmPage() {
    // Destructor - cleanup handled by PageManager
}

void AlarmPage::onViewLoad() {
    Serial.println("AlarmPage: onViewLoad");
    
    // Set background color
    lv_obj_set_style_bg_color(_root, lv_color_hex(0xF0FFF0), 0); // Light green
    lv_obj_set_style_bg_opa(_root, LV_OPA_COVER, 0);
}

void AlarmPage::onViewDidLoad() {
    Serial.println("AlarmPage: onViewDidLoad");
    createAlarmUI();
}

void AlarmPage::onViewWillAppear() {
    Serial.println("AlarmPage: onViewWillAppear");
    updateAlarmList();
}

void AlarmPage::onViewDidAppear() {
    Serial.println("AlarmPage: onViewDidAppear");
}

void AlarmPage::onViewWillDisappear() {
    Serial.println("AlarmPage: onViewWillDisappear");
}

void AlarmPage::onViewDidDisappear() {
    Serial.println("AlarmPage: onViewDidDisappear");
}

void AlarmPage::onViewUnload() {
    Serial.println("AlarmPage: onViewUnload");
}

void AlarmPage::onKey(lv_dir_t direction) {
    Serial.printf("AlarmPage: Key pressed - direction: %d\n", direction);

    switch (direction) {
        case LV_DIR_TOP:
            // Previous alarm (??????)
            selectedAlarm--;
            if (selectedAlarm < 0) {
                selectedAlarm = alarmCount - 1;
            }
            updateAlarmList();
            Serial.printf("Selected alarm: %d\n", selectedAlarm);
            break;

        case LV_DIR_BOTTOM:
            // Next alarm (??????)
            selectedAlarm++;
            if (selectedAlarm >= alarmCount) {
                selectedAlarm = 0;
            }
            updateAlarmList();
            Serial.printf("Selected alarm: %d\n", selectedAlarm);
            break;

        case LV_DIR_LEFT:
            // Decrease time (in edit mode)
            if (editMode) {
                alarms[selectedAlarm].hour--;
                if (alarms[selectedAlarm].hour < 0) {
                    alarms[selectedAlarm].hour = 23;
                }
                updateAlarmList();
            }
            break;

        case LV_DIR_RIGHT:
            // Increase time (in edit mode)
            if (editMode) {
                alarms[selectedAlarm].hour++;
                if (alarms[selectedAlarm].hour > 23) {
                    alarms[selectedAlarm].hour = 0;
                }
                updateAlarmList();
            }
            break;

        case LV_DIR_NONE:
            // Handled by physical buttons now
            break;
    }
}

void AlarmPage::onButton(bool pressed) {
    if (!pressed) return; // Only handle button press, not release

    // A button: Toggle alarm on/off
    toggleAlarm(selectedAlarm);
}

void AlarmPage::createAlarmUI() {
    // Page title
    titleLabel = lv_label_create(_root);
    lv_label_set_text_fmt(titleLabel, "%s ALARM", LV_SYMBOL_BELL);
    lv_obj_align(titleLabel, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_text_font(titleLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(titleLabel, lv_color_hex(0x00AA00), 0);

    // Alarm list container (??????)
    alarmList = lv_obj_create(_root);
    lv_obj_set_size(alarmList, 300, 180);
    lv_obj_align(alarmList, LV_ALIGN_TOP_MID, 0, 40);
    lv_obj_set_style_bg_color(alarmList, lv_color_white(), 0);
    lv_obj_set_style_border_width(alarmList, 1, 0);
    lv_obj_set_style_border_color(alarmList, lv_color_hex(0xDDDDDD), 0);
    lv_obj_set_style_radius(alarmList, 8, 0);
    lv_obj_set_style_pad_all(alarmList, 8, 0);
    lv_obj_clear_flag(alarmList, LV_OBJ_FLAG_SCROLLABLE); // ??????????

    // Create alarm items
    for (int i = 0; i < alarmCount; i++) {
        // Alarm item container
        alarmItems[i] = lv_obj_create(alarmList);
        lv_obj_set_size(alarmItems[i], 280, 30);
        lv_obj_set_pos(alarmItems[i], 5, i * 35 + 5);
        lv_obj_set_style_bg_color(alarmItems[i], lv_color_hex(0xF8F8F8), 0);
        lv_obj_set_style_border_width(alarmItems[i], 1, 0);
        lv_obj_set_style_border_color(alarmItems[i], lv_color_hex(0xE0E0E0), 0);
        lv_obj_set_style_radius(alarmItems[i], 4, 0);
        lv_obj_clear_flag(alarmItems[i], LV_OBJ_FLAG_SCROLLABLE);

        // Time label
        timeLabels[i] = lv_label_create(alarmItems[i]);
        lv_obj_align(timeLabels[i], LV_ALIGN_LEFT_MID, 8, -3);
        lv_obj_set_style_text_font(timeLabels[i], &lv_font_montserrat_14, 0);

        // Name label
        nameLabels[i] = lv_label_create(alarmItems[i]);
        lv_obj_align(nameLabels[i], LV_ALIGN_LEFT_MID, 8, 8);
        lv_obj_set_style_text_font(nameLabels[i], &lv_font_montserrat_14, 0);
        lv_obj_set_style_text_color(nameLabels[i], lv_color_hex(0x666666), 0);

        // Toggle switch
        toggleSwitches[i] = lv_switch_create(alarmItems[i]);
        lv_obj_align(toggleSwitches[i], LV_ALIGN_RIGHT_MID, -8, 0);
        lv_obj_set_size(toggleSwitches[i], 35, 18);
    }

    // No instructions needed
}

void AlarmPage::updateAlarmList() {
    for (int i = 0; i < alarmCount; i++) {
        const Alarm& alarm = alarms[i];
        
        // Update time display
        lv_label_set_text_fmt(timeLabels[i], "%02d:%02d", alarm.hour, alarm.minute);
        
        // Update name
        lv_label_set_text(nameLabels[i], alarm.name);
        
        // Update switch state
        if (alarm.enabled) {
            lv_obj_add_state(toggleSwitches[i], LV_STATE_CHECKED);
        } else {
            lv_obj_clear_state(toggleSwitches[i], LV_STATE_CHECKED);
        }
        
        // ???????????????????
        lv_color_t bgColor;
        lv_color_t borderColor;
        int borderWidth = 1;

        if (i == selectedAlarm) {
            // ????
            if (editMode) {
                bgColor = lv_color_hex(0xFFE0E0); // ????????
                borderColor = lv_color_hex(0xFF6666);
            } else {
                bgColor = lv_color_hex(0xE0F0FF); // ??????
                borderColor = lv_color_hex(0x66AAFF);
            }
            borderWidth = 2;
        } else if (alarm.enabled) {
            // ?????????
            bgColor = lv_color_hex(0xE8F5E8);
            borderColor = lv_color_hex(0x4CAF50);
        } else {
            // ????????
            bgColor = lv_color_hex(0xF8F8F8);
            borderColor = lv_color_hex(0xE0E0E0);
        }

        lv_obj_set_style_bg_color(alarmItems[i], bgColor, 0);
        lv_obj_set_style_border_color(alarmItems[i], borderColor, 0);
        lv_obj_set_style_border_width(alarmItems[i], borderWidth, 0);

        // Update text color based on enabled state
        lv_color_t textColor = alarm.enabled ? lv_color_hex(0x333333) : lv_color_hex(0x999999);
        lv_obj_set_style_text_color(timeLabels[i], textColor, 0);
        lv_obj_set_style_text_color(nameLabels[i], textColor, 0);
    }
    
    Serial.printf("Alarm list updated, selected: %d, edit mode: %s\n", selectedAlarm, editMode ? "ON" : "OFF");
}

void AlarmPage::toggleAlarm(int index) {
    if (index >= 0 && index < alarmCount) {
        alarms[index].enabled = !alarms[index].enabled;
        updateAlarmList();
        Serial.printf("Alarm %d %s\n", index, alarms[index].enabled ? "enabled" : "disabled");
    }
}

void AlarmPage::editAlarm(int index) {
    if (index >= 0 && index < alarmCount && editMode) {
        // Cycle through hours (simple edit)
        alarms[index].hour++;
        if (alarms[index].hour > 23) {
            alarms[index].hour = 0;
        }
        updateAlarmList();
        Serial.printf("Alarm %d time changed to %02d:%02d\n", index, alarms[index].hour, alarms[index].minute);
    }
}

void AlarmPage::checkAlarms() {
    // Simplified alarm check using millis() for demo
    // In a real implementation, you would use RTC
    static unsigned long lastCheck = 0;
    unsigned long currentTime = millis();

    // Check every 60 seconds
    if (currentTime - lastCheck < 60000) return;
    lastCheck = currentTime;

    // For demo purposes, trigger alarm based on system uptime
    // You can modify this to use actual RTC time
    int simulatedHour = (currentTime / 3600000) % 24;  // Hours since startup
    int simulatedMinute = (currentTime / 60000) % 60;  // Minutes since startup

    for (int i = 0; i < alarmCount; i++) {
        if (alarms[i].enabled &&
            alarms[i].hour == simulatedHour &&
            alarms[i].minute == simulatedMinute) {
            triggerAlarm(i);
        }
    }
}

void AlarmPage::triggerAlarm(int index) {
    if (index < 0 || index >= alarmCount) return;

    Serial.printf("ALARM TRIGGERED: %s at %02d:%02d\n",
                  alarms[index].name, alarms[index].hour, alarms[index].minute);

    // Visual notification - flash the alarm item
    if (alarmItems[index]) {
        lv_obj_set_style_bg_color(alarmItems[index], lv_color_hex(0xFF0000), 0);
    }

    // Audio notification using Grove Speaker
    const int SPEAKER_PIN = A0; // Grove connector
    pinMode(SPEAKER_PIN, OUTPUT);

    // Play alarm sound pattern
    for (int i = 0; i < 5; i++) {
        tone(SPEAKER_PIN, 1000, 200); // 1kHz for 200ms
        delay(300);
        tone(SPEAKER_PIN, 800, 200);  // 800Hz for 200ms
        delay(300);
    }
    noTone(SPEAKER_PIN);

    // Reset visual notification after 3 seconds
    // Note: In a real implementation, you'd use a timer callback
    delay(3000);
    if (alarmItems[index]) {
        lv_obj_set_style_bg_color(alarmItems[index], lv_color_hex(0xF8F8F8), 0);
    }
}
