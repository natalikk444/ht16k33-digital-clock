/*
 * HT16K33 Digital Clock - Arduino Configuration
 * ================================================
 * Edit this file to customize clock behavior
 */

#ifndef CONFIG_H
#define CONFIG_H

// ============================================
// DISPLAY CONFIGURATION
// ============================================

// HT16K33 I2C Address (default: 0x70)
// See WIRING_DIAGRAM.md for address jumper settings
#define HT16K33_ADDRESS 0x70

// Display brightness (0-15, 0=min, 15=max)
#define DISPLAY_BRIGHTNESS 10

// Display mode: 0=single, 1=dual (toggle), 2=carousel
#define DISPLAY_MODE 0

// Use 24-hour format (true) or 12-hour format (false)
#define USE_24_HOUR_FORMAT false

// Auto-enable colon blinking (looks like a real clock)
#define BLINK_COLON true

// ============================================
// TIMEZONE CONFIGURATION
// ============================================

// Primary timezone offset from UTC (in hours)
// Common values:
//   -8 = PST (Pacific Standard Time)
//   -7 = MST (Mountain Standard Time)
//   -6 = CST (Central Standard Time)
//   -5 = EST (Eastern Standard Time)
//    0 = GMT/UTC (Greenwich Mean Time)
//    1 = CET (Central European Time)
//    5 = IST (Indian Standard Time)
//    8 = CST (China Standard Time)
//    9 = JST (Japan Standard Time)
//   +10 = AEST (Australian Eastern Standard Time)
#define PRIMARY_TIMEZONE -5

// Secondary timezone offset (for dual display mode)
#define SECONDARY_TIMEZONE 0

// Array of timezones for carousel mode
// Define up to 8 timezones
const int CAROUSEL_TIMEZONES[] = {
  -5,   // EST
   0,   // GMT
   1,   // CET
   5,   // IST
   9,   // JST
  -8,   // PST
  -6,   // CST
   8    // CST (China)
};
const int NUM_CAROUSEL_TIMEZONES = 8;

// Time to display each timezone in carousel mode (milliseconds)
#define CAROUSEL_INTERVAL 5000  // 5 seconds

// ============================================
// TIME SYNCHRONIZATION
// ============================================

// Use Real-Time Clock module (DS3231)
// Set to true if you have an RTC module connected
#define USE_RTC_MODULE false

// Update RTC time periodically (milliseconds)
// 0 = disable automatic updates
#define RTC_UPDATE_INTERVAL 3600000  // 1 hour

// Manual time set (only used if no RTC or NTP)
// Format: HH:MM (24-hour format)
#define DEFAULT_HOUR 12
#define DEFAULT_MINUTE 0

// ============================================
// BUTTON CONTROL (Optional)
// ============================================

// Enable button for mode switching
#define ENABLE_BUTTON false

// Button pin (if enabled)
#define BUTTON_PIN 2

// Button debounce delay (milliseconds)
#define BUTTON_DEBOUNCE 50

// ============================================
// LED BRIGHTNESS AUTO-ADJUSTMENT (Optional)
// ============================================

// Enable automatic brightness based on ambient light
#define ENABLE_AUTO_BRIGHTNESS false

// Light sensor pin (analog input)
#define LIGHT_SENSOR_PIN A0

// Brightness levels based on ambient light (0-15)
// Adjust these values based on your light sensor
#define BRIGHTNESS_DARK 2        // Very dark (night)
#define BRIGHTNESS_DIM 5         // Dim light
#define BRIGHTNESS_MEDIUM 10     // Normal room light
#define BRIGHTNESS_BRIGHT 15     // Bright light

// Light sensor thresholds (0-1023 for analog read)
#define LIGHT_THRESHOLD_DIM 200
#define LIGHT_THRESHOLD_MEDIUM 500
#define LIGHT_THRESHOLD_BRIGHT 800

// ============================================
// DEBUG & SERIAL OUTPUT
// ============================================

// Enable serial debug output
#define DEBUG true

// Serial communication speed (baud rate)
#define SERIAL_BAUD 9600

// ============================================
// I2C CONFIGURATION
// ============================================

// I2C clock frequency (Hz)
// Standard: 100000 (100 kHz)
// Fast: 400000 (400 kHz)
#define I2C_CLOCK 100000

// ============================================
// FEATURE FLAGS
// ============================================

// Enable temperature display (requires DS3231)
#define SHOW_TEMPERATURE false

// Enable alarm functionality (requires RTC)
#define ENABLE_ALARM false

// Alarm time (HH:MM in 24-hour format)
#define ALARM_HOUR 7
#define ALARM_MINUTE 0

// Alarm buzzer pin (if enabled)
#define BUZZER_PIN 3

// ============================================
// UTILITY FUNCTIONS
// ============================================

// Function to get timezone name from offset
inline const char* getTimezoneName(int offset) {
  switch(offset) {
    case -8: return "PST";
    case -7: return "MST";
    case -6: return "CST";
    case -5: return "EST";
    case 0:  return "GMT";
    case 1:  return "CET";
    case 5:  return "IST";
    case 8:  return "CST";
    case 9:  return "JST";
    case 10: return "AEST";
    default: return "TZ";
  }
}

#endif // CONFIG_H
