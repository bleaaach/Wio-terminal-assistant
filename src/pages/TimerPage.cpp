#include "TimerPage.h"
#include <Arduino.h>

// ??? Timer ?? - ??????

TimerPage::TimerPage() {
    titleLabel = nullptr;
    timerDisplay = nullptr;
    statusLabel = nullptr;
    instructionLabel = nullptr;

    // ????????????
    isRunning = false;
    totalMinutes = 5;        // ???5????
    remainingSeconds = totalMinutes * 60;
    lastUpdateTime = 0;
}

TimerPage::~TimerPage() {
    // Destructor
}

void TimerPage::onViewLoad() {
    Serial.println("TimerPage: onViewLoad");
    // Traditional style: light purple background
    lv_obj_set_style_bg_color(_root, lv_color_hex(0xFFF0FF), 0);
    lv_obj_set_style_bg_opa(_root, LV_OPA_COVER, 0);
}

void TimerPage::onViewDidLoad() {
    Serial.println("TimerPage: onViewDidLoad");
    createTimerUI();
}

void TimerPage::onViewWillAppear() {
    Serial.println("TimerPage: onViewWillAppear");
    updateTimerDisplay();
}

void TimerPage::onViewDidAppear() {
    Serial.println("TimerPage: onViewDidAppear");
}

void TimerPage::onViewWillDisappear() {
    Serial.println("TimerPage: onViewWillDisappear");
}

void TimerPage::onViewDidDisappear() {
    Serial.println("TimerPage: onViewDidDisappear");
}

void TimerPage::onViewUnload() {
    Serial.println("TimerPage: onViewUnload");
}

void TimerPage::onKey(lv_dir_t direction) {
    Serial.printf("TimerPage: Key pressed - direction: %d\n", direction);

    // ????????????????????????????
    switch (direction) {
        case LV_DIR_LEFT:
        case LV_DIR_RIGHT:
        case LV_DIR_TOP:
        case LV_DIR_BOTTOM:
            // ??????????????????????????????ABC???????
            Serial.printf("Timer: Joystick navigation - direction: %d\n", direction);
            break;

        case LV_DIR_NONE:
            break;
    }
}

void TimerPage::onButton(bool pressed) {
    if (!pressed) return;
    startStopTimer();
    Serial.printf("Timer: %s\n", isRunning ? "Started" : "Stopped");
}

// ????????????????? - ????????????

