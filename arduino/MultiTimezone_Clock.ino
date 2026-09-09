/*
 * HT16K33 Multi-Timezone Digital Clock
 * =====================================
 * 
 * Displays current time in different time zones using HT16K33 LED backpack
 * 
 * Required Libraries:
 *   - Adafruit_LEDBackpack
 *   - Adafruit_GFX
 *   - Wire (built-in)
 *   - Time (optional, for advanced timezone handling)
 *   - RTClib (optional, for DS3231 RTC module)
 * 
 * Author: Digital Clock Project
 * Last Updated: 2026-09-09
 */

#include <Wire.h>
#include <Adafruit_LEDBackpack.h>
#include <Adafruit_GFX.h>
#include "config.h"

#if USE_RTC_MODULE
  #include "RTClib.h"
  RTC_DS3231 rtc;
#endif

// ============================================
// GLOBAL OBJECTS & VARIABLES
// ============================================

Adafruit_7segment display = Adafruit_7segment();

// Time tracking
unsigned long lastUpdate = 0;
unsigned long lastBrightnessUpdate = 0;
unsigned long lastCarouselUpdate = 0;

int currentHour = DEFAULT_HOUR;
int currentMinute = DEFAULT_MINUTE;
int currentSecond = 0;

// Display state
int currentDisplayMode = DISPLAY_MODE;
int currentCarouselIndex = 0;
int currentBrightness = DISPLAY_BRIGHTNESS;
bool colonState = false;

// Button state (if enabled)
#if ENABLE_BUTTON
  int buttonState = HIGH;
  int lastButtonState = HIGH;
  unsigned long lastDebounceTime = 0;
#endif

// ============================================
// SETUP FUNCTION
// ============================================

void setup() {
  // Initialize Serial (for debugging)
  #if DEBUG
    Serial.begin(SERIAL_BAUD);
    delay(1000);
    Serial.println(F("\n=== HT16K33 Multi-Timezone Digital Clock ==="));
    Serial.println(F("Initializing..."));
  #endif

  // Initialize I2C
  Wire.begin();
  Wire.setClock(I2C_CLOCK);

  // Initialize display
  if (!display.begin(HT16K33_ADDRESS)) {
    #if DEBUG
      Serial.print(F("ERROR: HT16K33 not found at address 0x"));
      Serial.println(HT16K33_ADDRESS, HEX);
    #endif
    while(1);  // Halt if display not found
  }

  // Configure display
  display.setBrightness(currentBrightness);
  display.clear();
  display.writeDisplay();

  #if DEBUG
    Serial.print(F("Display initialized at address: 0x"));
    Serial.println(HT16K33_ADDRESS, HEX);
  #endif

  // Initialize RTC (if enabled)
  #if USE_RTC_MODULE
    if (!rtc.begin()) {
      #if DEBUG
        Serial.println(F("ERROR: RTC module not found!"));
      #endif
    } else {
      #if DEBUG
        Serial.println(F("RTC module initialized"));
      #endif
      
      // Check if RTC lost power
      if (rtc.lostPower()) {
        #if DEBUG
          Serial.println(F("WARNING: RTC lost power, setting time to compile time"));
        #endif
        // Set to compile time
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
      }
      
      // Get initial time from RTC
      DateTime now = rtc.now();
      currentHour = now.hour();
      currentMinute = now.minute();
      currentSecond = now.second();
    }
  #else
    // Use default time from config
    #if DEBUG
      Serial.print(F("Using default time: "));
      Serial.print(DEFAULT_HOUR);
      Serial.print(F(":"));
      Serial.println(DEFAULT_MINUTE);
    #endif
  #endif

  // Setup button (if enabled)
  #if ENABLE_BUTTON
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    #if DEBUG
      Serial.println(F("Button control enabled"));
    #endif
  #endif

  // Setup buzzer (if enabled)
  #if ENABLE_ALARM && defined(BUZZER_PIN)
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);
  #endif

  // Setup light sensor (if enabled)
  #if ENABLE_AUTO_BRIGHTNESS
    pinMode(LIGHT_SENSOR_PIN, INPUT);
    #if DEBUG
      Serial.println(F("Auto-brightness enabled"));
    #endif
  #endif

  #if DEBUG
    Serial.println(F("Setup complete! Starting clock...\n"));
  #endif

  // Initial display
  updateDisplay();
}

