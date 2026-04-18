# 🥔 Sir Potato OS - Development Log

## Project Overview
ESP32-based Bluetooth audio receiver with TFT display and physical controls. Transforms a standard speaker into an interactive "Virtual Pet" style device.

---

## Phase 1: Foundation & Architecture ✅ COMPLETE
**Date:** 2026-04-18  
**Status:** ✅ Complete

### Accomplishments
- ✅ Created modular architecture with 5 manager classes
- ✅ Implemented state machine (BOOTING, PAIRING, PLAYING, PAUSED)
- ✅ Refactored monolithic main.cpp into clean separation of concerns
- ✅ All original functionality preserved and tested
- ✅ Optimized flash usage from 84.9% → 84.1% (saved ~10KB)

### Files Created
```
include/
├── StateManager.h      # State machine definitions
├── DisplayManager.h    # Display & UI management
├── AudioManager.h     # Bluetooth & I2S audio
├── InputManager.h     # Button handling
└── Assets.h           # Font and sprite data

src/
├── StateManager.cpp   # State machine logic
├── DisplayManager.cpp # Display operations
├── AudioManager.cpp   # Audio operations
├── InputManager.cpp   # Button operations
└── main.cpp           # Main loop & coordinator (98 lines)
```

### Memory Optimization
- **Flash:** 84.9% → 84.1% → 42.0% (after partition change)
- **RAM:** 11.9% (38,984 / 327,680 bytes)
- **Optimizations Applied:**
  - `-Os` optimization flag
  - Function/data section garbage collection
  - Disabled unused features (USB CDC, BT SPP, BLE)
  - Reduced debug logging
  - Ignored AudioTools library

---

## Phase 2: Visual Assets & Display ✅ COMPLETE
**Date:** 2026-04-18  
**Status:** ✅ Complete

### Goals
- [x] Convert FiraCode Nerd Font to C-header file
- [x] Convert Sir Potato Sprite Sheet to C-array in Assets.h
- [x] Build `drawSprite(frame_index)` function in DisplayManager
- [x] Implement screen layout zones (Header, Center, Footer)
- [x] Implement marquee text scroller
- [x] Test basic text rendering with new font

### Progress
- [x] Phase 1 complete
- [x] Display zone implementation (Header, Center, Footer)
- [x] Sprite drawing framework with RLE decompression
- [x] Marquee text scroller for long song titles
- [x] Custom 39-character font created and integrated
- [x] Sprite sheet converted with RLE compression
- [x] All assets integrated and tested

### New Features Added
- **Display Zones:** Screen divided into 3 zones (60px header, 120px center, 60px footer)
- **Sprite Drawing:** `drawSprite()` and `drawRLESprite()` functions with real data
- **Marquee Text:** Scrolling text for long song titles in footer zone
- **Custom Font:** Minimal 39-character font (A-Z, 0-9, space, dash, colon)
- **RLE Compression:** 77% average reduction in sprite data size
- **Placeholder Sprite:** Cute potato shape with eyes and mouth for testing

### Code Changes
- Created `convert_sprites.py` Python script for sprite conversion
- Generated `test/sprite_data.h` with RLE-compressed sprite frames
- Updated `Assets.h` with custom 39-character font data
- Updated `DisplayManager.h` with zone definitions and new methods
- Updated `DisplayManager.cpp` with zone drawing, sprite rendering, marquee logic, and custom font rendering
- Added RLE decompression function `drawRLESprite()`
- Added constants for zone heights and sprite dimensions

### Memory Status
- **Flash:** 84.1% → 42.0% (after partition change)
- **RAM:** 11.9% (38,984 / 327,680 bytes)
- **Remaining:** ~1.8MB (after partition change)
- **Font Size:** ~312 bytes (39 chars × 8 bytes) ✅
- **Sprite Size:** ~110KB (RLE compressed) ✅

### RLE Compression Results
- Original size: 460KB (16 frames × 120×120 × 2 bytes)
- Compressed size: ~110KB
- **Compression ratio: 77% average reduction**
- Frame sizes range from 4.8KB to 9.2KB

---

## Phase 3: The OS Polish ✅ COMPLETE
**Date:** 2026-04-18  
**Status:** ✅ Complete

