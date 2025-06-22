#include "style_manager.h"

bool StyleManager::initialized = false;

void StyleManager::init() {
    if (initialized) return;
    
    // Initialize global styles here if needed
    // For now, we'll use inline styles in each component
    
    initialized = true;
}

void StyleManager::applyCardStyle(lv_obj_t* obj) {
    if (!obj) return;
    
    lv_obj_set_style_bg_color(obj, COLOR_BACKGROUND, 0);
    lv_obj_set_style_border_width(obj, 1, 0);
    lv_obj_set_style_border_color(obj, COLOR_BORDER, 0);
    lv_obj_set_style_radius(obj, 8, 0);
    lv_obj_set_style_pad_all(obj, 8, 0);
}

void StyleManager::applyButtonStyle(lv_obj_t* obj) {
    if (!obj) return;
    
    lv_obj_set_style_bg_color(obj, COLOR_PRIMARY, 0);
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_radius(obj, 6, 0);
    lv_obj_set_style_text_color(obj, lv_color_white(), 0);
}

void StyleManager::applyTextStyle(lv_obj_t* obj) {
    if (!obj) return;
    
    lv_obj_set_style_text_color(obj, COLOR_TEXT_PRIMARY, 0);
    lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, 0);
}
