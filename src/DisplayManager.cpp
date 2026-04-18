#include "DisplayManager.h"
#include "Assets.h"
#include "../test/sprite_data.h"

DisplayManager::DisplayManager() 
  : tft() {
  currentSong = "potato";
  marqueeText = "";
  marqueeOffset = 0;
  lastMarqueeUpdate = 0;
  
  lastAnimationUpdate = 0;
  showingOverlay = false;
  overlayStartTime = 0;
  overlayText = "";
  overlayColor = COLOR_KAWAII_YELLOW;
  
  stateManager = nullptr;
  lastDrawnState = BOOTING;
}

void DisplayManager::begin() {
    tft.init();
    tft.setSwapBytes(true);
    tft.setRotation(2); // 0 or 2 will be right-side up without breaking the RAM window
}

void DisplayManager::setStateManager(StateManager* sm) {
  stateManager = sm;
}

void DisplayManager::showBootScreen() {
  tft.fillScreen(COLOR_KAWAII_PURPLE);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(20, 100);
  tft.println("READY TO PAIR");
  tft.setCursor(20, 140);
  tft.println("sir potato");
}

void DisplayManager::showPairingScreen() {
  // Show pairing screen with Kawaii Blue
  tft.fillScreen(COLOR_KAWAII_BLUE);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(20, 100);
  tft.println("READY TO PAIR");
  tft.setCursor(20, 140);
  tft.println("sir potato");
}

void DisplayManager::updateSongTitle(const char* songTitle) {
  // Only update the string variables - DO NOT draw anything here
  // Drawing from BT callback thread causes FreeRTOS SPI collision
  currentSong = songTitle;
  setMarqueeText(songTitle);
}

void DisplayManager::showVolume(int volume) {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(COLOR_KAWAII_YELLOW);
  tft.setTextSize(3);
  tft.setCursor(20, 110);
  tft.print("VOL: ");
  // The ESP32 volume goes from 0 to 127. This maps it to a 0-100 percentage!
  tft.print(map(volume, 0, 127, 0, 100));
  tft.println("%");
  
  delay(800);   // Hold the volume on screen for just under a second
  drawScreen(); // Go back to the song title!
}

const char* DisplayManager::getCurrentSongTitle() {
  return currentSong.c_str();
}

void DisplayManager::clearScreen() {
  tft.fillScreen(TFT_BLACK);
}

void DisplayManager::drawScreen() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_CYAN);
  tft.setTextSize(2);
  tft.setCursor(10, 80);
  tft.println("NOW PLAYING:");
  
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(3);
  tft.setCursor(10, 110);
  tft.println(currentSong);
}

// Phase 2: Display zone functions
void DisplayManager::drawHeaderZone() {
  // Draw header background
  tft.fillRect(0, 0, 240, ZONE_HEADER_HEIGHT, TFT_BLACK);
  
  // Draw volume indicator (placeholder)
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.setCursor(5, 5);
  tft.print("VOL: --");
  
  // Draw status icon (placeholder)
  tft.setCursor(200, 5);
  tft.print("BT");
}

void DisplayManager::drawCenterZone() {
  // Draw center background
  tft.fillRect(0, ZONE_CENTER_Y, 240, ZONE_CENTER_HEIGHT, TFT_BLACK);
  
  // NOTE: The animated sprite is drawn by drawAnimatedSprite() in drawFullDisplay()
  // Do NOT draw placeholder here - it will override the real sprites!
}

void DisplayManager::drawFooterZone() {
  // Draw footer background
  tft.fillRect(0, ZONE_FOOTER_Y, 240, ZONE_FOOTER_HEIGHT, TFT_BLACK);
  
  // Draw song title using custom font
  // Center the text in the footer zone
  int textWidth = getTextWidth(currentSong.c_str());
  int x = (240 - textWidth) / 2;
  int y = ZONE_FOOTER_Y + (ZONE_FOOTER_HEIGHT - FONT_HEIGHT) / 2;
  
  // If text is too wide, truncate it
  if (textWidth > 220) {
    String truncated = currentSong.substring(0, 20);
    truncated += "...";
    drawCustomText(truncated.c_str(), 10, y, TFT_WHITE);
  } else {
    drawCustomText(currentSong.c_str(), x, y, TFT_WHITE);
  }
}

// Phase 2: Sprite drawing
void DisplayManager::drawSprite(int frameIndex) {
  drawSpriteAt(frameIndex, SPRITE_CENTER_X, SPRITE_CENTER_Y);
}

void DisplayManager::drawSpriteAt(int frameIndex, int x, int y) {
  // Use raw sprite data from spriteSheet (no RLE compression)
  if (frameIndex >= 0 && frameIndex < 16) {
    // Read the pointer from PROGMEM first!
    const uint16_t* frameData = (const uint16_t*)pgm_read_ptr(&spriteSheet[frameIndex]);
    
    // Push the raw RGB565 image data directly to the display
    // This is much faster than RLE decoding
    tft.pushImage(x, y, 120, 120, frameData);
  } else {
    // Fallback to placeholder if invalid frame
    drawPlaceholderSprite(x, y);
  }
}