// ============================================
// MAIN LOOP
// ============================================

void loop() {
  // Update time every second
  if (millis() - lastUpdate >= 1000) {
    updateTime();
    lastUpdate = millis();
  }

  // Update display
  updateDisplay();

  // Handle button input (if enabled)
  #if ENABLE_BUTTON
    handleButtonPress();
  #endif

  // Update brightness (if auto-brightness enabled)
  #if ENABLE_AUTO_BRIGHTNESS
    if (millis() - lastBrightnessUpdate >= 1000) {
      updateBrightness();
      lastBrightnessUpdate = millis();
    }
  #endif

  // Handle carousel mode
  if (currentDisplayMode == 2) {  // Carousel mode
    if (millis() - lastCarouselUpdate >= CAROUSEL_INTERVAL) {
      currentCarouselIndex++;
      if (currentCarouselIndex >= NUM_CAROUSEL_TIMEZONES) {
        currentCarouselIndex = 0;
      }
      lastCarouselUpdate = millis();
    }
  }

  delay(100);  // Small delay to prevent overwhelming the loop
}

// ============================================
// TIME UPDATE FUNCTION
// ============================================

void updateTime() {
  #if USE_RTC_MODULE
    DateTime now = rtc.now();
    currentHour = now.hour();
    currentMinute = now.minute();
    currentSecond = now.second();
  #else
    // Increment time manually
    currentSecond++;
    if (currentSecond >= 60) {
      currentSecond = 0;
      currentMinute++;
      if (currentMinute >= 60) {
        currentMinute = 0;
        currentHour++;
        if (currentHour >= 24) {
          currentHour = 0;
        }
      }
    }
  #endif

  // Toggle colon every second (if blinking enabled)
  #if BLINK_COLON
    colonState = !colonState;
  #else
    colonState = true;
  #endif

  #if DEBUG && false  // Set to true for verbose output
    Serial.print(F("Time: "));
    if (currentHour < 10) Serial.print(F("0"));
    Serial.print(currentHour);
    Serial.print(F(":"));
    if (currentMinute < 10) Serial.print(F("0"));
    Serial.print(currentMinute);
    Serial.print(F(":"));
    if (currentSecond < 10) Serial.print(F("0"));
    Serial.println(currentSecond);
  #endif

  #if ENABLE_ALARM
    checkAlarm();
  #endif
}

// ============================================
// DISPLAY UPDATE FUNCTION
// ============================================

void updateDisplay() {
  display.clear();

  int displayHour = currentHour;
  int displayMinute = currentMinute;
  int timezone = PRIMARY_TIMEZONE;

  // Determine which timezone to display
  switch(currentDisplayMode) {
    case 0:  // Single timezone (primary)
      timezone = PRIMARY_TIMEZONE;
      break;
    
    case 1:  // Dual timezone (alternates every 2 seconds)
      if ((currentSecond / 2) % 2 == 0) {
        timezone = PRIMARY_TIMEZONE;
      } else {
        timezone = SECONDARY_TIMEZONE;
      }
      break;
    
    case 2:  // Carousel mode
      timezone = CAROUSEL_TIMEZONES[currentCarouselIndex];
      break;
  }

  // Apply timezone offset
  applyTimezoneOffset(displayHour, displayMinute, timezone);

  // Convert to 12-hour format if needed
  bool isPM = false;
  if (!USE_24_HOUR_FORMAT) {
    if (displayHour >= 12) {
      isPM = true;
      if (displayHour > 12) {
        displayHour -= 12;
      }
    } else if (displayHour == 0) {
      displayHour = 12;
    }
  }

  // Display time
  display.print(displayHour * 100 + displayMinute, DEC);
  display.drawColon(colonState);
  display.writeDisplay();

  #if DEBUG && false
    Serial.print(F("Display - TZ: "));
    Serial.print(timezone);
    Serial.print(F(" | Time: "));
    Serial.print(displayHour);
    Serial.print(F(":"));
    if (displayMinute < 10) Serial.print(F("0"));
    Serial.println(displayMinute);
  #endif
}

