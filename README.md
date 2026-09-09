# HT16K33 Digital Clock - Multi-Timezone Display

A versatile digital clock project that displays the current time in multiple time zones using the HT16K33 7-segment LED backpack. Supports both Arduino and Raspberry Pi/Python implementations.

## Features

✅ **Real-time clock display** - Shows current time in HH:MM format  
✅ **Multiple timezone support** - Display times for different time zones simultaneously  
✅ **HT16K33 LED backpack** - 7-segment display with brightness control  
✅ **Dual implementation** - Arduino and Python versions  
✅ **Easy configuration** - Simple timezone and display settings  
✅ **Automatic brightness** - Optional ambient light adjustment  
✅ **Temperature-compensated** - Optional NTP sync for accuracy  

## Hardware Requirements

### Minimum Setup
- **Microcontroller**: Arduino (Uno, Nano, Mega) or Raspberry Pi
- **Display Module**: HT16K33 7-segment LED backpack (4-digit display)
- **Display**: 0.56" common cathode 7-segment LED module
- **Connections**:
  - SDA (I2C Data) - GPIO 20 (Pi) / A4 (Arduino)
  - SCL (I2C Clock) - GPIO 21 (Pi) / A5 (Arduino)
  - VCC - 5V or 3.3V
  - GND - Ground

### Optional Components
- Real-Time Clock (DS3231) for offline timekeeping
- NTP internet connection for automatic time sync
- Light sensor (LDR) for automatic brightness adjustment

## Project Structure

```
ht16k33-digital-clock/
├── README.md                    # This file
├── WIRING_DIAGRAM.md           # Circuit connection guide
├── arduino/
│   ├── MultiTimezone_Clock.ino # Main Arduino sketch
│   ├── config.h                # Configuration header
│   └── libraries_needed.txt     # Required Arduino libraries
├── python/
│   ├── digital_clock.py         # Main Python script
│   ├── config.py               # Configuration file
│   └── requirements.txt         # Python dependencies
├── configs/
│   └── timezones.json          # Timezone definitions
└── docs/
    └── troubleshooting.md       # Common issues and solutions
```

## Quick Start

### Arduino Setup

1. **Install Libraries** (Arduino IDE):
   - Adafruit_LEDBackpack
   - Adafruit_GFX
   - RTClib (optional, for DS3231)
   - Time (optional, for timezone)

2. **Upload Sketch**:
   ```bash
   1. Open MultiTimezone_Clock.ino in Arduino IDE
   2. Select your board and port
   3. Upload to your Arduino
   ```

3. **Configure Timezones** (in `config.h`):
   ```cpp
   const int PRIMARY_TIMEZONE = -5;    // EST
   const int SECONDARY_TIMEZONE = 0;   // GMT
   ```

### Raspberry Pi / Python Setup

1. **Install Dependencies**:
   ```bash
   pip install -r python/requirements.txt
   ```

2. **Enable I2C**:
   ```bash
   sudo raspi-config
   # Interface Options > I2C > Enable
   ```

3. **Run the Script**:
   ```bash
   python3 python/digital_clock.py
   ```

## Usage

### Arduino Example
```cpp
// Set time manually
clock.setTime(14, 30);  // 2:30 PM

// Display timezone 1
clock.displayTimezone(PRIMARY_TIMEZONE);

// Adjust brightness (0-15)
clock.setBrightness(10);
```

### Python Example
```python
from digital_clock import DigitalClock

clock = DigitalClock(i2c_address=0x70)
clock.set_time_from_ntp()  # Auto sync from internet
clock.display_timezone("America/New_York")
clock.set_brightness(10)
```

## Configuration

Edit `config.py` (Python) or `config.h` (Arduino) to customize:

```python
# Timezone offsets (hours from UTC)
TIMEZONES = {
    'EST': -5,
    'CST': -6,
    'MST': -7,
    'PST': -8,
    'GMT': 0,
    'IST': 5.5,
    'JST': 9,
}

# Display settings
BRIGHTNESS = 10  # 0-15 (0=min, 15=max)
AUTO_BRIGHTNESS = False
DISPLAY_24_HOUR = False  # True for 24-hour format

# Time sync
USE_NTP = True  # Automatic internet time sync
NTP_SERVER = 'pool.ntp.org'
SYNC_INTERVAL = 3600  # seconds
```

## Display Modes

The clock supports multiple display modes:

1. **Single Timezone** - Shows time for one timezone only
2. **Dual Timezone** - Alternates between two timezones (press button to switch)
3. **Timezone Carousel** - Cycles through multiple timezones every 5 seconds

Edit mode in configuration files.

## I2C Addressing

The HT16K33 backpack supports 8 different I2C addresses (0x70-0x77).

**Set address via solder jumpers on the backpack:**

| A2 | A1 | A0 | Address |
|----|----|----|---------|
| 0  | 0  | 0  | 0x70    |
| 0  | 0  | 1  | 0x71    |
| 0  | 1  | 0  | 0x72    |
| 0  | 1  | 1  | 0x73    |
| 1  | 0  | 0  | 0x74    |
| 1  | 0  | 1  | 0x75    |
| 1  | 1  | 0  | 0x76    |
| 1  | 1  | 1  | 0x77    |

To use multiple displays, set different addresses and update your code accordingly.

## Troubleshooting

See [troubleshooting.md](docs/troubleshooting.md) for common issues:
- Display not showing
- I2C communication errors
- Time not updating
- Brightness issues

## Libraries Used

**Arduino:**
- [Adafruit_LEDBackpack](https://github.com/adafruit/Adafruit_LED_Backpack)
- [Adafruit_GFX](https://github.com/adafruit/Adafruit-GFX-Library)
- [RTClib](https://github.com/adafruit/RTClib) (optional)

**Python:**
- [smbus2](https://pypi.org/project/smbus2/) - I2C communication
- [pytz](https://pypi.org/project/pytz/) - Timezone support
- [ntplib](https://pypi.org/project/ntplib/) - NTP time sync

## Wiring Guide

See [WIRING_DIAGRAM.md](WIRING_DIAGRAM.md) for detailed circuit diagrams and pin connections.

## Power Consumption

- Typical: ~100mA @ 5V
- Standby: ~50mA
- Max brightness: ~150mA

## License

MIT License - See LICENSE file for details

## Contributing

Contributions welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Submit a pull request

## Support

- 📖 [Adafruit HT16K33 Guide](https://learn.adafruit.com/adafruit-led-backpack/)
- 📊 [HT16K33 Datasheet](https://www.alldatasheet.com/datasheet-pdf/pdf/439619/HOLTEK/HT16K33.html)
- 💬 Open an issue for bugs or questions

## Version History

**v1.0.0** - Initial release
- Arduino and Python implementations
- Multi-timezone support
- Brightness control
- NTP synchronization

---

**Last Updated**: 2026-09-09  
**Author**: Digital Clock Project Contributors
