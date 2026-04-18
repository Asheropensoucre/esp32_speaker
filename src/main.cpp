#include <Arduino.h>
#include "StateManager.h"
#include "DisplayManager.h"
#include "AudioManager.h"
#include "InputManager.h"
#include "Assets.h"

// Power & Amp pins
#define BACKLIGHT_PIN 32
#define AMP_ENABLE_PIN 14

// Global manager objects
StateManager stateManager;
DisplayManager displayManager;
AudioManager* audioManager;
InputManager inputManager;

// Animation timing
unsigned long lastAnimationUpdate = 0;
const unsigned long ANIMATION_SPEED = 150; // ms between frames

void setup() {
  // Initialize Serial for debugging
  Serial.begin(115200);
  delay(100);
  Serial.println("Sir Potato OS - Booting...");
  
  // Initialize power and amplifier
  pinMode(BACKLIGHT_PIN, OUTPUT);
  digitalWrite(BACKLIGHT_PIN, HIGH);
  pinMode(AMP_ENABLE_PIN, OUTPUT);
  digitalWrite(AMP_ENABLE_PIN, HIGH);
  delay(200);

  // Initialize all managers
  inputManager.begin();
  displayManager.begin();
  
  // Wire up the state manager reference to display manager
  displayManager.setStateManager(&stateManager);
  
  // Create audio manager (needs display manager reference)
  audioManager = new AudioManager(&displayManager);
  audioManager->begin();

  // Start in BOOTING state
  stateManager.setState(BOOTING);
  
  // Draw initial boot screen with Kawaii colors
  displayManager.drawFullDisplay();

  // Start Bluetooth A2DP sink
  audioManager->start("ESP32 Potato");

  // Transition to pairing state after boot
  delay(1000);
  stateManager.setState(PAIRING);
}

void loop() {
  inputManager.update();
  
  // Update animations
  unsigned long currentTime = millis();
  if (currentTime - lastAnimationUpdate >= ANIMATION_SPEED) {
    lastAnimationUpdate = currentTime;
    
    // Advance animation frame
    stateManager.advanceAnimationFrame();
    
    // Check if button reaction animation is complete
    if (stateManager.isAnimationComplete()) {
      // Return to the appropriate animation for current state
      AnimationState animState;
      switch (stateManager.getState()) {
        case BOOTING: animState = BOOT_SEQUENCE; break;
        case PAIRING: animState = PAIRING_SEARCH; break;
        case PLAYING: animState = PLAYING_DANCE; break;
        case PAUSED: animState = PAUSED_NAP; break;
        default: animState = BOOT_SEQUENCE; break;
      }
      stateManager.setAnimationState(animState);
    }
    
    // Redraw full display with new animation frame
    displayManager.drawFullDisplay();
  }
  
  // Update overlays
  displayManager.updateOverlay();
  
  // Handle button inputs - non-blocking with proper audio control
  if (inputManager.isButtonXPressed() && audioManager) {
    Serial.println("Button X Pressed - Play/Pause");
    // Button X: PLAY / PAUSE - toggle playback state
    BluetoothA2DPSink* sink = audioManager->getSink();
    if (audioManager->isPlaying()) {
      sink->pause();
      audioManager->pause();
      stateManager.setState(PAUSED);
      displayManager.showOverlay("PAUSED", COLOR_KAWAII_TEAL);
    } else {
      sink->play();
      audioManager->play();
      stateManager.setState(PLAYING);
      displayManager.showOverlay("PLAYING", COLOR_KAWAII_PINK);
    }
    displayManager.drawFullDisplay();
  }

  if (inputManager.isButtonYPressed() && audioManager) {
    Serial.println("Button Y Pressed - Next Track");
    // Button Y: NEXT TRACK - skip to next track
    BluetoothA2DPSink* sink = audioManager->getSink();
    sink->next();
    audioManager->next();
    displayManager.showOverlay("NEXT", COLOR_KAWAII_YELLOW);
    stateManager.triggerButtonReaction();
    displayManager.drawFullDisplay();
  }

  if (inputManager.isButtonAPressed() && audioManager) {
    Serial.println("Button A Pressed - Volume Up");
    // Button A: VOLUME UP - increase volume in steps of 5
    BluetoothA2DPSink* sink = audioManager->getSink();
    sink->volume_up();
    audioManager->volumeUp();
    displayManager.showOverlay("VOL UP", COLOR_KAWAII_YELLOW);
    stateManager.triggerButtonReaction();
    displayManager.drawFullDisplay();
  }

  if (inputManager.isButtonBPressed() && audioManager) {
    Serial.println("Button B Pressed - Volume Down");
    // Button B: VOLUME DOWN - decrease volume in steps of 5
    BluetoothA2DPSink* sink = audioManager->getSink();
    sink->volume_down();
    audioManager->volumeDown();
    displayManager.showOverlay("VOL DN", COLOR_KAWAII_YELLOW);
    stateManager.triggerButtonReaction();
    displayManager.drawFullDisplay();
  }
  
}
