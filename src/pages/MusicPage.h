#pragma once

#include <Arduino.h>
#include "../core/PageBase.h"
#include "../theme/style_manager.h"

class MusicPage : public PageBase {
public:
    MusicPage();
    virtual ~MusicPage();

    virtual void onViewLoad() override;
    virtual void onViewDidLoad() override;
    virtual void onViewWillAppear() override;
    virtual void onViewDidAppear() override;
    virtual void onViewWillDisappear() override;
    virtual void onViewDidDisappear() override;
    virtual void onViewUnload() override;

    virtual void onKey(lv_dir_t direction) override;
    virtual void onButton(bool pressed) override;
    void updatePlaybackProgress(); // 公共函数，供 AppManager 调用

private:
    void createMusicUI();
    void updateTrackInfo();
    void updateProgress();
    void updateTimeDisplay();
    void playCurrentTrack();
    void stopCurrentTrack();

private:
    lv_obj_t* titleLabel;
    lv_obj_t* albumCover;
    lv_obj_t* coverIcon;
    lv_obj_t* trackTitle;
    lv_obj_t* artistLabel;
    lv_obj_t* timeLabel;
    lv_obj_t* statusLabel;
    lv_obj_t* progressBar;
    lv_obj_t* playButton; // Status display only

    bool isPlaying;
    int currentTrack;
    int currentTime; // in seconds
    int totalTime;   // in seconds
    unsigned long lastUpdateTime; // for progress simulation

    // Music management
    static const int MAX_TRACKS = 10;
    char musicFiles[MAX_TRACKS][64]; // Fixed-size char arrays
    int trackCount;
    bool sdCardAvailable;

    // Grove Speaker pin (????? Grove ???)
    static const int SPEAKER_PIN = A0; // Grove A0 ???

    // SD card functions
    void loadMusicFiles();
};
