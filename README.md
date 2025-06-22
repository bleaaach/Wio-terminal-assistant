# Wio Terminal AI Assistant 🤖

A comprehensive smart assistant application for the Seeed Studio Wio Terminal, featuring multiple functional pages including calendar, music control, alarms, memos, timers, weather information, and an AI assistant with voice interaction capabilities.

## Project Overview 📋

This project demonstrates the power of the Wio Terminal as a versatile IoT device by creating a multi-functional smart assistant interface. Built with LVGL (Light and Versatile Graphics Library), the application provides an intuitive user experience with smooth page transitions and responsive controls.

### Key Features ✨

- **Multi-Page Navigation**: Seven distinct functional pages with smooth transitions
- **AI Assistant**: Voice-activated assistant with command processing
- **Calendar Management**: Date tracking and event scheduling
- **Music Control**: Audio playback control interface
- **Smart Alarms**: Configurable alarm system with multiple settings
- **Digital Memos**: Note-taking and reminder functionality
- **Timer System**: Countdown timers for various activities
- **Weather Information**: Real-time weather data display
- **Intuitive Controls**: Joystick and button-based navigation

## Hardware Requirements 🛠️

### Essential Components

- **Seeed Studio Wio Terminal** - Main development board
- **USB-C Cable** - For programming and power
- **MicroSD Card** (Optional) - For data storage and configuration

### Wio Terminal Specifications

- **MCU**: ATSAMD51P19 ARM Cortex-M4F @ 120MHz
- **Display**: 2.4" LCD (320×240) with capacitive touch
- **Connectivity**: Wi-Fi, Bluetooth, LoRaWAN ready
- **Storage**: 512KB Flash, 192KB RAM
- **I/O**: Grove connectors, GPIO pins
- **Controls**: 5-way joystick, 3 user buttons (A, B, C)

## Software Prerequisites 💻

### Development Environment

