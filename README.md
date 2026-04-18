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

### 3. Screen (Software SPI)
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
- Debouncing handled in InputManager

### Display Configuration
- Resolution: 240x240 pixels
- Interface: Software SPI
- Color format: RGB565 (16-bit)
- Backlight: Must be enabled (GPIO 32 HIGH)

### Audio Configuration
- Interface: I2S
- Sample rate: 44.1kHz
- Bit depth: 16-bit
- Channels: Stereo
- Amplifier: Must be enabled (GPIO 14 HIGH)

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
```

### Pin Definitions in Code
```cpp
// Power & Amp
#define BACKLIGHT_PIN 32
#define AMP_ENABLE_PIN 14

// Screen Pins 
#define TFT_MOSI 21
#define TFT_DC   22
#define TFT_SCLK 23
#define TFT_CS   33
#define TFT_RST  -1

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

### No Audio Output
1. Check GPIO 14 is HIGH (amp enable)
2. Verify I2S connections (BCLK, LRC, DOUT)
3. Check Bluetooth connection status

### Buttons Not Responding
1. Verify button pins are correct
2. Check INPUT_PULLUP mode is set
3. Test with multimeter (should be HIGH when not pressed)
4. Avoid strapping pins (D2, D5)

### Boot Loop Issues
1. Check if any button is stuck LOW
2. Verify no pins are connected to strapping pins
3. Check power supply stability

---

## Safety Notes

1. **Voltage Levels:** Ensure 3.3V and 5V are not mixed
2. **Current Limits:** Don't exceed ESP32 current limits
3. **Ground Connections:** All grounds must be connected together
4. **Power Supply:** Use adequate power supply (500mA+ recommended)
5. **ESD Protection:** Handle components carefully to avoid static damage

---

*Last Updated: 2026-04-18*
