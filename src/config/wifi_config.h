/*
 * @Author: Bleaach008
 * @Date: 2025-06-22 15:08:06
 * @LastEditTime: 2025-06-22 20:03:29
 * @FilePath: \wio_terminal_lvgl_starter_code-main\src\config\wifi_config.h
 * @Description: 
 * 
 * Copyright (c) 2025 by 008, All Rights Reserved. 
 */
#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

// 🔧 WiFi 配置 - 在这里设置你的 WiFi 信息
// WiFi Configuration - Set your WiFi credentials here

// 🌐 WiFi网络配置 - 统一配置方式
// WiFi Network Configuration - Unified Configuration
#define WIFI_SSID     "Redmi K60 Ultra"        // 🔑 替换为您的WiFi名称
#define WIFI_PASSWORD "200124.."               // 🔑 替换为您的WiFi密码

// 🌤️ 天气API配置 - 心知天气
// Weather API Configuration - Seniverse
#define WEATHER_API_KEY  "SQxtNQ38dZRxejdi5"  // 🔑 您的心知天气API密钥
#define WEATHER_CITY     "shenzhen"           // 🏙️ 您的城市名称（建议使用拼音小写）
#define WEATHER_LANGUAGE "zh-Hans"            // 🌍 语言设置（zh-Hans中文简体，en英文）

// 📝 心知天气API密钥获取步骤:
// 1. 访问 https://seniverse.com
// 2. 注册免费账户（免费额度：1000次/天）
// 3. 创建应用获取API密钥
// 4. 复制API密钥到上面的WEATHER_API_KEY
//
// 💡 心知天气优势：
// - 中国地区数据准确
// - 中文天气描述
// - 免费额度充足

// 其他网络配置
// Other network configurations
#define WIFI_CONNECT_TIMEOUT    15000   // WiFi 连接超时时间（毫秒）
#define WIFI_RETRY_INTERVAL     30000   // WiFi 重连间隔（毫秒）
#define WEATHER_UPDATE_INTERVAL 1800000 // 天气更新间隔（30分钟）

// 调试配置
// Debug configuration
#define WIFI_DEBUG_ENABLED      true    // 是否启用 WiFi 调试信息
#define WEATHER_DEBUG_ENABLED   true    // 是否启用天气调试信息

#endif // WIFI_CONFIG_H
