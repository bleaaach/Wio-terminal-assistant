#include "WeatherPage.h"
#include "../config/wifi_config.h"
#include "../utils/WiFiManager.h"

#ifndef SIMULATOR_BUILD
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#else
// Simulator build - use mock implementations
#include <iostream>
#include <string>
#include <ctime>
#endif

WeatherPage::WeatherPage() {
    titleLabel = nullptr;
    weatherContainer = nullptr;
    weatherIcon = nullptr;
    temperatureLabel = nullptr;
    descriptionLabel = nullptr;
    cityLabel = nullptr;
    humidityLabel = nullptr;
    loadingSpinner = nullptr;
    
    // Initialize weather data
    currentWeather.isValid = false;
    lastUpdateTime = 0;
    isUpdating = false;
    
    // Load configuration
    loadWeatherConfig();
}

WeatherPage::~WeatherPage() {
    // Cleanup handled by LVGL
}

void WeatherPage::onViewLoad() {
    Serial.println("WeatherPage: onViewLoad");

    // Traditional style: light blue background
    lv_obj_set_style_bg_color(_root, lv_color_hex(0xF0F8FF), 0); // Light blue
    lv_obj_set_style_bg_opa(_root, LV_OPA_COVER, 0);
}

void WeatherPage::onViewWillAppear() {
    Serial.println("WeatherPage: onViewWillAppear");

    // 只在UI未创建时创建，避免重复创建
    if (!titleLabel) {
        createWeatherUI();
        // 显示默认数据，避免阻塞页面切换
        displayDefaultWeatherInfo();
    }

    // 延迟更新天气数据，避免阻塞菜单切换
    Serial.println("Weather: Scheduling weather update...");
    lastUpdateTime = millis() - UPDATE_INTERVAL + 2000; // 2秒后开始更新
}

void WeatherPage::onViewDidAppear() {
    Serial.println("WeatherPage: onViewDidAppear");
}

void WeatherPage::onViewWillDisappear() {
    Serial.println("WeatherPage: onViewWillDisappear");
}

void WeatherPage::onViewDidDisappear() {
    Serial.println("WeatherPage: onViewDidDisappear");
}

void WeatherPage::update() {
    // Check for automatic updates or initial update
    unsigned long currentTime = millis();
    if (!isUpdating &&
        (!currentWeather.isValid || (currentTime - lastUpdateTime) > UPDATE_INTERVAL)) {
        // 非阻塞更新：只在页面活跃时更新
        updateWeatherData();
    }
}

