# Sir Potato OS - Hardware Pin Mapping

## Overview
This document describes the hardware connections between the ESP32 and the Pimoroni Pirate Audio HAT for the Sir Potato OS project.

## Hardware Components
- **ESP32** - Microcontroller
- **Pimoroni Pirate Audio HAT** - Audio amplifier, TFT display, and buttons
- **1.3" IPS Color LCD** - 240x240 resolution display

---

## Pin Connections

### 1. Power & Ground
| Pirate Pin | ESP32 Pin | Description |
|------------|-----------|-------------|
| Pin 2 (5V) | 5V / VIN | Powers the amplifier |
| Pin 1 (3.3V) | 3.3V | Powers the screen logic |
| Pin 17 (3.3V) | 3.3V | Powers the screen logic |
| Pin 6 (GND) | GND | Ground connection |

### 2. Audio & Amplifier (I2S)
| Pirate Pin | ESP32 Pin | Description |
|------------|-----------|-------------|
| Pin 12 (I2S Clock) | D13 | BCLK (Bit Clock) |
| Pin 22 (Amp Enable) | D14 | Must be pulled HIGH to unmute |
| Pin 35 (Word Select) | D27 | LRC/WS (Word Select) |
| Pin 40 (I2S Data) | D26 | DOUT (Data Out) |

### 3. Screen (Hardware SPI - TFT_eSPI)
| Pirate Pin | ESP32 Pin | Description |
|------------|-----------|-------------|
| Pin 19 (SPI MOSI) | D21 | MOSI (Master Out Slave In) |
| Pin 21 (SPI DC) | D22 | DC (Data/Command) |
| Pin 23 (SPI Clock) | D23 | SCLK (Serial Clock) |
| Pin 26 (SPI CS) | D33 | CS (Chip Select) |
| Pin 33 (Backlight) | D32 | Must be pulled HIGH to turn on LEDs |

### 4. Playback Buttons
| Pirate Pin | ESP32 Pin | Button | Function |
|------------|-----------|--------|----------|
| Pin 29 (Button A) | D25 | A | Volume Up (+5) |
| Pin 31 (Button B) | D18 | B | Volume Down (-5) |
| Pin 36 (Button X) | D19 | X | Play/Pause Toggle |
| Pin 18 (Button Y) | RX2 (D16) | Y | Next Track |

---

## Important Notes

### Button Pin Selection
We specifically avoid ESP32 strapping pins (like D2, D5) to prevent boot loops and "stuck button" bugs. The selected pins (D25, D18, D19, D16) are safe to use.

### Button Configuration
- All buttons are **active-low** (pressed = LOW, released = HIGH)
- Use `INPUT_PULLUP` mode for internal pull-up resistors
- Button press detected with `digitalRead(pin) == LOW`
- Debouncing handled in InputManager with 50ms delay
- Non-blocking edge detection prevents rapid-fire triggers
- **CRITICAL:** Display MUST be initialized BEFORE inputs in setup() to ensure GPIO 19 (Button X) retains INPUT_PULLUP protection

### Display Configuration
- Resolution: 240x240 pixels
- Interface: Hardware SPI (TFT_eSPI library)
- Color format: RGB565 (16-bit, Big-Endian)
- Backlight: Must be enabled (GPIO 32 HIGH)
- Rotation: 180 degrees (setRotation(2))
- Byte swapping: Enabled for correct RGB565 rendering

### Audio Configuration
- Interface: I2S
- Sample rate: 44.1kHz
- Bit depth: 16-bit
- Channels: Stereo
- Amplifier: Must be enabled (GPIO 14 HIGH)
- Bluetooth: A2DP sink with AVRCP support
- Connection State: Tracked via `set_on_connection_state_changed()` callback
- Auto-Sync: Enabled only when phone is connected (guards in main loop)
- Phone State Sync: Automatic PLAYING/PAUSED/volume sync when connected

---

## Power Requirements

### Voltage Levels
- **ESP32:** 3.3V logic, 5V input
- **Display:** 3.3V logic
- **Amplifier:** 5V power
- **Buttons:** 3.3V logic

### Current Draw
- **ESP32:** ~100-200mA
- **Display:** ~50-100mA
- **Amplifier:** ~50-200mA (depends on volume)
- **Total:** ~200-500mA

---

## Software Configuration

### PlatformIO Configuration
```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200

build_flags = 
    -Os
    -ffunction-sections
    -fdata-sections
    -Wl,--gc-sections
    -DCORE_DEBUG_LEVEL=0
    -DBT_SPP_ENABLED=0
    -DBLE_ENABLED=0
    -DARDUINO_USB_MODE=0
    -DARDUINO_USB_CDC_ON_BOOT=0
    -DLOG_LOCAL_LEVEL=0
    -DARDUINO_EVENT_LOOP_STACK_SIZE=2048
    -D CGRAM_OFFSET=1
    ; TFT_eSPI configuration
    -D USER_SETUP_LOADED=1
    -D ST7789_DRIVER=1
    -D TFT_WIDTH=240
    -D TFT_HEIGHT=240
    -D TFT_MOSI=21
    -D TFT_SCLK=23
    -D TFT_CS=33
    -D TFT_DC=22
    -D TFT_RST=-1
    -D LOAD_GLCD=1
    -D SPI_FREQUENCY=40000000

lib_deps =
    bodmer/TFT_eSPI@^2.5.31
    https://github.com/pschatzmann/ESP32-A2DP.git
```

