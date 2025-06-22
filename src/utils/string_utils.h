/*
 * @Author: Bleaach008
 * @Date: 2025-06-21 12:16:07
 * @LastEditTime: 2025-06-21 12:32:45
 * @FilePath: \wiki-documents\wio_terminal_lvgl_starter_code-main\src\utils\string_utils.h
 * @Description: 
 * 
 * Copyright (c) 2025 by 008, All Rights Reserved. 
 */
#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

// Replacement for strdup
static inline char* custom_strdup(const char* str) {
    if (str == NULL) return NULL;
    size_t len = strlen(str) + 1;
    char* new_str = (char*)malloc(len);
    if (new_str) {
        memcpy(new_str, str, len);
    }
    return new_str;
}

// Replacement for strndup
static inline char* custom_strndup(const char* str, size_t n) {
    if (str == NULL) return NULL;
    size_t len = strlen(str);
    if (len > n) len = n;
    char* new_str = (char*)malloc(len + 1);
    if (new_str) {
        memcpy(new_str, str, len);
        new_str[len] = '\0';
    }
    return new_str;
}

#ifdef __cplusplus
}
#endif

#define strdup custom_strdup
#define strndup custom_strndup

#endif // STRING_UTILS_H 