void WeatherPage::createWeatherUI() {
    // Page title (consistent with other pages)
    titleLabel = lv_label_create(_root);
    lv_label_set_text_fmt(titleLabel, "%s WEATHER", LV_SYMBOL_WIFI);
    lv_obj_align(titleLabel, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_text_font(titleLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(titleLabel, lv_color_hex(0x2196F3), 0); // Blue theme

    // Weather主卡片 (重新设计 - 垂直布局)
    weatherContainer = lv_obj_create(_root);
    lv_obj_set_size(weatherContainer, 200, 100);
    lv_obj_align(weatherContainer, LV_ALIGN_TOP_MID, 0, 40);
    lv_obj_set_style_bg_color(weatherContainer, lv_color_hex(0x2196F3), 0);
    lv_obj_set_style_radius(weatherContainer, 16, 0);
    lv_obj_set_style_border_width(weatherContainer, 0, 0);
    lv_obj_set_style_pad_all(weatherContainer, 10, 0);
    lv_obj_clear_flag(weatherContainer, LV_OBJ_FLAG_SCROLLABLE); // 禁用滚动条

    // Weather icon (顶部中心)
    weatherIcon = lv_label_create(weatherContainer);
    lv_label_set_text(weatherIcon, LV_SYMBOL_WIFI);
    lv_obj_align(weatherIcon, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_set_style_text_font(weatherIcon, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(weatherIcon, lv_color_hex(0xFFFFFF), 0);

    // Temperature display (中心，大字体)
    temperatureLabel = lv_label_create(weatherContainer);
    lv_label_set_text(temperatureLabel, "--°C");
    lv_obj_align(temperatureLabel, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_font(temperatureLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(temperatureLabel, lv_color_hex(0xFFFFFF), 0);

    // Weather description (底部中心)
    descriptionLabel = lv_label_create(weatherContainer);
    lv_label_set_text(descriptionLabel, "Loading...");
    lv_obj_align(descriptionLabel, LV_ALIGN_BOTTOM_MID, 0, -5);
    lv_obj_set_style_text_font(descriptionLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(descriptionLabel, lv_color_hex(0xE3F2FD), 0);

    // City name (卡片下方)
    cityLabel = lv_label_create(_root);
    lv_label_set_text(cityLabel, "Fetching location...");
    lv_obj_align(cityLabel, LV_ALIGN_TOP_MID, 0, 150);
    lv_obj_set_style_text_font(cityLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(cityLabel, lv_color_hex(0x333333), 0);

    // 湿度信息 (简化显示)
    humidityLabel = lv_label_create(_root);
    lv_label_set_text(humidityLabel, "Humidity: --%");
    lv_obj_align(humidityLabel, LV_ALIGN_TOP_MID, 0, 175);
    lv_obj_set_style_text_font(humidityLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(humidityLabel, lv_color_hex(0x666666), 0);

    // 删除instructionLabel，简化界面

    // Loading spinner (initially hidden)
    loadingSpinner = lv_spinner_create(_root, 1000, 60);
    lv_obj_set_size(loadingSpinner, 40, 40);
    lv_obj_align(loadingSpinner, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_flag(loadingSpinner, LV_OBJ_FLAG_HIDDEN);
}

void WeatherPage::onButton(bool pressed) {
    Serial.printf("WeatherPage: onButton called with pressed=%d\n", pressed);
    if (!pressed) return;
    // Default button handler - refresh weather
    Serial.println("Weather: Button pressed - refreshing weather");
    refreshWeather();
}

void WeatherPage::onKey(lv_dir_t direction) {
    Serial.printf("WeatherPage: onKey called with direction=%d\n", direction);
    switch (direction) {
        case LV_DIR_TOP:
            // Up key: Manual refresh
            Serial.println("Weather: Manual refresh requested");
            refreshWeather();
            break;
        case LV_DIR_LEFT:
            // Left key: Settings (placeholder)
            Serial.println("Weather: Settings (not implemented yet)");
            break;
        case LV_DIR_RIGHT:
            // Right key: Toggle auto-update
            Serial.println("Weather: Toggle auto-update (not implemented yet)");
            break;
        case LV_DIR_NONE:
            // Button press
            Serial.println("Weather: Button press via onKey");
            refreshWeather();
            break;
        default:
            Serial.printf("Weather: Unhandled key direction: %d\n", direction);
            break;
    }
}

void WeatherPage::updateWeatherData() {
    if (isUpdating) return;

    Serial.println("Weather: Starting update...");
    isUpdating = true;
    showLoadingIndicator(true);

    // Try to fetch real weather data
    bool success = false;

#ifndef SIMULATOR_BUILD
    // Real hardware - try API first, fallback to mock data
    if (weatherAPIKey != "your_seniverse_api_key_here" && weatherAPIKey.length() > 0) {
        success = fetchWeatherFromAPI();
        if (!success) {
            Serial.println("Weather: API failed, using mock data");
        }
    } else {
        Serial.println("Weather: API密钥未配置，请在wifi_config.h中设置WEATHER_API_KEY，使用模拟数据");
    }
#endif

    // If API failed or simulator, use mock data
    if (!success) {
        currentWeather.city = "Beijing";
        currentWeather.description = "Partly Cloudy";
        currentWeather.temperature = 22;
        currentWeather.humidity = 65;
        currentWeather.updateTime = formatTime(millis());
        currentWeather.isValid = true;
    }

    lastUpdateTime = millis();
    isUpdating = false;
    showLoadingIndicator(false);

    displayWeatherInfo();
    Serial.println("Weather: Update completed");
}

void WeatherPage::displayWeatherInfo() {
    if (!currentWeather.isValid) return;

    // Update weather icon
    WeatherIcon icon = getWeatherIcon(currentWeather.description);
    lv_label_set_text(weatherIcon, getWeatherIconSymbol(icon));

    // Update temperature
    lv_label_set_text_fmt(temperatureLabel, "%d°C", currentWeather.temperature);

    // Update description
    lv_label_set_text(descriptionLabel, currentWeather.description.c_str());

    // Update city
    lv_label_set_text(cityLabel, currentWeather.city.c_str());

    // Update details (简化显示)
    lv_label_set_text_fmt(humidityLabel, "Humidity: %d%%", currentWeather.humidity);
}

void WeatherPage::displayDefaultWeatherInfo() {
    // 显示默认的占位符信息，避免空白页面
    lv_label_set_text(weatherIcon, LV_SYMBOL_WIFI);
    lv_label_set_text(temperatureLabel, "--°C");
    lv_label_set_text(descriptionLabel, "Loading...");
    lv_label_set_text(cityLabel, "Fetching location...");
    lv_label_set_text(humidityLabel, "Humidity: --%");
}

void WeatherPage::refreshWeather() {
    updateWeatherData();
}

void WeatherPage::showLoadingIndicator(bool show) {
    if (show) {
        lv_obj_clear_flag(loadingSpinner, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(loadingSpinner, LV_OBJ_FLAG_HIDDEN);
    }
}

WeatherIcon WeatherPage::getWeatherIcon(const String& description) {
    String desc = description;
    desc.toLowerCase();
    
    if (desc.indexOf("sun") >= 0 || desc.indexOf("clear") >= 0) {
        return WEATHER_SUNNY;
    } else if (desc.indexOf("cloud") >= 0) {
        return WEATHER_CLOUDY;
    } else if (desc.indexOf("rain") >= 0) {
        return WEATHER_RAINY;
    } else if (desc.indexOf("snow") >= 0) {
        return WEATHER_SNOWY;
    } else if (desc.indexOf("thunder") >= 0 || desc.indexOf("storm") >= 0) {
        return WEATHER_THUNDERSTORM;
    } else if (desc.indexOf("fog") >= 0 || desc.indexOf("mist") >= 0) {
        return WEATHER_FOGGY;
    }
    
    return WEATHER_UNKNOWN;
}

const char* WeatherPage::getWeatherIconSymbol(WeatherIcon icon) {
    switch (icon) {
        case WEATHER_SUNNY: return LV_SYMBOL_PLUS;
        case WEATHER_CLOUDY: return LV_SYMBOL_WIFI;
        case WEATHER_RAINY: return LV_SYMBOL_DOWN;
        case WEATHER_SNOWY: return LV_SYMBOL_MINUS;
        case WEATHER_THUNDERSTORM: return LV_SYMBOL_WARNING;
        case WEATHER_FOGGY: return LV_SYMBOL_EYE_CLOSE;
        default: return LV_SYMBOL_AUDIO;
    }
}

String WeatherPage::formatTime(unsigned long timestamp) {
    // Simple time formatting (replace with actual time formatting)
    unsigned long seconds = timestamp / 1000;
    unsigned long minutes = seconds / 60;
    unsigned long hours = minutes / 60;
    
    return String(hours % 24) + ":" + String((minutes % 60) < 10 ? "0" : "") + String(minutes % 60);
}

void WeatherPage::loadWeatherConfig() {
    // 从配置文件读取天气设置
    // Load weather settings from config file

    weatherCity = WEATHER_CITY;
    weatherLanguage = WEATHER_LANGUAGE;
    weatherAPIKey = WEATHER_API_KEY;

    // 心知天气API URL
    weatherAPIURL = "https://api.seniverse.com/v3/weather/now.json";

    Serial.printf("Weather: Config loaded - City: %s, Language: %s\n",
                 weatherCity.c_str(), weatherLanguage.c_str());
    Serial.printf("Weather: API Key length: %d, Key: %s\n",
                 weatherAPIKey.length(),
                 weatherAPIKey.length() > 0 ? weatherAPIKey.c_str() : "EMPTY");
}

void WeatherPage::saveWeatherConfig() {
    // Save weather configuration to storage
    // TODO: Implement configuration saving
}

bool WeatherPage::fetchWeatherFromAPI() {
    Serial.println("Weather: Fetching data from API...");

    if (!connectToWiFi()) {
        Serial.println("Weather: WiFi connection failed");
        return false;
    }

    // 构建心知天气API URL
    // 格式: https://api.seniverse.com/v3/weather/now.json?key=KEY&location=LOCATION&language=LANGUAGE&unit=c
    String url = weatherAPIURL + "?key=" + weatherAPIKey +
                 "&location=" + weatherCity +
                 "&language=" + weatherLanguage +
                 "&unit=c";

    // 分段打印URL避免缓冲区截断
    Serial.println("Weather: API URL构建:");
    Serial.printf("  Base URL: %s\n", weatherAPIURL.c_str());
    Serial.printf("  API Key: %s\n", weatherAPIKey.c_str());
    Serial.printf("  Location: %s\n", weatherCity.c_str());
    Serial.printf("  Language: %s\n", weatherLanguage.c_str());
    Serial.println("Weather: Complete URL:");
    Serial.println(url);  // 使用println避免printf截断

    String response = makeHTTPRequest(url);
    if (response.length() == 0) {
        Serial.println("Weather: HTTP request failed");
        return false;
    }

    return parseWeatherJSON(response);
}

bool WeatherPage::connectToWiFi() {
    // Use WiFiManager for connection
    if (WiFiMgr.isConnected()) {
        return true; // Already connected
    }

    Serial.println("Weather: Checking WiFi connection...");

    // 快速检查：如果WiFiManager未初始化，快速失败避免阻塞
    if (!WiFiMgr.isInitialized()) {
        Serial.println("Weather: WiFiManager not initialized, skipping connection");
        return false;
    }

    // 如果已经有凭据但未连接，尝试快速连接
    if (WiFiMgr.hasCredentials()) {
        Serial.println("Weather: Attempting quick WiFi connection...");
        // 这里不调用begin()避免阻塞，直接尝试连接
        if (WiFiMgr.connectWithSavedCredentials()) {
            Serial.printf("Weather: WiFi connected! IP: %s\n", WiFiMgr.getLocalIP().c_str());
            return true;
        }
    }

    Serial.println("Weather: WiFi not available, using cached/mock data");
    return false;
}

String WeatherPage::makeHTTPRequest(const String& url) {
#ifndef SIMULATOR_BUILD
    HTTPClient http;
    WiFiClientSecure client;  // 使用WiFiClientSecure支持HTTPS

    // Wio Terminal的WiFiClientSecure设置（不验证证书）
    client.setCACert(NULL);  // 不使用CA证书验证

    Serial.println("Weather: Starting HTTPS request...");
    http.begin(client, url);
    http.addHeader("User-Agent", "WioTerminal-Weather/1.0");
    http.setTimeout(10000);  // 10秒超时

    int httpResponseCode = http.GET();
    String response = "";

    if (httpResponseCode > 0) {
        response = http.getString();
        Serial.printf("Weather: HTTPS Response code: %d\n", httpResponseCode);
        Serial.printf("Weather: Response length: %d\n", response.length());

        // 如果是错误响应，打印响应内容用于调试
        if (httpResponseCode != 200) {
            Serial.println("Weather: Error response content:");
            Serial.println(response);
        } else {
            Serial.println("Weather: HTTPS request successful!");
        }
    } else {
        Serial.printf("Weather: HTTPS Error: %d\n", httpResponseCode);
    }

    http.end();
    return response;
#else
    // Simulator - return mock 心知天气 JSON response
    Serial.println("Weather: 模拟心知天气API响应...");
    return R"({
        "results": [{
            "location": {
                "name": "北京"
            },
            "now": {
                "text": "多云",
                "code": "4",
                "temperature": "22"
            }
        }]
    })";
#endif
}

bool WeatherPage::parseWeatherJSON(const String& json) {
    Serial.println("Weather: Parsing JSON response...");

#ifndef SIMULATOR_BUILD
    // Real JSON parsing for hardware
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, json);

    if (error) {
        Serial.printf("Weather: JSON parsing failed: %s\n", error.c_str());
        return false;
    }

    // 解析心知天气API响应数据
    // 心知天气API响应格式: {"results":[{"location":{"name":"深圳"},"now":{"text":"多云","code":"4","temperature":"22"}}]}
    if (doc.containsKey("results") && doc["results"].size() > 0) {
        auto result = doc["results"][0];

        if (result.containsKey("location") && result.containsKey("now")) {
            auto location = result["location"];
            auto now = result["now"];

            // 提取基本天气数据（免费用户可用）
            currentWeather.city = translateCityName(location["name"].as<String>());
            currentWeather.temperature = now["temperature"].as<String>().toInt();
            currentWeather.description = translateWeatherDescription(now["text"].as<String>());

            // 提取湿度数据（如果可用）
            if (now.containsKey("humidity")) {
                currentWeather.humidity = now["humidity"].as<String>().toInt();
            } else {
                currentWeather.humidity = 60;  // 默认湿度
            }

            currentWeather.updateTime = formatTime(millis());
            currentWeather.isValid = true;

            Serial.printf("Weather: 心知天气解析成功 - %s, %d°C, %s\n",
                         currentWeather.city.c_str(),
                         currentWeather.temperature,
                         currentWeather.description.c_str());
            return true;
        }
    }
#else
    // Simulator - parse mock data
    currentWeather.city = "Beijing";
    currentWeather.description = "Partly Cloudy";
    currentWeather.temperature = 22;
    currentWeather.humidity = 65;
    currentWeather.pressure = 1013;
    currentWeather.windDirection = "NW";
    currentWeather.windSpeed = 12;
    currentWeather.updateTime = formatTime(millis());
    currentWeather.isValid = true;

    Serial.println("Weather: Mock data parsed successfully");
    return true;
#endif

    return false;
}

void WeatherPage::animateWeatherIcon() {
    // Simple weather icon animation
    static unsigned long lastAnimTime = 0;
    static bool animPhase = false;

    if (millis() - lastAnimTime > 2000) { // 2 second interval
        lastAnimTime = millis();
        animPhase = !animPhase;

        if (weatherIcon && currentWeather.isValid) {
            // Add subtle animation based on weather type
            WeatherIcon icon = getWeatherIcon(currentWeather.description);
            if (icon == WEATHER_RAINY && animPhase) {
                // Animate rain
                lv_obj_set_style_text_color(weatherIcon, lv_color_hex(0x87CEEB), 0);
            } else if (icon == WEATHER_SUNNY && animPhase) {
                // Animate sun
                lv_obj_set_style_text_color(weatherIcon, lv_color_hex(0xFFD700), 0);
            } else {
                // Default color
                lv_obj_set_style_text_color(weatherIcon, lv_color_hex(0xFFFFFF), 0);
            }
        }
    }
}

// 中文天气描述翻译为英文
String WeatherPage::translateWeatherDescription(const String& chineseDesc) {
    String desc = chineseDesc;

    // 常见天气描述翻译
    if (desc == "晴" || desc == "晴天") return "Sunny";
    if (desc == "多云") return "Cloudy";
    if (desc == "阴" || desc == "阴天") return "Overcast";
    if (desc == "小雨") return "Light Rain";
    if (desc == "中雨") return "Moderate Rain";
    if (desc == "大雨") return "Heavy Rain";
    if (desc == "雷阵雨") return "Thunderstorm";
    if (desc == "雪") return "Snow";
    if (desc == "雾") return "Fog";
    if (desc == "霾") return "Haze";
    if (desc == "沙尘暴") return "Sandstorm";
    if (desc == "台风") return "Typhoon";

    // 如果没有匹配，返回原文（可能已经是英文）
    return desc;
}

// 中文城市名翻译为英文
String WeatherPage::translateCityName(const String& chineseName) {
    String name = chineseName;

    // 常见城市名翻译
    if (name == "北京") return "Beijing";
    if (name == "上海") return "Shanghai";
    if (name == "深圳") return "Shenzhen";
    if (name == "广州") return "Guangzhou";
    if (name == "杭州") return "Hangzhou";
    if (name == "南京") return "Nanjing";
    if (name == "成都") return "Chengdu";
    if (name == "重庆") return "Chongqing";
    if (name == "武汉") return "Wuhan";
    if (name == "西安") return "Xi'an";
    if (name == "天津") return "Tianjin";
    if (name == "苏州") return "Suzhou";
    if (name == "青岛") return "Qingdao";
    if (name == "大连") return "Dalian";
    if (name == "厦门") return "Xiamen";

    // 如果没有匹配，返回原文
    return name;
}