### Pin Definitions in Code
```cpp
// Power & Amp
#define BACKLIGHT_PIN 32
#define AMP_ENABLE_PIN 14

// Screen Pins (configured in platformio.ini)
// TFT_MOSI 21
// TFT_DC   22
// TFT_SCLK 23
// TFT_CS   33
// TFT_RST  -1

// Audio Pins 
#define I2S_BCLK 13
#define I2S_LRC  27
#define I2S_DOUT 26

// Button Pins
#define BTN_A 25
#define BTN_B 18
#define BTN_X 19
#define BTN_Y 16
```

---

## Troubleshooting

### Display Not Working
1. Check GPIO 32 is HIGH (backlight)
2. Verify SPI connections (MOSI, DC, SCLK, CS)
3. Check display initialization in code
4. Verify TFT_eSPI library is installed
5. Check build_flags in platformio.ini

### No Audio Output
1. Check GPIO 14 is HIGH (amp enable)
2. Verify I2S connections (BCLK, LRC, DOUT)
3. Check Bluetooth connection status
4. Verify volume is not muted

### Buttons Not Responding
1. Verify button pins are correct
2. Check INPUT_PULLUP mode is set
3. Test with multimeter (should be HIGH when not pressed)
4. Avoid strapping pins (D2, D5)
5. Check for ghost presses on boot (add 50ms delay in begin())

### Boot Loop Issues
1. Check if any button is stuck LOW
2. Verify no pins are connected to strapping pins
3. Check power supply stability
4. Ensure proper pull-up resistor initialization

### Screen Mirrored/Upside Down
1. Check tft.setRotation(2) is set in begin()
2. Verify CGRAM_OFFSET=1 in build_flags
3. Do NOT use raw MADCTL commands (breaks offsets)
4. Use library functions only

### Button X Not Working (GPIO 19 SPI Conflict)
**Root Cause:** TFT_eSPI initializes Hardware SPI and defaults to using GPIO 19 as MISO, stripping INPUT_PULLUP protection
**Solution:** Initialize display BEFORE inputs in setup():
1. displayManager.begin() - Claims SPI pins
2. delay(50) - Let SPI bus settle
3. inputManager.begin() - Reclaims GPIO 19 with INPUT_PULLUP protection

**If still not working:**
1. Verify initialization order in main.cpp setup()
2. Ensure InputManager.begin() has 50ms stabilization delay
3. Check digitalRead() initial values captured after delay
4. Verify edge detection logic (LOW->HIGH transition)
5. Check isButtonXPressed() flag handling

### UI Out of Sync with Phone (Bluetooth State Mismatch)
**Root Cause:** ESP32 state machine wasn't tracking phone state changes from Bluetooth callbacks
**Solution:** Enable Bluetooth connection state tracking and auto-sync:
1. Use `set_on_connection_state_changed()` callback to track connection state
2. Wrap auto-sync block with `if (audioManager->isConnected())` guard
3. Auto-sync only triggers when phone is actually connected
4. Callbacks fire for play/pause changes from phone
5. Volume sync detects phone volume changes

**If auto-sync not working:**
1. Check serial monitor for "Bluetooth Connection State" messages
2. Verify "Phone CONNECTED" message appears when phone connects
3. Confirm auto-sync logs only appear after connection
4. Check AVRC callbacks are enabled in AudioManager::begin()
5. Verify play state boolean is updated by callbacks

### Screen Flickering
1. Check lastDrawnState tracking is implemented
2. Only call fillScreen() when state changes
3. Use pushImage() for sprite updates (not fillScreen)
4. Verify animation timing is correct (150ms)

---

## Safety Notes

1. **Voltage Levels:** Ensure 3.3V and 5V are not mixed
2. **Current Limits:** Don't exceed ESP32 current limits
3. **Ground Connections:** All grounds must be connected together
4. **Power Supply:** Use adequate power supply (500mA+ recommended)
5. **ESD Protection:** Handle components carefully to avoid static damage

---

## Recent Changes (2026-04-18)

### TFT_eSPI Migration
- Migrated from Adafruit_GFX to TFT_eSPI library
- Removed RLE compression (using raw RGB565 arrays)
- Configured pins via platformio.ini build_flags
- Enabled hardware SPI for faster rendering
- Added setSwapBytes(true) for correct byte order

### Bug Fixes
- Fixed button rapid-fire with proper edge detection
- Fixed screen orientation with setRotation(2)
- Fixed screen flicker with lastDrawnState tracking
- Fixed ghost presses with 50ms stabilization delay
- Fixed Play/Pause sync with local boolean state
- Fixed animation step bug with resetAnimation() in setState()

### Performance Improvements
- Hardware SPI (40MHz) for blazing-fast sprite rendering
- Uncompressed sprites for maximum hardware speed
- Non-blocking button handling
- Optimized display updates (only redraw when needed)

---

*Last Updated: 2026-04-19*
*Phase 6 Complete: Bluetooth State Machine & Phone Sync - Auto-sync now tracks phone connections*
