#include "MemoPage.h"
#include <Arduino.h>

// Static memo data
MemoPage::Memo MemoPage::memos[6] = {
    {"Today's Tasks", "• Finish UI design\n• Test alarm system\n• Update documentation", LV_SYMBOL_LIST, lv_color_hex(0xFFE0B2)},
    {"Shopping List", "• Milk\n• Bread\n• Fruits\n• Coffee beans", LV_SYMBOL_HOME, lv_color_hex(0xE8F5E9)},
    {"Travel Plan", "July 10th - Yunnan\n• Flight: 8:30 AM\n• Hotel: Lijiang\n• Activities: Old Town", LV_SYMBOL_GPS, lv_color_hex(0xE3F2FD)},
    {"Meeting Notes", "Project Review\n• Status: On track\n• Next: Testing phase\n• Due: End of month", LV_SYMBOL_FILE, lv_color_hex(0xF3E5F5)},
    {"Ideas", "• Smart home automation\n• Voice control\n• Energy monitoring\n• Mobile app", LV_SYMBOL_SETTINGS, lv_color_hex(0xFFF3E0)},
    {"Reminders", "• Call dentist\n• Pay electricity bill\n• Birthday gift for mom\n• Car maintenance", LV_SYMBOL_BELL, lv_color_hex(0xFCE4EC)}
};

const int MemoPage::memoCount = 6;

MemoPage::MemoPage() {
    titleLabel = nullptr;
    memoGrid = nullptr;
    
    for (int i = 0; i < 6; i++) {
        memoCards[i] = nullptr;
        memoTitles[i] = nullptr;
        memoContents[i] = nullptr;
        memoIcons[i] = nullptr;
    }
    
    selectedMemo = 0;
    viewMode = false; // Start in grid mode
}

MemoPage::~MemoPage() {
    // Destructor - cleanup handled by PageManager
}

void MemoPage::onViewLoad() {
    Serial.println("MemoPage: onViewLoad");
    
    // Set background color
    lv_obj_set_style_bg_color(_root, lv_color_hex(0xFFF0F8), 0); // Light pink
    lv_obj_set_style_bg_opa(_root, LV_OPA_COVER, 0);
}

void MemoPage::onViewDidLoad() {
    Serial.println("MemoPage: onViewDidLoad");
    createMemoUI();
}

void MemoPage::onViewWillAppear() {
    Serial.println("MemoPage: onViewWillAppear");
    updateMemoDisplay();
}

void MemoPage::onViewDidAppear() {
    Serial.println("MemoPage: onViewDidAppear");
}

void MemoPage::onViewWillDisappear() {
    Serial.println("MemoPage: onViewWillDisappear");
}

void MemoPage::onViewDidDisappear() {
    Serial.println("MemoPage: onViewDidDisappear");
}

void MemoPage::onViewUnload() {
    Serial.println("MemoPage: onViewUnload");
}

void MemoPage::onKey(lv_dir_t direction) {
    Serial.printf("MemoPage: Key pressed - direction: %d\n", direction);

    if (viewMode) {
        // In view mode - no joystick actions, use A button to go back
        return;
    } else {
        // In grid mode - navigation (摇杆只用于上下导航)
        switch (direction) {
            case LV_DIR_LEFT:
                // 摇杆左：上一个备忘录（线性导航）
                selectedMemo--;
                if (selectedMemo < 0) {
                    selectedMemo = memoCount - 1;
                }
                updateMemoDisplay();
                Serial.printf("Selected memo: %d (LEFT)\n", selectedMemo);
                break;

            case LV_DIR_RIGHT:
                // 摇杆右：下一个备忘录（线性导航）
                selectedMemo++;
                if (selectedMemo >= memoCount) {
                    selectedMemo = 0;
                }
                updateMemoDisplay();
                Serial.printf("Selected memo: %d (RIGHT)\n", selectedMemo);
                break;

            case LV_DIR_TOP:
                // 摇杆上：上一个备忘录
                selectedMemo--;
                if (selectedMemo < 0) {
                    selectedMemo = memoCount - 1;
                }
                updateMemoDisplay();
                Serial.printf("Selected memo: %d (UP)\n", selectedMemo);
                break;

            case LV_DIR_BOTTOM:
                // 摇杆下：下一个备忘录
                selectedMemo++;
                if (selectedMemo >= memoCount) {
                    selectedMemo = 0;
                }
                updateMemoDisplay();
                Serial.printf("Selected memo: %d (DOWN)\n", selectedMemo);
                break;

            case LV_DIR_NONE:
                // Handled by physical buttons now
                break;
        }
    }
}

