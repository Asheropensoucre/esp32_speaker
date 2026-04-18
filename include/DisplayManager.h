#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "StateManager.h"

/**
 * DisplayManager - Handles all TFT display operations
 * 
 * Manages the 240x240 TFT screen with:
 * - Song title display
 * - Volume popup overlay
 * - Boot screen
 * - Ready to pair screen
 * - Sprite animations (Phase 2)
 * - Display zones (Phase 2)
 * 
 * Uses TFT_eSPI with hardware SPI (pins configured in platformio.ini):
 * - MOSI: GPIO 21
 * - DC: GPIO 22
 * - SCLK: GPIO 23
 * - CS: GPIO 33
 * - RST: -1 (not connected)
 * 
 * Display Zones (Phase 2):
 * - Header (0-60px): Volume, status icons
 * - Center (60-180px): Sir Potato sprite (120x120)
 * - Footer (180-240px): Song title (marquee scrolling)
 */

// Display zone definitions
#define ZONE_HEADER_HEIGHT 60
#define ZONE_CENTER_HEIGHT 120
#define ZONE_FOOTER_HEIGHT 60
#define ZONE_CENTER_Y 60
#define ZONE_FOOTER_Y 180

// Sprite dimensions
#define SPRITE_WIDTH 120
#define SPRITE_HEIGHT 120
#define SPRITE_CENTER_X 60
#define SPRITE_CENTER_Y 60

// Kawaii Color Palette (RGB565 format)
// Converted from hex specifications
#define COLOR_KAWAII_PINK   0xF8D5  // #ffabde - Playing state
#define COLOR_KAWAII_PURPLE 0xDADF  // #dbafff - Booting state
#define COLOR_KAWAII_BLUE   0xADFF  // #afd0ff - Pairing state
#define COLOR_KAWAII_TEAL   0xAFFF  // #aafff8 - Paused state
#define COLOR_KAWAII_YELLOW 0xFFD6  // #ffffb0 - UI Overlays

class DisplayManager {
public:
  DisplayManager();
  
  // Initialize the display
  void begin();
  
  // Set state manager reference (needed for full display rendering)
  void setStateManager(StateManager* sm);
  
  // Show boot screen
  void showBootScreen();
  
  // Show ready to pair screen
  void showPairingScreen();
  
  // Update the song title display
  void updateSongTitle(const char* songTitle);
  
  // Show volume popup overlay
  void showVolume(int volume);
  
  // Get current song title
  const char* getCurrentSongTitle();
  
  // Clear screen
  void clearScreen();
  
  // Phase 2: Display zone functions
  void drawHeaderZone();
  void drawCenterZone();
  void drawFooterZone();
  
  // Phase 2: Sprite drawing
  void drawSprite(int frameIndex);
  void drawSpriteAt(int frameIndex, int x, int y);
  
  // Phase 2: Marquee text
  void updateMarquee();
  void setMarqueeText(const char* text);
  
  // Phase 2: Custom font rendering
  void drawCustomText(const char* text, int x, int y, uint16_t color);
  void drawCustomChar(char c, int x, int y, uint16_t color);
  int getTextWidth(const char* text);
  
  // Phase 3: Animation management
  void updateAnimation();
  void drawAnimatedSprite(int frameIndex);
  
  // Phase 3: Pop-up overlay system
  void showOverlay(const char* text, uint16_t color);
  void updateOverlay();
  void hideOverlay();
  bool isOverlayVisible();
  
  // Phase 3: State-based display colors
  void updateDisplayForState(SystemState state);
  uint16_t getStateColor(SystemState state);
  
  // Phase 3: Full display rendering
  void drawFullDisplay();

private:
  TFT_eSPI tft;
  
  // Current song title
  String currentSong;
  
  // Marquee scrolling state
  String marqueeText;
  int marqueeOffset;
  unsigned long lastMarqueeUpdate;
  static const unsigned long MARQUEE_SPEED = 200; // ms between updates
  
  // Animation timing
  unsigned long lastAnimationUpdate;
  static const unsigned long ANIMATION_SPEED = 150; // ms between frames
  
  // Pop-up overlay state
  bool showingOverlay;
  unsigned long overlayStartTime;
  static const unsigned long OVERLAY_DURATION = 2000; // 2 seconds
  String overlayText;
  uint16_t overlayColor;
  
  // State manager reference
  StateManager* stateManager;
  
  // Last drawn state (for flicker prevention)
  SystemState lastDrawnState;
  
  // Helper function to draw the main screen
  void drawScreen();
  
  // Helper to draw a placeholder sprite (for testing)
  void drawPlaceholderSprite(int x, int y);
};

#endif // DISPLAY_MANAGER_H
