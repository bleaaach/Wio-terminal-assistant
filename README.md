# Wio Terminal AI Assistant 🤖

A multi-functional smart assistant for Seeed Studio Wio Terminal with AI voice interaction, calendar, music player, alarms, timers, weather, and memo management.

## Features ✨

- **AI Assistant**: Voice interaction with microphone detection and command processing
- **Calendar**: RTC-based calendar with real-time clock display  
- **Music Player**: SD card support with Grove speaker integration
- **Smart Alarms**: 5 configurable alarms with custom schedules
- **Timer System**: Countdown timer with circular progress display
- **Weather Info**: Simulated weather data with manual refresh
- **Digital Memos**: 6 pre-loaded categorized notes
- **Navigation**: Joystick and 3-button control system

## Quick Start 🚀

### Requirements
- **Seeed Studio Wio Terminal**
- **USB-C Cable** 
- **MicroSD Card** (optional, for music files)
- **Visual Studio Code** with PlatformIO extension

### Setup
```bash
git clone https://github.com/your-username/wio-terminal-assistant.git
cd wio-terminal-assistant
```
1. Open in VS Code with PlatformIO
2. Connect Wio Terminal via USB-C  
3. Build and upload the project
4. Open Serial Monitor for debug output

## Usage Guide 📖

### Navigation
- **Left/Right Joystick**: Switch between pages
- **Joystick Press**: Enter selected page
- **Button A**: Primary action (play/pause, start/stop, etc.)
- **Button B/C**: Secondary actions (varies by page)

### Page Functions

#### AI Assistant 🎤
- **Button A**: Start/stop voice listening
- **Left Joystick**: Quick weather query
- **Right Joystick**: Quick time query
- **Microphone**: Real-time sound level detection

#### Calendar 📅
- **Left/Right**: Navigate months
- **Up/Down**: Navigate years  
- **Button A**: Refresh current time
- **Auto-update**: Time updates every minute

#### Music Player 🎵
- **Left/Right**: Previous/next track
- **Up/Down**: Fast forward/rewind (±10s)
- **Button A**: Play/pause toggle
- **SD Card**: Supports WAV files in /Music folder

#### Timer ⏲️
- **Button A**: Start/stop timer
- **Button B**: Add 1 minute (when stopped)
- **Button C**: Subtract 1 minute (when stopped)
- **Range**: 1-120 minutes

#### Alarms ⏰
- **Joystick**: Navigate between 5 alarms
- **Button A**: Toggle alarm on/off
- **Pre-configured**: Morning, Work, Lunch, Dinner, Sleep

#### Weather 🌤️
- **Button A**: Manual refresh
- **Up Joystick**: Force refresh
- **Auto-update**: Background updates

#### Memos 📝
- **All Directions**: Navigate between 6 memos
- **Button A**: Switch between grid/detail view
- **Categories**: Tasks, Shopping, Travel, Meetings, Ideas, Reminders

## Known Issues & TODOs 🚧

### Current Limitations
- **AI Assistant**: Offline simulation only (TODO: real AI service integration)
- **Weather**: Simulated data (TODO: real API integration)  
- **Music**: WAV files only, requires SD card
- **Character Encoding**: Mixed encoding in some source files (TODO: fix UTF-8)
- **Memory**: No active heap monitoring (TODO: implement)

### Bug Reports
- **Timer**: Contains Chinese comments causing display issues
- **Alarms**: Uses blocking delay() for notifications
- **Navigation**: Occasional UI artifacts during rapid switching
- **Input**: Button debouncing may need adjustment

## Development 🛠️

### Code Structure
```
src/
├── main.cpp                 # Application entry point
├── core/                    # App & page management
├── pages/                   # Individual page implementations  
├── theme/                   # UI styling
└── config/                  # Configuration files
```

### Contributing
**Priority Areas:**
- Fix character encoding issues
- Implement real AI service integration
- Add persistent configuration storage
- Improve error handling and memory management

**Process:**
1. Check open issues labeled `good-first-issue`
2. Fork repository and create feature branch
3. Test on actual Wio Terminal hardware
4. Submit PR with detailed description

### Troubleshooting
- **Blank Screen**: Check LVGL configuration
- **Unresponsive Input**: Verify pin definitions and debouncing
- **Memory Issues**: Monitor heap usage, restart device
- **SD Card**: Ensure FAT32 format and proper file structure

## Hardware Extensions 🔧

### Recommended Additions
- **Grove Speaker**: For audio feedback and music playback
- **Grove Sensors**: Temperature, humidity, light monitoring
- **Larger SD Card**: More storage for music and data
- **External Battery**: Portable power solution

### Future Enhancements
- Real AI service integration (OpenAI, local LLM)
- WiFi configuration UI
- Cloud data synchronization  
- Plugin system for third-party pages
- Multi-language support

## License & Support 📄

- **License**: MIT License
- **Hardware**: Seeed Studio Wio Terminal
- **Graphics**: LVGL 8.4.0
- **Community**: GitHub Issues, Seeed Forum

---

**Built with ❤️ for the Seeed Studio community**

*For detailed technical documentation, see the full README in the repository.*