void MemoPage::onButton(bool pressed) {
    if (!pressed) return; // Only handle button press, not release

    // A button: View/Back
    if (viewMode) {
        viewMode = false;
        updateMemoDisplay();
    } else {
        viewMode = true;
        updateMemoDisplay();
    }
}

void MemoPage::createMemoUI() {
    // Page title
    titleLabel = lv_label_create(_root);
    lv_label_set_text_fmt(titleLabel, "%s MEMO", LV_SYMBOL_EDIT);
    lv_obj_align(titleLabel, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_text_font(titleLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(titleLabel, lv_color_hex(0xFF0066), 0);

    // Memo grid container (调整大小以完整显示)
    memoGrid = lv_obj_create(_root);
    lv_obj_set_size(memoGrid, 310, 180);
    lv_obj_align(memoGrid, LV_ALIGN_TOP_MID, 0, 35);
    lv_obj_set_style_bg_opa(memoGrid, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(memoGrid, 0, 0);
    lv_obj_set_style_pad_all(memoGrid, 2, 0);
    lv_obj_set_scrollbar_mode(memoGrid, LV_SCROLLBAR_MODE_AUTO); // 添加滚动条

    // Create memo cards (2x3 grid)
    for (int i = 0; i < memoCount; i++) {
        int row = i / 2;
        int col = i % 2;
        
        // Memo card (调整大小)
        memoCards[i] = lv_obj_create(memoGrid);
        lv_obj_set_size(memoCards[i], 145, 55);
        lv_obj_set_pos(memoCards[i], col * 150 + 5, row * 60 + 5);
        lv_obj_set_style_bg_color(memoCards[i], memos[i].color, 0);
        lv_obj_set_style_border_width(memoCards[i], 1, 0);
        lv_obj_set_style_border_color(memoCards[i], lv_color_hex(0xDDDDDD), 0);
        lv_obj_set_style_radius(memoCards[i], 8, 0);
        lv_obj_set_style_pad_all(memoCards[i], 6, 0);
        lv_obj_clear_flag(memoCards[i], LV_OBJ_FLAG_SCROLLABLE);

        // Icon
        memoIcons[i] = lv_label_create(memoCards[i]);
        lv_label_set_text(memoIcons[i], memos[i].icon);
        lv_obj_align(memoIcons[i], LV_ALIGN_TOP_LEFT, 2, 2);
        lv_obj_set_style_text_font(memoIcons[i], &lv_font_montserrat_14, 0);
        lv_obj_set_style_text_color(memoIcons[i], lv_color_hex(0x666666), 0);

        // Title
        memoTitles[i] = lv_label_create(memoCards[i]);
        lv_label_set_text(memoTitles[i], memos[i].title);
        lv_obj_align(memoTitles[i], LV_ALIGN_TOP_LEFT, 2, 18);
        lv_obj_set_style_text_font(memoTitles[i], &lv_font_montserrat_14, 0);
        lv_obj_set_style_text_color(memoTitles[i], lv_color_hex(0x333333), 0);

        // Content preview
        memoContents[i] = lv_label_create(memoCards[i]);
        lv_obj_align(memoContents[i], LV_ALIGN_TOP_LEFT, 2, 35);
        lv_obj_set_style_text_font(memoContents[i], &lv_font_montserrat_14, 0);
        lv_obj_set_style_text_color(memoContents[i], lv_color_hex(0x666666), 0);
        lv_obj_set_size(memoContents[i], 130, 20);
    }

    // No instructions needed
}

void MemoPage::updateMemoDisplay() {
    // Use member variables instead of static to avoid persistence issues
    static lv_obj_t* detailView = nullptr;
    static lv_obj_t* detailTitle = nullptr;
    static lv_obj_t* detailContent = nullptr;

    if (viewMode) {
        // Hide grid, show detailed view
        lv_obj_add_flag(memoGrid, LV_OBJ_FLAG_HIDDEN);

        // Create detailed view if it doesn't exist
        if (!detailView) {
            detailView = lv_obj_create(_root);
            lv_obj_set_size(detailView, 300, 200);
            lv_obj_align(detailView, LV_ALIGN_TOP_MID, 0, 35);
            lv_obj_set_style_bg_color(detailView, lv_color_white(), 0);
            lv_obj_set_style_border_width(detailView, 1, 0);
            lv_obj_set_style_border_color(detailView, lv_color_hex(0xDDDDDD), 0);
            lv_obj_set_style_radius(detailView, 8, 0);
            lv_obj_set_style_pad_all(detailView, 12, 0);
            lv_obj_clear_flag(detailView, LV_OBJ_FLAG_SCROLLABLE);

            detailTitle = lv_label_create(detailView);
            lv_obj_align(detailTitle, LV_ALIGN_TOP_LEFT, 0, 0);
            lv_obj_set_style_text_font(detailTitle, &lv_font_montserrat_14, 0);
            lv_obj_set_style_text_color(detailTitle, lv_color_hex(0x333333), 0);

            detailContent = lv_label_create(detailView);
            lv_obj_align(detailContent, LV_ALIGN_TOP_LEFT, 0, 25);
            lv_obj_set_style_text_font(detailContent, &lv_font_montserrat_14, 0);
            lv_obj_set_style_text_color(detailContent, lv_color_hex(0x666666), 0);
            lv_obj_set_size(detailContent, 270, 150);
        }

        // Show and update detailed view
        lv_obj_clear_flag(detailView, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text_fmt(detailTitle, "%s %s", memos[selectedMemo].icon, memos[selectedMemo].title);
        lv_label_set_text(detailContent, memos[selectedMemo].content);

    } else {
        // Show grid, hide detailed view
        lv_obj_clear_flag(memoGrid, LV_OBJ_FLAG_HIDDEN);

        // Hide detailed view if it exists
        if (detailView) {
            lv_obj_add_flag(detailView, LV_OBJ_FLAG_HIDDEN);
        }

        // Update grid display
        for (int i = 0; i < memoCount; i++) {
            // Update content preview (first line only)
            char preview[30];
            const char* content = memos[i].content;
            int j = 0;
            while (content[j] && content[j] != '\n' && j < 25) {
                preview[j] = content[j];
                j++;
            }
            if (j == 25) {
                preview[j++] = '.';
                preview[j++] = '.';
                preview[j++] = '.';
            }
            preview[j] = '\0';
            lv_label_set_text(memoContents[i], preview);

            // Highlight selected memo
            if (i == selectedMemo) {
                lv_obj_set_style_border_width(memoCards[i], 3, 0);
                lv_obj_set_style_border_color(memoCards[i], lv_color_hex(0xFF0066), 0);
            } else {
                lv_obj_set_style_border_width(memoCards[i], 1, 0);
                lv_obj_set_style_border_color(memoCards[i], lv_color_hex(0xDDDDDD), 0);
            }
        }
    }

    Serial.printf("Memo display updated: selected=%d, viewMode=%s\n", selectedMemo, viewMode ? "true" : "false");
}
