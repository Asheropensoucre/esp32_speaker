# Speaker Project - Agent Guidelines

## Project Overview
This is an ESP32-based Bluetooth audio receiver with display and physical controls ("Sir Potato OS"). It streams audio via Bluetooth A2DP, displays song information on a TFT screen, and provides physical buttons for playback control and volume adjustment.

**Current Status:** Phase 1 Complete ✅ | Phase 2 Complete ✅ | Phase 3 Complete ✅ | Phase 4 Complete ✅ | Phase 5 Complete ✅

---

## Directory Structure
- `src/` - Main source code (modular architecture with managers)
- `include/` - Header files for all manager classes
- `lib/` - Library directory (empty except README)
- `test/` - Documentation and resources (Project.md, README.md, sprite sheet, sprite_data.h)
- `backup/` - Backup files
- `DEVELOPMENT.md` - Development log and progress tracking
- `convert_sprites.py` - Python script for sprite conversion

---

## Essential Commands

### PlatformIO Commands
Since this project uses PlatformIO with the Arduino framework:

- **Build**: `pio run`
- **Upload**: `pio run --target upload`
- **Clean**: `pio run --target clean`
- **Monitor**: `pio device monitor` (uses 115200 baud as configured in platformio.ini)
- **Run tests**: No test framework configured in this project

### Development Workflow
1. Edit source files in `src/` or `include/`
2. Build with `pio run` to check for compilation errors
3. Upload to ESP32 with `pio run --target upload` (when device is connected)
4. Monitor output with `pio device monitor`

### Sprite Conversion
```bash
# Convert sprite sheet to raw RGB565 C arrays (no compression)
python3 convert_sprites.py
```

---

## Code Patterns and Conventions

### Modular Architecture
The project uses a modular architecture with manager classes:

```cpp
// Global manager objects in main.cpp
StateManager stateManager;
DisplayManager displayManager;
AudioManager* audioManager;
InputManager inputManager;
```

### Pin Definitions
- All pins defined using `#define` statements in respective manager headers
- Related pins grouped by function (Power & Amp, Screen Pins, Audio Pins, Buttons)
- Clear commenting for each pin's purpose
- TFT_eSPI pins configured in platformio.ini via build_flags

### State Management
- State machine defined in `StateManager.h`
- States: BOOTING, PAIRING, PLAYING, PAUSED
- Animation states: BOOT_SEQUENCE, PAIRING_SEARCH, PLAYING_DANCE, PAUSED_NAP, BUTTON_REACTION
- Use `stateManager.setState()` and `stateManager.getState()`
- Use `stateManager.getCurrentAnimationFrame()` for animations
- `resetAnimation()` called automatically on state changes

### Function Organization
- **DisplayManager**: `showBootScreen()`, `showPairingScreen()`, `updateSongTitle()`, `showVolume()`, `drawAnimatedSprite()`, `showOverlay()`, `drawFullDisplay()`
- **AudioManager**: `play()`, `pause()`, `next()`, `volumeUp()`, `volumeDown()`, `isPlaying()`, `getSink()`
- **InputManager**: `begin()`, `update()`, `isButtonAPressed()`, `isButtonBPressed()`, `isButtonXPressed()`, `isButtonYPressed()`
- **StateManager**: `getState()`, `setState()`, `isState()`, `getCurrentAnimationFrame()`, `advanceAnimationFrame()`, `resetAnimation()`

### Button Handling
- Active-low buttons using `INPUT_PULLUP`
- Button press detected with `digitalRead(pin) == LOW`
- Debouncing handled internally in InputManager with 50ms delay
- Non-blocking edge detection prevents rapid-fire triggers
- `update()` must be called at top of loop()
- Initial pin reads in `begin()` to prevent ghost presses

### Volume Control
- Volume range 0-127 mapped to 0-100% for display
- Adjustments made in steps of 5
- Boundary checking (0 and 127 limits)
- Use `audioManager->volumeUp()` and `audioManager->volumeDown()`

### Animation System
- Animation speed: 150ms between frames
- State-based frame sequences
- Automatic looping for continuous animations
- One-time animations for boot and button reactions
- Raw 120x120 sprites with RGB565 format (no compression)
- Hardware-accelerated rendering via TFT_eSPI pushImage()

### Overlay System
- Overlay duration: 2 seconds
- Used for button feedback
- Automatically hides after duration
- Colors: Yellow for volume, Pink for playing, Teal for paused

### Kawaii Color Palette
- Custom pastel colors defined in RGB565 format
- State-based background colors
- Consistent visual theme
- Matches original design specifications

### Display Rendering
- Only call `fillScreen()` when state changes (prevents flicker)
- Track `lastDrawnState` to detect state changes
- Use `pushImage()` for sprite updates (every frame)
- Static elements redrawn only on state change
- `setSwapBytes(true)` required for correct RGB565 byte order

