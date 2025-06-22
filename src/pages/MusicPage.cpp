#include "MusicPage.h"
#include <Arduino.h>

// Try to include SD card support if available
#ifdef ARDUINO_ARCH_SAMD
    #include "Seeed_FS.h"
    #include "SD/Seeed_SD.h"
    #define SEEED_FS_H
#endif

MusicPage::MusicPage() {
    titleLabel = nullptr;
    albumCover = nullptr;
    coverIcon = nullptr;
    trackTitle = nullptr;
    artistLabel = nullptr;
    timeLabel = nullptr;
    statusLabel = nullptr;
    progressBar = nullptr;
    playButton = nullptr;

    isPlaying = false;
    currentTrack = 0;
    currentTime = 0;
    totalTime = 180;
    lastUpdateTime = 0; // Default 3 minutes
    trackCount = 0;
    sdCardAvailable = false;

    // Initialize Grove Speaker pin
    pinMode(SPEAKER_PIN, OUTPUT);
    digitalWrite(SPEAKER_PIN, LOW);
}

MusicPage::~MusicPage() {
    // Destructor - cleanup handled by PageManager
}

void MusicPage::onViewLoad() {
    Serial.println("MusicPage: onViewLoad");
    
    // Set background color
    lv_obj_set_style_bg_color(_root, lv_color_hex(0xF0F8FF), 0); // Light blue
    lv_obj_set_style_bg_opa(_root, LV_OPA_COVER, 0);
}

void MusicPage::onViewDidLoad() {
    Serial.println("MusicPage: onViewDidLoad");
    loadMusicFiles();
    createMusicUI();
}

void MusicPage::onViewWillAppear() {
    Serial.println("MusicPage: onViewWillAppear");
    updateTrackInfo();
    updateProgress();
}

void MusicPage::onViewDidAppear() {
    Serial.println("MusicPage: onViewDidAppear");
}

void MusicPage::onViewWillDisappear() {
    Serial.println("MusicPage: onViewWillDisappear");
}

void MusicPage::onViewDidDisappear() {
    Serial.println("MusicPage: onViewDidDisappear");
}

void MusicPage::onViewUnload() {
    Serial.println("MusicPage: onViewUnload");
}

void MusicPage::onKey(lv_dir_t direction) {
    Serial.printf("MusicPage: Key pressed - direction: %d\n", direction);

    switch (direction) {
        case LV_DIR_LEFT:
            // Previous track
            currentTrack--;
            if (currentTrack < 0) {
                currentTrack = trackCount - 1;
            }
            currentTime = 0;
            totalTime = 180; // Default 3 minutes
            updateTrackInfo();
            updateProgress();
            break;

        case LV_DIR_RIGHT:
            // Next track
            currentTrack++;
            if (currentTrack >= trackCount) {
                currentTrack = 0;
            }
            currentTime = 0;
            totalTime = 180; // Default 3 minutes
            updateTrackInfo();
            updateProgress();
            break;

        case LV_DIR_TOP:
            // Fast forward
            currentTime += 10;
            if (currentTime > totalTime) {
                currentTime = totalTime;
            }
            updateProgress();
            break;

        case LV_DIR_BOTTOM:
            // Rewind
            currentTime -= 10;
            if (currentTime < 0) {
                currentTime = 0;
            }
            updateProgress();
            break;

        case LV_DIR_NONE:
            // This is handled by physical buttons now
            break;
    }
}

void MusicPage::onButton(bool pressed) {
    if (!pressed) return; // Only handle button press, not release

    // A button: Play/Pause
    isPlaying = !isPlaying;

    if (isPlaying) {
        playCurrentTrack();
    } else {
        stopCurrentTrack();
    }

    updateTrackInfo(); // This will update the play button display
    Serial.printf("Music: %s\n", isPlaying ? "Playing" : "Paused");
}