// Phase 2: Marquee text
void DisplayManager::updateMarquee() {
  unsigned long currentTime = millis();
  
  if (currentTime - lastMarqueeUpdate >= MARQUEE_SPEED) {
    lastMarqueeUpdate = currentTime;
    
    if (marqueeText.length() > 15) {
      marqueeOffset++;
      if (marqueeOffset > marqueeText.length()) {
        marqueeOffset = 0;
      }
    }
  }
}

void DisplayManager::setMarqueeText(const char* text) {
  marqueeText = text;
  marqueeOffset = 0;
}

// Helper to draw a placeholder sprite (for testing)
void DisplayManager::drawPlaceholderSprite(int x, int y) {
  // Draw a simple potato-like shape
  tft.fillEllipse(x + SPRITE_WIDTH/2, y + SPRITE_HEIGHT/2, 
                  SPRITE_WIDTH/2 - 5, SPRITE_HEIGHT/2 - 5, COLOR_KAWAII_YELLOW);
  
  // Draw eyes
  tft.fillCircle(x + 40, y + 45, 8, TFT_BLACK);
  tft.fillCircle(x + 80, y + 45, 8, TFT_BLACK);
  
  // Draw mouth (simple line)
  tft.drawLine(x + 40, y + 75, x + 80, y + 75, TFT_BLACK);
}

// Phase 2: Custom font rendering
void DisplayManager::drawCustomText(const char* text, int x, int y, uint16_t color) {
  int currentX = x;
  
  for (int i = 0; text[i] != '\0'; i++) {
    drawCustomChar(text[i], currentX, y, color);
    currentX += FONT_WIDTH + 1;  // Add 1 pixel spacing
  }
}

void DisplayManager::drawCustomChar(char c, int x, int y, uint16_t color) {
  int index = getFontIndex(c);
  
  // Read font data from PROGMEM
  for (int row = 0; row < FONT_HEIGHT; row++) {
    uint8_t fontRow = pgm_read_byte(&customFont[index][row]);
    
    for (int col = 0; col < FONT_WIDTH; col++) {
      // Check if pixel is set
      if (fontRow & (0x80 >> col)) {
        tft.drawPixel(x + col, y + row, color);
      }
    }
  }
}

int DisplayManager::getTextWidth(const char* text) {
  int width = 0;
  for (int i = 0; text[i] != '\0'; i++) {
    width += FONT_WIDTH + 1;  // Character width + spacing
  }
  return width;
}

// Phase 3: Animation management
void DisplayManager::updateAnimation() {
  unsigned long currentTime = millis();
  
  if (currentTime - lastAnimationUpdate >= ANIMATION_SPEED) {
    lastAnimationUpdate = currentTime;
    // Animation frame advancement will be handled by StateManager
  }
}

void DisplayManager::drawAnimatedSprite(int frameIndex) {
  drawSprite(frameIndex);
}

// Phase 3: Pop-up overlay system
void DisplayManager::showOverlay(const char* text, uint16_t color) {
  overlayText = text;
  overlayColor = color;
  overlayStartTime = millis();
  showingOverlay = true;
}

void DisplayManager::updateOverlay() {
  if (!showingOverlay) return;
  
  unsigned long currentTime = millis();
  
  // Check if overlay duration has expired
  if (currentTime - overlayStartTime >= OVERLAY_DURATION) {
    hideOverlay();
  }
}

void DisplayManager::hideOverlay() {
  showingOverlay = false;
  overlayText = "";
}

bool DisplayManager::isOverlayVisible() {
  return showingOverlay;
}

// Phase 3: State-based display colors
void DisplayManager::updateDisplayForState(SystemState state) {
  uint16_t bgColor = getStateColor(state);
  
  // Update background color based on state
  // This will be called when state changes
  // For now, we'll just update the background
  tft.fillScreen(bgColor);
}

uint16_t DisplayManager::getStateColor(SystemState state) {
  switch (state) {
    case BOOTING:
      return COLOR_KAWAII_PURPLE;
    case PAIRING:
      return COLOR_KAWAII_BLUE;
    case PLAYING:
      return COLOR_KAWAII_PINK;
    case PAUSED:
      return COLOR_KAWAII_TEAL;
    default:
      return TFT_BLACK;
  }
}

// Phase 3: Full display rendering
void DisplayManager::drawFullDisplay() {
  // Get the appropriate background color for current state
  uint16_t bgColor = getStateColor(stateManager->getState());
  SystemState currentState = stateManager->getState();
  
  // Only fill screen if state has changed (prevents flicker)
  if (currentState != lastDrawnState) {
    tft.fillScreen(bgColor);
    lastDrawnState = currentState;
    
    // Redraw static elements when state changes
    drawHeaderZone();
    drawFooterZone();
  }
  
  // Draw the animated sprite on top (every frame)
  int frameIndex = stateManager->getCurrentAnimationFrame();
  drawAnimatedSprite(frameIndex);
  
  // Draw overlay if visible
  if (showingOverlay) {
    // Draw overlay box
    tft.fillRect(40, 90, 160, 60, TFT_BLACK);
    tft.drawRect(40, 90, 160, 60, overlayColor);
    
    // Draw overlay text centered
    tft.setTextColor(overlayColor);
    tft.setTextSize(2);
    
    // Center the text
    int textWidth = overlayText.length() * 12; // Approximate width
    int x = 120 - textWidth / 2;
    tft.setCursor(x, 115);
    tft.println(overlayText);
  }
}
