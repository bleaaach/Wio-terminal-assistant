#pragma once

#include "../core/PageBase.h"
#include "../theme/style_manager.h"

class TimerPage : public PageBase {
public:
    TimerPage();
    virtual ~TimerPage();

    virtual void onViewLoad() override;
    virtual void onViewDidLoad() override;
    virtual void onViewWillAppear() override;
    virtual void onViewDidAppear() override;
    virtual void onViewWillDisappear() override;
    virtual void onViewDidDisappear() override;
    virtual void onViewUnload() override;

    virtual void onKey(lv_dir_t direction) override;
    virtual void onButton(bool pressed) override;
    void onButtonA(bool pressed);
    void onButtonB(bool pressed);
    void onButtonC(bool pressed);

    // Public methods for AppManager access
    void resetTimer();
    void updateTimer(); // Called periodically to update display
    void adjustTime(int minutes); // ???????

private:
    void createTimerUI();
    void updateTimerDisplay();
    void startStopTimer();

private:
    // UI ???
    lv_obj_t* titleLabel;
    lv_obj_t* timerDisplay;
    lv_obj_t* statusLabel;
    lv_obj_t* instructionLabel;
    lv_obj_t* progressArc;

    // ???????
    bool isRunning;
    int totalMinutes;        // ???????????????
    int remainingSeconds;    // ????????
    unsigned long lastUpdateTime; // ?????????
};