void MusicPage::createMusicUI() {
    // Page title
    titleLabel = lv_label_create(_root);
    lv_label_set_text_fmt(titleLabel, "%s MUSIC", LV_SYMBOL_AUDIO);
    lv_obj_align(titleLabel, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_text_font(titleLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(titleLabel, lv_color_hex(0x0066FF), 0);

    // Album cover placeholder
    albumCover = lv_obj_create(_root);
    lv_obj_set_size(albumCover, 100, 100);
    lv_obj_align(albumCover, LV_ALIGN_TOP_MID, 0, 40);
    lv_obj_set_style_bg_color(albumCover, lv_color_hex(0xE0E0E0), 0);
    lv_obj_set_style_radius(albumCover, 8, 0);
    lv_obj_set_style_border_width(albumCover, 2, 0);
    lv_obj_set_style_border_color(albumCover, lv_color_hex(0xCCCCCC), 0);

    // Album cover icon (store reference for updates)
    coverIcon = lv_label_create(albumCover);
    lv_label_set_text(coverIcon, LV_SYMBOL_AUDIO);
    lv_obj_center(coverIcon);
    lv_obj_set_style_text_font(coverIcon, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(coverIcon, lv_color_hex(0x888888), 0);

    // Track title
    trackTitle = lv_label_create(_root);
    lv_obj_align(trackTitle, LV_ALIGN_TOP_MID, 0, 150);
    lv_obj_set_style_text_font(trackTitle, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(trackTitle, lv_color_hex(0x333333), 0);

    // Artist label
    artistLabel = lv_label_create(_root);
    lv_obj_align(artistLabel, LV_ALIGN_TOP_MID, 0, 170);
    lv_obj_set_style_text_font(artistLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(artistLabel, lv_color_hex(0x666666), 0);

    // Time label
    timeLabel = lv_label_create(_root);
    lv_obj_align(timeLabel, LV_ALIGN_TOP_MID, 0, 190);
    lv_obj_set_style_text_font(timeLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(timeLabel, lv_color_hex(0x888888), 0);

    // Progress bar
    progressBar = lv_bar_create(_root);
    lv_obj_set_size(progressBar, 200, 6);
    lv_obj_align(progressBar, LV_ALIGN_TOP_MID, 0, 210);
    lv_obj_set_style_bg_color(progressBar, lv_color_hex(0xE0E0E0), 0);
    lv_obj_set_style_bg_color(progressBar, lv_color_hex(0x0066FF), LV_PART_INDICATOR);

    // Status indicator (replace control buttons)
    statusLabel = lv_label_create(_root);
    lv_obj_align(statusLabel, LV_ALIGN_TOP_MID, 0, 225);
    lv_obj_set_style_text_font(statusLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(statusLabel, lv_color_hex(0x0066FF), 0);

    // Store reference for updates
    playButton = statusLabel;

    // No instructions needed
}

void MusicPage::updateTrackInfo() {
    if (!trackTitle || currentTrack >= trackCount) return;

    // Use music files (either from SD card or default)
    char displayName[64];
    strncpy(displayName, musicFiles[currentTrack], sizeof(displayName) - 1);
    displayName[sizeof(displayName) - 1] = '\0';

    // Remove file extension for display if it's a real file
    if (sdCardAvailable) {
        char* dotPos = strrchr(displayName, '.');
        if (dotPos && (strcmp(dotPos, ".wav") == 0 || strcmp(dotPos, ".mp3") == 0)) {
            *dotPos = '\0'; // Remove extension
        }
    }

    lv_label_set_text(trackTitle, displayName);

    // ??????????????????
    if (coverIcon && albumCover) {
        const char* icons[] = {LV_SYMBOL_AUDIO, LV_SYMBOL_VOLUME_MAX, LV_SYMBOL_BELL, LV_SYMBOL_SETTINGS, LV_SYMBOL_WIFI};
        const uint32_t colors[] = {0xFF6B6B, 0x4ECDC4, 0x45B7D1, 0x96CEB4, 0xFECA57};

        int iconIndex = currentTrack % 5;
        lv_label_set_text(coverIcon, icons[iconIndex]);
        lv_obj_set_style_bg_color(albumCover, lv_color_hex(colors[iconIndex]), 0);
        lv_obj_set_style_text_color(coverIcon, lv_color_hex(0xFFFFFF), 0);
    }

    // ????????????
    if (sdCardAvailable) {
        lv_label_set_text_fmt(artistLabel, "SD Card (%d tracks)", trackCount);
    } else {
        lv_label_set_text(artistLabel, "No SD Card - Demo Mode");
    }

    // Update status indicator
    if (playButton) {
        lv_label_set_text_fmt(playButton, "%s %s",
                             isPlaying ? LV_SYMBOL_PAUSE : LV_SYMBOL_PLAY,
                             isPlaying ? "Playing" : "Paused");
    }

    // Update SD card status
    if (statusLabel) {
        if (sdCardAvailable && trackCount > 0) {
            lv_label_set_text_fmt(statusLabel, "SD Card: %d tracks found", trackCount);
            lv_obj_set_style_text_color(statusLabel, lv_color_hex(0x00AA00), 0);
        } else if (!sdCardAvailable) {
            lv_label_set_text(statusLabel, "No SD Card - Using Demo Tracks");
            lv_obj_set_style_text_color(statusLabel, lv_color_hex(0xFF6600), 0);
        } else {
            lv_label_set_text(statusLabel, "SD Card: No music files found");
            lv_obj_set_style_text_color(statusLabel, lv_color_hex(0xFF0000), 0);
        }
    }

    totalTime = 180; // Default 3 minutes

    Serial.printf("Music: Updated to track %d - %s\n",
                  currentTrack, displayName);
}

void MusicPage::updateProgress() {
    if (!progressBar || !timeLabel) return;

    // Update progress bar
    int progress = (currentTime * 100) / totalTime;
    lv_bar_set_value(progressBar, progress, LV_ANIM_OFF);

    // Update time display
    int currentMin = currentTime / 60;
    int currentSec = currentTime % 60;
    int totalMin = totalTime / 60;
    int totalSec = totalTime % 60;

    lv_label_set_text_fmt(timeLabel, "%d:%02d / %d:%02d",
                          currentMin, currentSec, totalMin, totalSec);
}

void MusicPage::updateTimeDisplay() {
    if (!timeLabel) return;

    // Update time display
    int currentMin = currentTime / 60;
    int currentSec = currentTime % 60;
    int totalMin = totalTime / 60;
    int totalSec = totalTime % 60;

    lv_label_set_text_fmt(timeLabel, "%d:%02d / %d:%02d",
                          currentMin, currentSec, totalMin, totalSec);
}

void MusicPage::loadMusicFiles() {
    Serial.println("Loading music files...");

    trackCount = 0;
    sdCardAvailable = false;

    // Try to initialize SD card with different approaches
    #ifdef ARDUINO_ARCH_SAMD
    // Try Seeed FS first
    if (SD.begin(SDCARD_SS_PIN, SDCARD_SPI, 4000000UL)) {
        Serial.println("SD Card initialized with Seeed FS");
        sdCardAvailable = true;

        // Try to open Music directory
        File musicDir = SD.open("/Music");
        if (!musicDir) {
            // Try root directory if Music folder doesn't exist
            musicDir = SD.open("/");
            Serial.println("Music folder not found, scanning root directory");
        }

        if (musicDir) {
            File file = musicDir.openNextFile();
            while (file && trackCount < MAX_TRACKS) {
                if (!file.isDirectory()) {
                    const char* fileName = file.name();
                    int len = strlen(fileName);

                    // Check for audio file extensions (case insensitive)
                    if (len > 4) {
                        char ext[5];
                        strncpy(ext, fileName + len - 4, 4);
                        ext[4] = '\0';

                        // Convert to lowercase for comparison
                        for (int i = 0; i < 4; i++) {
                            if (ext[i] >= 'A' && ext[i] <= 'Z') {
                                ext[i] += 32;
                            }
                        }

                        // 只支持 WAV 格式（未压缩音频）
                        if (strcmp(ext, ".wav") == 0) {
                            strncpy(musicFiles[trackCount], fileName, sizeof(musicFiles[trackCount]) - 1);
                            musicFiles[trackCount][sizeof(musicFiles[trackCount]) - 1] = '\0';

                            // ????????С?????????
                            size_t fileSize = file.size();
                            Serial.printf("Found music file: %s (Size: %d bytes)\n", fileName, fileSize);

                            trackCount++;
                        }
                    }
                }
                file.close();
                file = musicDir.openNextFile();
            }
            musicDir.close();
        } else {
            Serial.println("Could not open any directory on SD card");
        }
    } else {
        Serial.println("SD Card initialization failed");
    }
    #endif

    // If no SD card or no music files, use default tracks
    if (!sdCardAvailable || trackCount == 0) {
        Serial.println("Using default music tracks");
        strncpy(musicFiles[0], "Sunny Day", sizeof(musicFiles[0]) - 1);
        strncpy(musicFiles[1], "Night Sky", sizeof(musicFiles[1]) - 1);
        strncpy(musicFiles[2], "Ocean Waves", sizeof(musicFiles[2]) - 1);
        strncpy(musicFiles[3], "Mountain View", sizeof(musicFiles[3]) - 1);
        strncpy(musicFiles[4], "City Lights", sizeof(musicFiles[4]) - 1);
        // Ensure null termination
        for (int i = 0; i < 5; i++) {
            musicFiles[i][sizeof(musicFiles[i]) - 1] = '\0';
        }
        trackCount = 5;
        sdCardAvailable = false; // Mark as demo mode
    }

    Serial.printf("Loaded %d music tracks (SD Card: %s)\n", trackCount, sdCardAvailable ? "Yes" : "No");
}

void MusicPage::playCurrentTrack() {
    if (currentTrack >= trackCount) return;

    Serial.printf("Playing track: %s\n", musicFiles[currentTrack]);

    // Initialize Grove Speaker
    pinMode(SPEAKER_PIN, OUTPUT);

    if (isPlaying) {
        if (sdCardAvailable) {
            // WAV file playback
            Serial.printf("Playing WAV file: %s\n", musicFiles[currentTrack]);

            // Play start tone
            tone(SPEAKER_PIN, 1000, 500);
            delay(600);

            // Estimate duration from file size
            totalTime = 180 + (currentTrack * 30); // Default duration

        } else {
            // Demo mode with different melodies
            const int melodies[][6] = {
                {262, 294, 330, 349, 392, 440}, // C major
                {440, 494, 523, 587, 659, 698}, // A major
                {330, 370, 415, 440, 494, 554}, // E major
                {392, 440, 494, 523, 587, 659}, // G major
                {294, 330, 370, 392, 440, 494}  // D major
            };

            int melodyIndex = currentTrack % 5;
            const int* melody = melodies[melodyIndex];

            // Play melody preview
            for (int i = 0; i < 3; i++) {
                tone(SPEAKER_PIN, melody[i], 200);
                delay(250);
            }

            totalTime = 180 + (currentTrack * 30);
            Serial.printf("Demo mode: Playing melody pattern %d\n", melodyIndex);
        }

        // Initialize playback state
        currentTime = 0;
        lastUpdateTime = millis();
    }
}

void MusicPage::updatePlaybackProgress() {
    if (isPlaying && totalTime > 0) {
        unsigned long now = millis();
        if (now - lastUpdateTime >= 1000) { // ?????????
            currentTime++;
            lastUpdateTime = now;

            // ????????????????
            if (currentTime >= totalTime) {
                isPlaying = false;
                currentTime = totalTime;
                stopCurrentTrack();
                Serial.println("Track finished");
            }

            // ??????????
            updateProgress();
        }
    }
}

void MusicPage::stopCurrentTrack() {
    Serial.println("Stopping current track");
    noTone(SPEAKER_PIN);
    digitalWrite(SPEAKER_PIN, LOW);
}