1. **Visual Studio Code** with PlatformIO extension

   - Download from [code.visualstudio.com](https://code.visualstudio.com/)
   - Install PlatformIO IDE extension
2. **Git** (for version control)

   - Download from [git-scm.com](https://git-scm.com/)

### Required Libraries

All dependencies are automatically managed by PlatformIO:

- LVGL 8.4.0 (Graphics library)
- Seeed Arduino LCD (Display driver)
- Arduino JSON (Data parsing)
- PubSubClient (MQTT communication)
- Seeed Arduino RTC (Real-time clock)

## Getting Started 🚀

### 1. Clone the Repository

```bash
git clone https://github.com/your-username/wio-terminal-assistant.git
cd wio-terminal-assistant
```

### 2. Open in Visual Studio Code

- Launch VS Code
- Open the project folder
- PlatformIO should automatically detect the project

### 3. Configure LVGL

The project includes a pre-configured `lv_conf.h` file optimized for Wio Terminal:

- 16-bit color depth for optimal performance
- Touch input support enabled
- Memory allocation optimized for SAMD51

### 4. Build and Upload

1. Connect your Wio Terminal via USB-C
2. In PlatformIO toolbar:
   - Click **Build** (✓) to compile
   - Click **Upload** (→) to flash the device
3. Open **Serial Monitor** to view debug output

## Application Architecture 🏗️

### Core Components

#### AppManager

Central application controller managing:

- Page navigation and transitions
- Input handling (joystick, buttons)
- Status bar updates (time, battery)
- Global state management

#### PageManager

Handles page lifecycle and navigation:

- Page registration and discovery
- Stack-based navigation system
- Animation management
- Memory optimization

#### Individual Pages

Each functional area is implemented as a separate page:

1. **CalendarPage** - Date management and event scheduling
2. **MusicPage** - Audio control interface
3. **AlarmPage** - Alarm configuration and management
4. **MemoPage** - Note-taking functionality
5. **TimerPage** - Countdown timer system
6. **WeatherPage** - Weather information display
7. **AIAssistantPage** - Voice assistant with command processing

### User Interface Design

#### Navigation System

- **Horizontal Navigation**: Left/Right joystick movement
- **Page Selection**: Joystick press to enter page
- **Quick Actions**: Physical buttons A, B, C for context-specific functions
- **Status Bar**: Always-visible time and system status

#### Visual Theme

- **Modern Dark Theme**: Easy on the eyes with high contrast
- **Consistent Icons**: Intuitive symbols for each function
- **Smooth Animations**: Page transitions and UI feedback
- **Responsive Layout**: Optimized for 320×240 display

## Detailed Feature Guide 📖

### AI Assistant Page 🎤

The AI Assistant provides intelligent voice interaction capabilities with microphone-based voice activity detection:

#### Voice Interaction
- **Manual Activation**: Press Button A or joystick press to start/stop listening
- **Voice Activity Detection**: Built-in microphone monitoring with VAD (Voice Activity Detection)
- **Real-time Processing**: Sound level visualization during listening state
- **Response Generation**: Text-based responses with simulated AI processing

#### Quick Commands
- **Weather Query**: Left joystick (simulates Button B) for instant weather information
- **Time Query**: Right joystick (simulates Button C) for current time display
- **Smart Commands**: Context-aware command processing and execution

#### Supported Commands
```
"weather"     → "Today: Sunny, 25°C. Perfect weather for outdoor activities!"
"time"        → Current system time display
"timer"       → Timer-related responses
"alarm"       → Alarm-related responses
```

#### Technical Features
- Microphone input processing (WIO_MIC pin)
- Sound level mapping and threshold detection
- State machine with IDLE → LISTENING → PROCESSING → SPEAKING states
- Offline mode simulation with demo responses

### Calendar Management 📅

Real-time calendar with RTC (Real-Time Clock) integration:

#### Features
- **Monthly Calendar View**: Full month display with proper date layout
- **RTC Integration**: Uses SAMD51 built-in RTC for accurate timekeeping
- **Current Date Highlighting**: Today's date highlighted in blue
- **Time Display**: Real-time clock display in HH:MM format
- **Navigation**: Month/year navigation with joystick controls

#### Controls
- **Left/Right Joystick**: Navigate between months
- **Up/Down Joystick**: Navigate between years
- **Button A**: Refresh current time from RTC
- **Auto-update**: Time display updates every minute automatically

#### Technical Implementation
- Automatic RTC initialization with compile-time fallback
- Leap year calculation for accurate date display
- Persistent time tracking across page switches

### Music Control 🎵

Comprehensive music player with SD card support and Grove speaker integration:

#### SD Card Integration
- **Automatic Detection**: Scans for WAV audio files on SD card
- **File Management**: Supports /Music directory or root directory scanning
- **Format Support**: WAV format audio files (uncompressed)
- **Demo Mode**: Falls back to demo tracks if no SD card detected

#### Playback Features
- **Track Navigation**: Left/Right joystick to switch between tracks
- **Time Control**: Up/Down joystick for fast forward/rewind (±10 seconds)
- **Play/Pause**: Button A toggles playback state
- **Progress Display**: Visual progress bar and time counter
- **Album Art Simulation**: Dynamic icons and colors for each track

#### Grove Speaker Output
- **Hardware Integration**: Uses Grove speaker connected to designated pin
- **Tone Generation**: Plays melodies and notification sounds
- **Demo Melodies**: Different musical patterns for each demo track
- **Audio Feedback**: Start/stop tones and track preview sounds

### Smart Alarms ⏰

Multi-alarm system with configurable settings:

#### Alarm Management
- **Multiple Alarms**: Support for up to 5 different alarms
- **Individual Control**: Each alarm can be enabled/disabled independently
- **Time Configuration**: Hour and minute setting for each alarm
- **Custom Names**: Personalized alarm labels
- **Weekly Schedule**: Day-of-week repeat patterns

#### Pre-configured Alarms
```
Alarm 1: 07:00 - "Morning" (Weekdays)
Alarm 2: 08:30 - "Work" (Monday-Friday)
Alarm 3: 12:00 - "Lunch" (Daily)
Alarm 4: 18:00 - "Dinner" (Daily)
Alarm 5: 22:00 - "Sleep" (Daily)
```

#### Controls
- **Joystick Navigation**: Select between different alarms
- **Button A**: Toggle selected alarm on/off
- **Edit Mode**: Modify alarm times and settings
- **Visual Indicators**: Clear on/off status for each alarm

### Timer System ⏲️

Countdown timer with circular progress visualization:

#### Timer Features
- **Circular Progress**: Visual arc showing remaining time
- **Default Duration**: 5-minute default timer
- **Time Adjustment**: Add/subtract minutes before starting
- **Audio Alert**: Buzzer notification when timer expires
- **Pause/Resume**: Start/stop functionality

#### Controls
- **Button A (or main button)**: Start/Stop timer
- **Button B**: Add 1 minute (when stopped)
- **Button C**: Subtract 1 minute (when stopped)
- **Time Range**: 1-120 minutes adjustable
- **Visual Feedback**: Color-coded status (Ready/Running/Finished)

#### Technical Implementation
- Real-time countdown with 1-second precision
- Circular arc progress indicator (0-300 range)
- Grove speaker integration for completion alert
- State management prevents adjustment during operation

### Weather Information 🌤️

Weather display with simulated data and manual refresh:

#### Weather Display
- **Current Conditions**: Temperature, description, and weather icon
- **Location Info**: City name display
- **Humidity Data**: Relative humidity percentage
- **Visual Icons**: Weather-appropriate symbols
- **Auto-refresh**: Periodic updates with loading indicators

#### Simulated Weather Data
```
Temperature: 15-30°C range
Conditions: Sunny, Cloudy, Rainy, Snowy
Cities: Beijing, Shanghai, Guangzhou, Shenzhen
Humidity: 40-80% range
```

#### Controls
- **Button A**: Manual weather refresh
- **Up Joystick**: Force refresh weather data
- **Auto-update**: Background updates every few minutes
- **Loading States**: Visual feedback during data updates

### Memo Management 📝

Digital note-taking system with categorized memos:

#### Pre-loaded Memos
1. **Today's Tasks** - Daily task checklist
2. **Shopping List** - Grocery and shopping items
3. **Travel Plan** - Trip planning and itinerary
4. **Meeting Notes** - Work meeting summaries
5. **Ideas** - Creative ideas and brainstorming
6. **Reminders** - Important reminders and to-dos

#### Navigation System
- **Grid View**: Overview of all memos with icons
- **Detail View**: Full memo content display
- **Linear Navigation**: Left/Right or Up/Down joystick movement
- **View Toggle**: Button A switches between grid and detail views

#### Visual Design
- **Color-coded Cards**: Each memo has unique background color
- **Category Icons**: Visual symbols for different memo types
- **Smooth Transitions**: Animated switching between memos
- **Readable Layout**: Optimized text display for 320×240 screen

## Code Structure 📁

```
src/
├── main.cpp                 # Application entry point
├── core/
│   ├── AppManager.cpp       # Main application controller
│   ├── PageManager.cpp      # Page navigation system
│   └── PageBase.h           # Base class for all pages
├── pages/
│   ├── CalendarPage.cpp     # Calendar functionality
│   ├── MusicPage.cpp        # Music control
│   ├── AlarmPage.cpp        # Alarm management
│   ├── MemoPage.cpp         # Note-taking
│   ├── TimerPage.cpp        # Timer system
│   ├── WeatherPage.cpp      # Weather display
│   └── AIAssistantPage.cpp  # AI assistant
├── theme/
│   └── style_manager.cpp    # UI styling and themes
└── utils/
    └── helpers.cpp          # Utility functions
```

## Customization Guide 🎨

### Adding New Pages

1. **Create Page Class**:

```cpp
class MyCustomPage : public PageBase {
public:
    void onViewLoad() override;
    void onViewDidAppear() override;
    void onKey(lv_dir_t direction) override;
    void onButton(bool pressed) override;
};
```

2. **Register in AppManager**:

```cpp
myCustomPage = new MyCustomPage();
pageManager.Register(myCustomPage, "MyCustom");
```

3. **Add Navigation**:
   Update navigation arrays in AppManager.cpp

### Modifying UI Theme

Edit `src/theme/style_manager.cpp`:

```cpp
// Color definitions
#define COLOR_PRIMARY     lv_color_hex(0x2196F3)
#define COLOR_SECONDARY   lv_color_hex(0xFF9800)
#define COLOR_BACKGROUND  lv_color_hex(0x121212)
```

### Configuring AI Commands

Extend command processing in `AIAssistantPage.cpp`:

```cpp
String processCommand(const String& command) {
    if (command.indexOf("my_command") >= 0) {
        return "Custom response";
    }
    // ... existing commands
}
```

## Troubleshooting 🔧

### Common Issues

#### Display Problems

- **Blank Screen**: Check LVGL configuration and display initialization
- **Corrupted Graphics**: Verify color depth settings (16-bit)
- **Slow Rendering**: Optimize buffer size in main.cpp

#### Input Issues

- **Unresponsive Joystick**: Check pin definitions and pull-up resistors
- **Button Debouncing**: Adjust `INPUT_DEBOUNCE` constant
- **Navigation Problems**: Verify page registration in AppManager

#### Memory Issues

- **Crashes/Resets**: Monitor heap usage, optimize LVGL buffers
- **Slow Performance**: Reduce animation complexity
- **Storage Problems**: Check SD card formatting and file system

### Debug Tips

1. **Enable Serial Output**:

```cpp
Serial.begin(115200);
Serial.println("Debug message");
```

2. **Monitor Memory Usage**:

```cpp
Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
```

3. **LVGL Debug Mode**:
   Enable in `lv_conf.h`:

```c
#define LV_USE_LOG 1
#define LV_LOG_LEVEL LV_LOG_LEVEL_INFO
```

## Performance Optimization ⚡

### Memory Management

- Use object pools for frequently created/destroyed objects
- Implement lazy loading for page content
- Optimize image assets (use indexed colors)
- Regular garbage collection in page transitions

### Display Optimization

- Use partial screen updates when possible
- Optimize animation frame rates
- Implement efficient dirty region tracking
- Use hardware acceleration features

### Power Management

- Implement sleep modes during inactivity
- Optimize display brightness based on ambient light
- Use efficient polling intervals for sensors
- Implement wake-on-interrupt for user input

## Extension Ideas 🚀

### Hardware Additions

- **Grove Sensors**: Temperature, humidity, light sensors
- **Audio Module**: Speaker for voice feedback
- **Camera Module**: Image capture and processing
- **LoRaWAN Module**: Long-range communication

### Software Enhancements

- **Cloud Integration**: Sync data with cloud services
- **Voice Recognition**: Local speech processing
- **Machine Learning**: On-device AI inference
- **IoT Dashboard**: Home automation control

### Community Features

- **Plugin System**: Third-party page development
- **Theme Store**: Downloadable UI themes
- **Command Sharing**: Community AI command library
- **Project Gallery**: User-created applications

## Contributing 🤝

We welcome contributions from the community! Here's how you can help:

### Development Process

1. Fork the repository
2. Create a feature branch: `git checkout -b feature/amazing-feature`
3. Make your changes with clear commit messages
4. Add tests for new functionality
5. Submit a pull request with detailed description

### Coding Standards

- Follow existing code style and formatting
- Add comments for complex logic
- Use meaningful variable and function names
- Include error handling and validation
- Write unit tests for new features

### Documentation

- Update README for new features
- Add inline code documentation
- Create tutorials for complex features
- Maintain changelog for releases

## License 📄

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments 🙏

- **Seeed Studio** for the amazing Wio Terminal hardware
- **LVGL Team** for the excellent graphics library
- **PlatformIO** for the development environment
- **Arduino Community** for extensive library support
- **Contributors** who help improve this project

## Support & Community 💬

### Getting Help

- **GitHub Issues**: Report bugs and request features
- **Seeed Forum**: Community discussions and support
- **Discord**: Real-time chat with developers
- **Wiki**: Comprehensive documentation and tutorials

### Stay Connected

- **GitHub**: Star and watch for updates
- **Twitter**: Follow [@SeeedStudio](https://twitter.com/seeedstudio)
- **YouTube**: Video tutorials and demos
- **Blog**: Latest news and project showcases

---

**Happy coding with your Wio Terminal AI Assistant!** 🎉

*Built with ❤️ by the Seeed Studio community*
