#pragma once

#include "../core/PageBase.h"

class AlarmPage : public PageBase {
public:
    AlarmPage();
    virtual ~AlarmPage();

    virtual void onViewLoad() override;
    virtual void onViewDidLoad() override;
    virtual void onViewWillAppear() override;
    virtual void onViewDidAppear() override;
    virtual void onViewWillDisappear() override;
    virtual void onViewDidDisappear() override;
    virtual void onViewUnload() override;

    virtual void onKey(lv_dir_t direction) override;
    virtual void onButton(bool pressed) override;

    // Public members for AppManager access
    bool editMode;
    void updateAlarmList();
    void checkAlarms(); // Check if any alarm should trigger

private:
    void createAlarmUI();
    void toggleAlarm(int index);
    void editAlarm(int index);
    void triggerAlarm(int index); // Trigger alarm notification

private:
    lv_obj_t* titleLabel;
    lv_obj_t* alarmList;
    lv_obj_t* alarmItems[5];
    lv_obj_t* timeLabels[5];
    lv_obj_t* nameLabels[5];
    lv_obj_t* toggleSwitches[5];
    lv_obj_t* addButton;

    int selectedAlarm;

    struct Alarm {
        int hour;
        int minute;
        const char* name;
        bool enabled;
        bool repeat[7]; // Days of week
    };

    static Alarm alarms[5];
    static const int alarmCount;
};
