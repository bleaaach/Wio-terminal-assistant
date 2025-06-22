/*
 * @Author: Bleaach008
 * @Date: 2025-06-21 21:51:42
 * @LastEditTime: 2025-06-22 20:43:03
 * @FilePath: \wio_terminal_lvgl_starter_code-main\src\pages\CalendarPage.h
 * @Description: 
 * 
 * Copyright (c) 2025 by 008, All Rights Reserved. 
 */
#pragma once

#include "../core/PageBase.h"
#include "../theme/style_manager.h"

class CalendarPage : public PageBase {
public:
    CalendarPage();
    virtual ~CalendarPage();

    virtual void onViewLoad() override;
    virtual void onViewDidLoad() override;
    virtual void onViewWillAppear() override;
    virtual void onViewDidAppear() override;
    virtual void onViewWillDisappear() override;
    virtual void onViewDidDisappear() override;
    virtual void onViewUnload() override;

    virtual void onKey(lv_dir_t direction) override;
    virtual void onButton(bool pressed) override;

    // 更新方法
    void update();

private:
    void createCalendarUI();
    void updateCalendar();
    void initializeRTC();
    void updateCurrentTime();

private:
    lv_obj_t* titleLabel;
    lv_obj_t* timeLabel;
    lv_obj_t* monthLabel;
    lv_obj_t* calendarGrid;
    lv_obj_t* dayLabels[7];
    lv_obj_t* dateLabels[42]; // 6 weeks * 7 days

    int currentYear;
    int currentMonth;
    int currentDay;
    int currentHour;
    int currentMinute;

    // 时间刷新相关
    unsigned long lastTimeUpdate;
    static const unsigned long TIME_UPDATE_INTERVAL = 1000; // 1秒更新一次
};