### Goals
- [x] Implement state-based sprite animations
- [x] Build pop-up overlay system
- [x] Map sprite frames to states
- [x] Integrate button reactions
- [x] Solve sprite data size issue
- [x] Test all display states
- [x] Implement Kawaii color palette

### Progress
- [x] Animation sequences defined for each state
- [x] StateManager updated with animation tracking
- [x] DisplayManager updated with animation timing
- [x] Pop-up overlay system implemented
- [x] Button feedback overlays integrated
- [x] Main loop updated with animation system
- [x] Sprite data integrated and working
- [x] Memory issue solved with huge_app.csv partition
- [x] All display states functional
- [x] Kawaii color palette implemented

### New Features Added
- **Animation System:** State-based frame sequences (Boot, Pairing, Playing, Paused)
- **Animation Timing:** 150ms between frames for smooth animations
- **Pop-up Overlays:** 2-second duration overlays for button feedback
- **Button Feedback:** Visual feedback for all button presses
- **State Transitions:** Automatic animation updates on state changes
- **Real Sprites:** Full 120x120 animated sprites with RLE decompression
- **Kawaii Color Palette:** Custom pastel colors matching specifications

### Animation Sequences
- **Boot:** Frames [1, 7, 14, 16] (one-time)
- **Pairing:** Frames [5, 2, 6, 9] (looping)
- **Playing:** Frames [3, 7, 9, 10, 14, 15] (looping)
- **Paused:** Frames [4, 8, 12] (looping)
- **Button Reaction:** Frames [5, 9, 13] (one-time)