// ============================================
// TIMEZONE OFFSET FUNCTION
// ============================================

void applyTimezoneOffset(int &hour, int &minute, int timezone) {
  // Add timezone offset (handling DST if needed)
  hour += timezone;

  // Handle hour wrapping
  if (hour >= 24) {
    hour -= 24;
  } else if (hour < 0) {
    hour += 24;
  }
}

// ============================================
// BRIGHTNESS CONTROL
// ============================================

#if ENABLE_AUTO_BRIGHTNESS
void updateBrightness() {
  int sensorValue = analogRead(LIGHT_SENSOR_PIN);
  int newBrightness = currentBrightness;

  if (sensorValue < LIGHT_THRESHOLD_DIM) {
    newBrightness = BRIGHTNESS_DARK;
  } else if (sensorValue < LIGHT_THRESHOLD_MEDIUM) {
    newBrightness = BRIGHTNESS_DIM;
  } else if (sensorValue < LIGHT_THRESHOLD_BRIGHT) {
    newBrightness = BRIGHTNESS_MEDIUM;
  } else {
    newBrightness = BRIGHTNESS_BRIGHT;
  }

  if (newBrightness != currentBrightness) {
    currentBrightness = newBrightness;
    display.setBrightness(currentBrightness);
    
    #if DEBUG
      Serial.print(F("Brightness adjusted to: "));
      Serial.println(currentBrightness);
    #endif
  }
}
#endif

// ============================================
// BUTTON HANDLING
// ============================================

#if ENABLE_BUTTON
void handleButtonPress() {
  int reading = digitalRead(BUTTON_PIN);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > BUTTON_DEBOUNCE) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == LOW) {  // Button pressed
        cycleDisplayMode();
        
        #if DEBUG
          Serial.print(F("Button pressed - Display mode: "));
          Serial.println(currentDisplayMode);
        #endif
      }
    }
  }

  lastButtonState = reading;
}

void cycleDisplayMode() {
  currentDisplayMode++;
  if (currentDisplayMode > 2) {
    currentDisplayMode = 0;
  }
  currentCarouselIndex = 0;
  lastCarouselUpdate = millis();
}
#endif

// ============================================
// ALARM FUNCTIONALITY
// ============================================

#if ENABLE_ALARM
void checkAlarm() {
  if (currentHour == ALARM_HOUR && currentMinute == ALARM_MINUTE && currentSecond == 0) {
    #if DEBUG
      Serial.println(F("ALARM TRIGGERED!"));
    #endif
    
    #if defined(BUZZER_PIN)
      // Sound buzzer (simple beep pattern)
      for (int i = 0; i < 10; i++) {
        digitalWrite(BUZZER_PIN, HIGH);
        delay(100);
        digitalWrite(BUZZER_PIN, LOW);
        delay(100);
      }
    #endif
  }
}
#endif

// ============================================
// UTILITY FUNCTIONS
// ============================================

// Function to manually set time (via Serial or button)
void setTime(int hour, int minute) {
  currentHour = hour;
  currentMinute = minute;
  currentSecond = 0;

  #if USE_RTC_MODULE
    DateTime now = rtc.now();
    rtc.adjust(DateTime(now.year(), now.month(), now.day(), hour, minute, 0));
  #endif

  #if DEBUG
    Serial.print(F("Time set to: "));
    Serial.print(hour);
    Serial.print(F(":"));
    Serial.println(minute);
  #endif
}

// Function to set brightness
void setBrightness(int brightness) {
  if (brightness >= 0 && brightness <= 15) {
    currentBrightness = brightness;
    display.setBrightness(currentBrightness);
    
    #if DEBUG
      Serial.print(F("Brightness set to: "));
      Serial.println(currentBrightness);
    #endif
  }
}

// ============================================
// END OF SKETCH
// ============================================