---

## Important Gotchas

1. **Button Pin Selection**: Code specifically avoids ESP32 strapping pins (like D2, D5) to prevent boot loops and "stuck button" bugs.

2. **I2S Configuration**: Audio output uses I2S interface with specific pin configuration:
   - BCK: IO13
   - WS/LRC: IO27  
   - DOUT: IO26
   - DIN: Not connected (I2S_PIN_NO_CHANGE)

3. **Display Initialization**: TFT display initialized as 240x240 resolution
   - Uses hardware SPI with TFT_eSPI library
   - Pins configured in platformio.ini: MOSI(21), DC(22), SCLK(23), CS(33), RST(-1/not connected)
   - Rotation set to 2 (180 degrees)
   - setSwapBytes(true) for correct RGB565 rendering

4. **Amplifier Control**: Amplifier enable pin (IO14) must be pulled HIGH to unmute

5. **Backlight Control**: Backlight pin (IO32) must be pulled HIGH to turn on display LEDs

6. **Bluetooth Device Name**: Set to "ESP32 Potato" in `audioManager->start()`

7. **Default Display Text**: Shows "potato" as default song title until metadata received

8. **Flash Memory**: Current usage 50.6% (~1.6MB / 3MB)
   - Using `huge_app.csv` partition scheme for maximum space
   - All assets stored in PROGMEM
   - Use optimization flags in platformio.ini
   - Monitor memory usage after each change

9. **Partition Scheme**: Using `huge_app.csv` provides ~3MB app space
   - Disables OTA updates (not needed)
   - Perfect for asset-heavy projects
   - More than 8x space compared to default