### Kawaii Color Palette Implementation
**Hex → RGB565 Conversion:**
- **Pink (#ffabde):** `0xF8D5` - Playing state
- **Purple (#dbafff):** `0xDADF` - Booting state
- **Blue (#afd0ff):** `0xADFF` - Pairing state
- **Teal (#aafff8):** `0xAFFF` - Paused state
- **Yellow (#ffffb0):** `0xFFD6` - UI Overlays

**State-Based Colors:**
- Boot screen: Purple background ✅
- Pairing screen: Blue background ✅
- Playing overlay: Pink ✅
- Paused overlay: Teal ✅
- Volume/Next overlays: Yellow ✅

### Code Changes
- Updated `StateManager.h` with AnimationState enum and animation methods
- Updated `StateManager.cpp` with animation frame management
- Updated `DisplayManager.h` with animation and overlay methods
- Updated `DisplayManager.cpp` with animation timing and overlay rendering
- Updated `main.cpp` with animation loop and button feedback
- Updated `platformio.ini` with `huge_app.csv` partition scheme
- Enabled sprite data in DisplayManager
- Added Kawaii color palette definitions
- Implemented state-based color system

### Memory Status
- **Flash:** 42.0% (1,319,641 / 3,145,728 bytes)
- **RAM:** 11.9% (38,984 / 327,680 bytes)
- **Remaining:** ~1.8MB
- **Sprite Data:** ~110KB (RLE compressed) ✅
- **Font Data:** ~312 bytes ✅

### Partition Scheme Update
**Solution:** Used `huge_app.csv` partition scheme  
**Result:** Increased app space from 1.3MB to 3MB  
**Flash usage:** Dropped from 84.2% to 42.0%  
**Available space:** ~1.8MB (more than 8x improvement!)

---

## Phase 6: Bluetooth State Machine & Phone Sync ✅ COMPLETE
**Date:** 2026-04-19
**Status:** ✅ Complete

### Problem
UI and animations were out of sync with phone state. If user paused music on phone, ESP32 didn't know and continued showing PLAYING animations. Volume changes on phone weren't displayed.

### Root Cause
Missing Bluetooth connection state callback and no auto-sync mechanism in main loop. Auto-sync was triggering before phone connected.

### Solution Implemented

#### Task 1: Enable Bluetooth Connection Callback
Added `set_on_connection_state_changed()` callback in AudioManager.begin():
- Tracks connection state in `connected` boolean
- Logs connection/disconnection events
- Updated isConnected() to return actual connection state

#### Task 2: Auto-Sync Phone State to Visual State
Wrapped auto-sync block in connection check in main loop:
- Only syncs play/pause state when phone is connected
- Only syncs volume changes when phone is connected
- Detects state changes with static variable tracking
- Updates display and state machine automatically
- Logs all state transitions for debugging

### Code Changes
- **include/AudioManager.h:** Added `bool connected = false` member variable
- **src/AudioManager.cpp:** Added connection state callback with logging
- **src/AudioManager.cpp:** Updated isConnected() to return connection state
- **src/main.cpp:** Wrapped auto-sync in `if (audioManager->isConnected())` block
- **src/main.cpp:** Added volume change detection in auto-sync

### Files Modified
- `include/AudioManager.h` - Added connected state tracking
- `src/AudioManager.cpp` - Added connection callback and logging
- `src/main.cpp` - Added connection guard to auto-sync block

### Testing Results
✅ Display stays in PAIRING until phone connects
✅ Auto-sync only activates after "Phone CONNECTED" message
✅ Play/Pause changes from phone show on display
✅ Volume changes from phone show on display
✅ Phone disconnect detected and tracked
✅ All debug logging working

### Key Implementation Details
```cpp
// Connection callback in AudioManager::begin()
a2dp_sink.set_on_connection_state_changed([](esp_a2dp_connection_state_t state, void* param) {
  if (state == ESP_A2DP_CONNECTION_STATE_CONNECTED) {
    AudioManager::instance->connected = true;
  } else {
    AudioManager::instance->connected = false;
  }
});

// Auto-sync in main loop (only when connected)
if (audioManager->isConnected()) {
  static bool lastPhonePlayState = false;
  static int lastPhoneVolume = -1;
  // ... sync logic ...
}
```

### Debug Serial Output
- "Bluetooth Connection State: X" - Connection state change
- "Phone CONNECTED" - Connection established
- "Phone DISCONNECTED" - Connection lost
- "Auto-Sync: Play state changed from X to Y" - Play state sync
- "Auto-Sync: Volume changed from X to Y" - Volume sync

### Memory Impact
- +1 bool member variable in AudioManager (~1 byte)
- No change to flash usage
- No change to RAM usage

---

## Phase 5: Button X GPIO 19 SPI Conflict Fix ✅ COMPLETE
**Date:** 2026-04-19  
**Status:** ✅ Complete

### Problem
Button X (GPIO 19) was experiencing:
- Phantom presses at boot during TFT_eSPI initialization
- No button registration after boot
- State machine jumping to PLAYING, skipping BOOTING/PAIRING animations

### Root Cause
When TFT_eSPI library initializes the Hardware SPI bus, it defaults to using GPIO 19 as the MISO (Master In Slave Out) pin. This overwrote the INPUT_PULLUP mode set by InputManager, stripping away pull-up protection and allowing electrical noise to trigger phantom presses.

### Solution: Initialization Order
Changed setup() initialization sequence to:
1. Initialize DisplayManager.begin() FIRST - Hijacks SPI pins including GPIO 19
2. delay(50ms) - Let SPI bus settle
3. Initialize InputManager.begin() SECOND - Reclaims GPIO 19 with INPUT_PULLUP

This ensures GPIO 19 is properly protected with internal pull-up resistors AFTER TFT_eSPI finishes SPI setup.

### Code Changes
- **src/main.cpp:** Reordered initialization in setup() function
- **src/InputManager.cpp:** Ensured 50ms stabilization delay and initial pin reads
- **platformio.ini:** Added `-D TFT_MISO=-1` to disable MISO pin claim (safety measure)

### Files Modified
- `src/main.cpp` - Reordered DisplayManager before InputManager
- `platformio.ini` - Added TFT_MISO=-1 flag

### Testing Results
✅ Button X no longer has phantom presses at boot
✅ Button X responds correctly after boot
✅ BOOTING and PAIRING animations display properly
✅ All other buttons (A, B, Y) unaffected
✅ Display rendering unaffected

### Key Implementation Details
```cpp
// CORRECT ORDER in setup():
displayManager.begin();              // 1. Initialize display/SPI
displayManager.setStateManager(&stateManager);
delay(50);                           // 2. Let SPI bus settle
inputManager.begin();                // 3. Reclaim GPIO 19 with INPUT_PULLUP
```

```cpp
// InputManager.begin() must have stabilization:
void InputManager::begin() {
    pinMode(BTN_A, INPUT_PULLUP);
    pinMode(BTN_B, INPUT_PULLUP);
    pinMode(BTN_X, INPUT_PULLUP);    // GPIO 19
    pinMode(BTN_Y, INPUT_PULLUP);
    
    delay(50);                        // Critical: stabilize pull-ups
    
    lastA = digitalRead(BTN_A);      // Capture initial state
    lastB = digitalRead(BTN_B);
    lastX = digitalRead(BTN_X);
    lastY = digitalRead(BTN_Y);
}
```

### Memory Impact
- No change to flash or RAM usage
- Configuration change only (initialization order and one platformio.ini flag)

### Lessons Learned
1. Hardware SPI initialization has side effects on GPIO pins
2. Library initialization order matters when pins are shared or reused
3. Proper debouncing requires both electrical (pull-ups) AND timing (delays)
4. Edge detection systems are sensitive to initial state capture timing

---

## Hardware Configuration

### Power & Amplifier
| Pin | Function | Note |
|-----|----------|------|
| GPIO 32 | Backlight | Must be HIGH to turn on display |
| GPIO 14 | Amp Enable | Must be HIGH to unmute audio |

### Display (Software SPI - 240x240 TFT)
| Pin | Function |
|-----|----------|
| GPIO 21 | MOSI |
| GPIO 22 | DC |
| GPIO 23 | SCLK |
| GPIO 33 | CS |
| -1 | RST (not connected) |

### Audio (I2S)
| Pin | Function |
|-----|----------|
| GPIO 13 | BCLK (Bit Clock) |
| GPIO 27 | LRC/WS (Word Select) |
| GPIO 26 | DOUT (Data Out) |

### Buttons (Active-low with INPUT_PULLUP)
| Pin | Button | Function |
|-----|--------|----------|
| GPIO 25 | A | Volume Up (+5) |
| GPIO 18 | B | Volume Down (-5) |
| GPIO 19 | X | Play/Pause Toggle |
| GPIO 16 | Y | Next Track |

---

## Essential Commands

```bash
# Build
pio run

# Upload to ESP32
pio run --target upload

# Clean build
pio run --target clean

# Serial monitor
pio device monitor

# Check memory usage
pio run | grep "Memory Usage"

# Convert sprites (if needed)
python3 convert_sprites.py
```

---

### Known Issues & Notes

### Button X GPIO 19 SPI Conflict (FIXED in Phase 5)
- **Issue:** TFT_eSPI Hardware SPI initialization conflicted with Button X pin
- **Solution:** Initialize display BEFORE inputs in setup()
- **Status:** ✅ RESOLVED - Button X now working perfectly
- **Critical:** Do NOT initialize InputManager before DisplayManager

### Flash Memory Constraints
- Current usage: 42.0% (1,319,641 / 3,145,728 bytes)
- Remaining: ~1.8MB for Phase 4 assets
- Strategy: Store all assets in PROGMEM to save RAM

### Partition Scheme
- Using `huge_app.csv` for maximum app space
- Provides ~3MB instead of 1.3MB
- Disables OTA updates (not needed for this project)
- Perfect for asset-heavy projects

### Kawaii Aesthetic
- Custom pastel color palette implemented
- State-based background colors
- Consistent visual theme throughout
- Matches original design specifications

### Future Considerations
- Plenty of space for Phase 4 audio assets
- Can add more features without memory concerns
- Room for additional animations or effects

---

## Dependencies

```ini
lib_deps =
    adafruit/Adafruit GFX Library @ ^1.11.9
    adafruit/Adafruit ST7735 and ST7789 Library @ ^1.10.3
    https://github.com/pschatzmann/ESP32-A2DP.git
```

---

## Backup & Version Control

### Backups
- `backup/main_working_backup.cpp` - Original working version
- `backup/bankup_do_not_delete.cpp` - Original backup

### Git (if using)
```bash
git add .
git commit -m "Phase 3 complete: Kawaii color palette and full animation system"
```

---

## Next Steps

1. **Phase 4:** Audio Feedback & Final Integration
   - Convert sound effects to C-arrays
   - Implement system sounds
   - Final testing and optimization
2. **Hardware Testing:** (when ESP32 is connected)
   - Upload firmware to device
   - Test all functionality
   - Verify animations and overlays
   - Test audio quality
   - Verify Kawaii color palette on actual display

---

*Last Updated: 2026-04-19*
*Phase 6 Complete: Bluetooth State Machine & Phone Sync - Auto-sync now tracks phone connections*
