#ifndef WEATHER_PAGE_H
#define WEATHER_PAGE_H

#include "../core/PageBase.h"
#include <Arduino.h>

// Weather data structure
struct WeatherData {
    String city;
    String description;
    int temperature;
    int humidity;
    String updateTime;
    bool isValid;
};

// Weather icons mapping
enum WeatherIcon {
    WEATHER_SUNNY = 0,
    WEATHER_CLOUDY,
    WEATHER_RAINY,
    WEATHER_SNOWY,
    WEATHER_THUNDERSTORM,
    WEATHER_FOGGY,
    WEATHER_UNKNOWN
};

class WeatherPage : public PageBase {
public:
    WeatherPage();
    virtual ~WeatherPage();

    // PageBase interface
    virtual void onViewLoad() override;
    virtual void onViewWillAppear() override;
    virtual void onViewDidAppear() override;
    virtual void onViewWillDisappear() override;
    virtual void onViewDidDisappear() override;

    // Button handlers (using PageBase interface)
    virtual void onButton(bool pressed) override;
    virtual void onKey(lv_dir_t direction) override;

    // Update method
    void update();

    // Weather functions
    void updateWeatherData();
    void refreshWeather();
    bool fetchWeatherFromAPI();
    void displayWeatherInfo();
    void displayDefaultWeatherInfo();
    void showLoadingIndicator(bool show);

private:
    // UI elements
    lv_obj_t* titleLabel;
    lv_obj_t* weatherContainer;
    lv_obj_t* weatherIcon;
    lv_obj_t* temperatureLabel;
    lv_obj_t* descriptionLabel;
    lv_obj_t* cityLabel;
    lv_obj_t* humidityLabel;
    lv_obj_t* loadingSpinner;

    // Weather data
    WeatherData currentWeather;
    unsigned long lastUpdateTime;
    bool isUpdating;
    
    // Update intervals
    static const unsigned long UPDATE_INTERVAL = 30 * 60 * 1000; // 30 minutes
    static const unsigned long RETRY_INTERVAL = 5 * 60 * 1000;   // 5 minutes on error

    // Helper functions
    void createWeatherUI();
    void updateWeatherDisplay();
    WeatherIcon getWeatherIcon(const String& description);
    const char* getWeatherIconSymbol(WeatherIcon icon);
    String formatTemperature(int temp);
    String formatTime(unsigned long timestamp);
    void animateWeatherIcon();
    String translateWeatherDescription(const String& chineseDesc);
    String translateCityName(const String& chineseName);
    
    // Network functions
    bool connectToWiFi();
    String makeHTTPRequest(const String& url);
    bool parseWeatherJSON(const String& json);
    
    // Configuration
    String weatherAPIKey;
    String weatherCity;
    String weatherLanguage;
    String weatherAPIURL;
    
    void loadWeatherConfig();
    void saveWeatherConfig();
};

#endif // WEATHER_PAGE_H