10. **Kawaii Color Palette**: Custom pastel colors must be used
    - Pink (#ffabde) for Playing state
    - Purple (#dbafff) for Booting state
    - Blue (#afd0ff) for Pairing state
    - Teal (#aafff8) for Paused state
    - Yellow (#ffffb0) for UI Overlays

11. **Button Update Loop**: `inputManager.update()` MUST be called at the top of `loop()` for proper non-blocking button handling

12. **Ghost Press Prevention**: InputManager `begin()` must include 50ms delay and initial pin reads to stabilize pull-up resistors

13. **Play/Pause State**: Use local boolean `playing` variable, NOT AVRC playback status callback (too latent)

14. **Screen Flicker Prevention**: Only call `fillScreen()` when state changes, use `pushImage()` for sprite updates

15. **TFT_eSPI Configuration**: All display pins configured in platformio.ini via build_flags, do NOT modify User_Setup.h

16. **Button X GPIO 19 SPI Conflict (CRITICAL)**: TFT_eSPI Hardware SPI initialization defaults to GPIO 19 as MISO pin
    - **Problem:** This overwrites INPUT_PULLUP protection, causing phantom presses at boot
    - **Solution:** Initialize DisplayManager BEFORE InputManager in setup()
    - **Order:** displayManager.begin() → delay(50ms) → inputManager.begin()
    - **Backup:** Added `-D TFT_MISO=-1` to platformio.ini as additional safety measure
    - **Status:** ✅ FIXED - Do NOT change initialization order or Button X will break again

---

## Libraries Used
- Arduino core (`#include <Arduino.h>`)
- TFT_eSPI (`#include <TFT_eSPI.h>`)
- BluetoothA2DPSink (`#include "BluetoothA2DPSink.h"`)
- PIL (Python Imaging Library) for sprite conversion

---

## Testing Approach
This project does not appear to have automated tests configured. Validation is done through:
1. Compilation verification with PlatformIO
2. Manual testing on physical hardware (when ESP32 is connected)
3. Serial monitoring for debugging

---

## File Encoding and Format
- Standard C++/Arduino syntax
- No special formatting requirements observed
- Consistent use of 2-space indentation

---

## Memory Optimization

### Current Status
- **Flash:** 50.6% (1,592,161 / 3,145,728 bytes)
- **RAM:** 12.0% (39,408 / 327,680 bytes)
- **Remaining Flash:** ~1.5MB

### Optimization Flags (platformio.ini)
```ini
build_flags = 
    -Os                                    # Optimize for size
    -ffunction-sections                    # Separate functions
    -fdata-sections                         # Separate data
    -Wl,--gc-sections                      # Garbage collect unused sections
    -DCORE_DEBUG_LEVEL=0                   # Disable debug logging
    -DBT_SPP_ENABLED=0                     # Disable BT SPP
    -DBLE_ENABLED=0                        # Disable BLE
    -DARDUINO_USB_MODE=0                   # Disable USB
    -DARDUINO_USB_CDC_ON_BOOT=0            # Disable USB CDC
    -DLOG_LOCAL_LEVEL=0                    # Disable local logging
    -DARDUINO_EVENT_LOOP_STACK_SIZE=2048   # Reduce stack size
    -D CGRAM_OFFSET=1                      # Fix hardware mirroring
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

lib_ignore = AudioTools                    # Ignore unused library
```

### Partition Scheme
```ini
board_build.partitions = huge_app.csv
```
- Provides ~3MB app space instead of 1.3MB
- Disables OTA updates (not needed for this project)
- Perfect for asset-heavy projects

### Best Practices
- Store all assets in PROGMEM using `PROGMEM` keyword
- Use `pgm_read_byte()` and `pgm_read_word()` to read PROGMEM data
- Avoid large static arrays in RAM
- Use `const` and `PROGMEM` for constant data
- Use raw RGB565 arrays for maximum hardware speed

---

## Phase 3 Guidelines

### Animation System
- Frame sequences defined in StateManager
- State-based automatic animation selection
- 150ms timing between frames
- Looping vs one-time animations
- Raw 120x120 sprites with RGB565 format (no compression)
- Hardware-accelerated rendering via TFT_eSPI pushImage()

### Display Zones
The screen is divided into 3 zones:
- **Header** (0-60px): Volume, status icons
- **Center** (60-180px): Sir Potato sprite (120x120)
- **Footer** (180-240px): Song title (marquee scrolling)

### Overlay System
- 2-second duration for button feedback
- Automatic hiding
- Color-coded feedback (Yellow=volume, Pink=playing, Teal=paused)

### Animation Sequences
- **Boot:** Frames [1, 7, 14, 16] (one-time)
- **Pairing:** Frames [5, 2, 6, 9] (looping)
- **Playing:** Frames [3, 7, 9, 10, 14, 15] (looping)
- **Paused:** Frames [4, 8, 12] (looping)
- **Button Reaction:** Frames [5, 9, 13] (one-time)

### Kawaii Color Palette
**Custom RGB565 Colors:**
- `COLOR_KAWAII_PINK` (0xF8D5) - Playing state
- `COLOR_KAWAII_PURPLE` (0xDADF) - Booting state
- `COLOR_KAWAII_BLUE` (0xADFF) - Pairing state
- `COLOR_KAWAII_TEAL` (0xAFFF) - Paused state
- `COLOR_KAWAII_YELLOW` (0xFFD6) - UI Overlays

**State-Based Usage:**
- Boot screen: Purple background
- Pairing screen: Blue background
- Playing overlay: Pink
- Paused overlay: Teal
- Volume/Next overlays: Yellow

---

## Phase 4 Guidelines

### TFT_eSPI Migration
- Migrated from Adafruit_GFX to TFT_eSPI library
- Removed RLE compression (using raw RGB565 arrays)
- Configured pins via platformio.ini build_flags
- Enabled hardware SPI for faster rendering (40MHz)
- Added setSwapBytes(true) for correct byte order

### Performance Improvements
- Hardware SPI for blazing-fast sprite rendering
- Uncompressed sprites for maximum hardware speed
- Non-blocking button handling
- Optimized display updates (only redraw when needed)

### Bug Fixes
- Fixed button rapid-fire with proper edge detection
- Fixed screen orientation with setRotation(2)
- Fixed screen flicker with lastDrawnState tracking
- Fixed ghost presses with 50ms stabilization delay
- Fixed Play/Pause sync with local boolean state
- Fixed animation step bug with resetAnimation() in setState()

### Known Limitations
- Bluetooth connection state transitions not automatic (protected member access issue)
- Device stays in PAIRING state until manual state change
- Text mirroring may still occur (CGRAM_OFFSET=1 may not be sufficient)

---

## Development Notes

### When Adding New Features
1. Update the appropriate manager class
2. Test compilation with `pio run`
3. Check memory usage
4. Update DEVELOPMENT.md
5. Test on hardware if available

### When Modifying Existing Code
1. Keep the modular architecture intact
2. Don't mix concerns between managers
3. Use the state machine for state-dependent behavior
4. Maintain backward compatibility
5. Use Kawaii color palette for visual consistency
6. Call inputManager.update() at top of loop()
7. Only call fillScreen() when state changes

### Debugging
- Use Serial.println() for debugging (but remove before final build)
- Monitor flash usage after each change
- Test button functionality thoroughly
- Verify Bluetooth connection stability
- Test animation timing and smoothness
- Verify color palette matches specifications
- Check for ghost presses on boot
- Verify screen orientation and mirroring

### Known Issues
- Bluetooth connection state transitions not automatic (protected member access)
- Text may still be mirrored (CGRAM_OFFSET=1 may not fix all cases)
- Device requires manual state changes for PAIRING ↔ PLAYING transitions

---

*Last Updated: 2026-04-19*
*Phase 5 Complete: Button X GPIO 19 SPI conflict resolved via initialization order*
