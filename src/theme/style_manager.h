#pragma once

#include "lvgl.h"

// Color definitions for traditional UI style
#define COLOR_BACKGROUND    lv_color_hex(0xF5F5F5)  // Light gray background
#define COLOR_PRIMARY       lv_color_hex(0x2196F3)  // Blue primary
#define COLOR_SECONDARY     lv_color_hex(0xFF9800)  // Orange secondary
#define COLOR_SUCCESS       lv_color_hex(0x4CAF50)  // Green success
#define COLOR_WARNING       lv_color_hex(0xFF9800)  // Orange warning
#define COLOR_ERROR         lv_color_hex(0xF44336)  // Red error
#define COLOR_TEXT_PRIMARY  lv_color_hex(0x333333)  // Dark gray text
#define COLOR_TEXT_SECONDARY lv_color_hex(0x666666) // Medium gray text
#define COLOR_BORDER        lv_color_hex(0xDDDDDD)  // Light border

class StyleManager {
public:
    static void init();
    static void applyCardStyle(lv_obj_t* obj);
    static void applyButtonStyle(lv_obj_t* obj);
    static void applyTextStyle(lv_obj_t* obj);
    
private:
    static bool initialized;
};