void TimerPage::createTimerUI() {
    // Page title (???????)
    titleLabel = lv_label_create(_root);
    lv_label_set_text_fmt(titleLabel, "%s TIMER", LV_SYMBOL_LOOP);
    lv_obj_align(titleLabel, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_text_font(titleLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(titleLabel, lv_color_hex(0xFF9800), 0); // ???????

    // ??μ???????? (Timer??????)
    lv_obj_t* timerContainer = lv_obj_create(_root);
    lv_obj_set_size(timerContainer, 130, 130);
    lv_obj_align(timerContainer, LV_ALIGN_CENTER, 0, 10);
    lv_obj_set_style_bg_color(timerContainer, lv_color_hex(0xFFF3E0), 0); // ????????
    lv_obj_set_style_radius(timerContainer, 65, 0); // 圆形
    lv_obj_set_style_border_width(timerContainer, 3, 0);
    lv_obj_set_style_border_color(timerContainer, lv_color_hex(0xFF9800), 0);
    lv_obj_clear_flag(timerContainer, LV_OBJ_FLAG_SCROLLABLE); // 禁用滚动，隐藏滚动条

    // 圆形进度条 (Timer特色)
    progressArc = lv_arc_create(timerContainer);
    lv_obj_set_size(progressArc, 100, 100);
    lv_obj_align(progressArc, LV_ALIGN_CENTER, 0, 0);
    lv_arc_set_range(progressArc, 0, 300); // 5???? = 300??
    lv_arc_set_value(progressArc, 300);
    lv_obj_set_style_arc_color(progressArc, lv_color_hex(0xFF9800), LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(progressArc, 6, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(progressArc, lv_color_hex(0xFFE0B2), LV_PART_MAIN);
    lv_obj_set_style_arc_width(progressArc, 6, LV_PART_MAIN);
    lv_obj_remove_style(progressArc, NULL, LV_PART_KNOB); // ???????

    // Timer??? (????)
    lv_obj_t* timerIcon = lv_label_create(timerContainer);
    lv_label_set_text(timerIcon, LV_SYMBOL_LOOP);
    lv_obj_align(timerIcon, LV_ALIGN_CENTER, 0, -15);
    lv_obj_set_style_text_font(timerIcon, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(timerIcon, lv_color_hex(0xFF9800), 0);

    // ?????? (?????壬???)
    timerDisplay = lv_label_create(timerContainer);
    lv_label_set_text(timerDisplay, "05:00");
    lv_obj_align(timerDisplay, LV_ALIGN_CENTER, 0, 5);
    lv_obj_set_style_text_font(timerDisplay, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(timerDisplay, lv_color_hex(0xFF9800), 0);

    // ?????
    statusLabel = lv_label_create(timerContainer);
    lv_label_set_text(statusLabel, "Ready");
    lv_obj_align(statusLabel, LV_ALIGN_CENTER, 0, 20);
    lv_obj_set_style_text_font(statusLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(statusLabel, lv_color_hex(0xF57C00), 0);


    updateTimerDisplay();
}

void TimerPage::updateTimerDisplay() {
    // ???????????
    if (isRunning) {
        unsigned long currentTime = millis();
        if (currentTime - lastUpdateTime >= 1000) { // ??????
            remainingSeconds--;
            lastUpdateTime = currentTime;

            if (remainingSeconds <= 0) {
                // ?????
                isRunning = false;
                remainingSeconds = 0;
                lv_label_set_text(timerDisplay, "00:00");
                lv_label_set_text(statusLabel, " Time's Up!");
                lv_obj_set_style_text_color(statusLabel, lv_color_hex(0xFF0000), 0);

                // ?????????
                const int SPEAKER_PIN = A0;
                pinMode(SPEAKER_PIN, OUTPUT);
                for (int i = 0; i < 3; i++) {
                    tone(SPEAKER_PIN, 1200, 300);
                    delay(400);
                }
                noTone(SPEAKER_PIN);

                // Timer finished
                return;
            }
        }
    }

    // ??????????
    int minutes = remainingSeconds / 60;
    int seconds = remainingSeconds % 60;
    lv_label_set_text_fmt(timerDisplay, "%02d:%02d", minutes, seconds);

    // ??????ν?????
    int totalSeconds = totalMinutes * 60;
    if (totalSeconds > 0) {
        int progressValue = (remainingSeconds * 300) / totalSeconds; // ???0-300??Χ
        lv_arc_set_value(progressArc, progressValue);
    }

    // Update status (?????)
    if (isRunning) {
        lv_label_set_text(statusLabel, "Running");
        lv_obj_set_style_text_color(statusLabel, lv_color_hex(0x4CAF50), 0);
    } else {
        if (remainingSeconds > 0) {
            lv_label_set_text(statusLabel, "Ready");
        } else {
            lv_label_set_text(statusLabel, "Finished");
            lv_obj_set_style_text_color(statusLabel, lv_color_hex(0xFF5722), 0);
        }
        if (remainingSeconds > 0) {
            lv_obj_set_style_text_color(statusLabel, lv_color_hex(0xF57C00), 0);
        }
    }
}

// ??? updateCountdownList ???? - ??????????

void TimerPage::startStopTimer() {
    if (isRunning) {
        // ????????
        isRunning = false;
        Serial.println("Timer paused");
    } else {
        // ????????
        if (remainingSeconds > 0) {
            isRunning = true;
            lastUpdateTime = millis();
            Serial.println("Timer started");
        }
    }
    updateTimerDisplay();
}

void TimerPage::resetTimer() {
    isRunning = false;
    remainingSeconds = totalMinutes * 60;
    updateTimerDisplay();
    Serial.println("Timer reset");
}

// ??? switchMode ???? - ???????

void TimerPage::updateTimer() {
    if (isRunning) {
        updateTimerDisplay();
    }
}

void TimerPage::onButtonA(bool pressed) {
    if (!pressed) return;
    // A???????/????????
    startStopTimer();
    Serial.printf("Timer A: %s\n", isRunning ? "Started" : "Stopped");
}

void TimerPage::onButtonB(bool pressed) {
    if (!pressed) return;
    // B??????????? (+1????)
    if (!isRunning) {
        adjustTime(1); // ????1????
        Serial.printf("Timer B: Added 1 minute, total: %d minutes\n", totalMinutes);
    }
}

void TimerPage::onButtonC(bool pressed) {
    if (!pressed) return;
    // C??????????? (-1????)
    if (!isRunning) {
        adjustTime(-1); // ????1????
        Serial.printf("Timer C: Removed 1 minute, total: %d minutes\n", totalMinutes);
    }
}

void TimerPage::adjustTime(int minutes) {
    if (isRunning) {
        Serial.println("Cannot adjust time while timer is running");
        return; // ????????????
    }

    int oldMinutes = totalMinutes;
    totalMinutes += minutes;
    if (totalMinutes < 1) totalMinutes = 1;     // ????1????
    if (totalMinutes > 120) totalMinutes = 120; // ???120????

    remainingSeconds = totalMinutes * 60;

    Serial.printf("Time adjusted: %d -> %d minutes (change: %+d)\n",
                  oldMinutes, totalMinutes, minutes);

    updateTimerDisplay();
}
