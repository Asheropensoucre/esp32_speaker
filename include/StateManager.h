#ifndef STATE_MANAGER_H
#define STATE_MANAGER_H

/**
 * StateManager - Defines the state machine for Sir Potato OS
 * 
 * States represent the different operating modes of the system:
 * - BOOTING: Initial startup sequence
 * - PAIRING: Waiting for Bluetooth connection
 * - PLAYING: Music is actively playing
 * - PAUSED: Music is paused
 */

enum SystemState {
  BOOTING,    // System initializing, playing boot animation
  PAIRING,    // Waiting for Bluetooth connection
  PLAYING,    // Music is playing, showing dance animation
  PAUSED      // Music is paused, showing idle animation
};

// Animation frame sequences for each state
enum AnimationState {
  BOOT_SEQUENCE,      // Frames [1, 7, 14, 16]
  PAIRING_SEARCH,     // Frames [5, 2, 6, 9] looping
  PLAYING_DANCE,      // Frames [3, 7, 9, 10, 14, 15] looping
  PAUSED_NAP,         // Frames [4, 8, 12] looping
  BUTTON_REACTION     // Frames [5, 9, 13] one-time
};

class StateManager {
public:
  StateManager();
  
  // Get current state
  SystemState getState();
  
  // Set current state
  void setState(SystemState newState);
  
  // Check if in a specific state
  bool isState(SystemState state);
  
  // Get state name as string (for debugging)
  const char* getStateName();
  
  // Animation management
  int getCurrentAnimationFrame();
  void advanceAnimationFrame();
  void resetAnimation();
  AnimationState getAnimationState();
  void setAnimationState(AnimationState animState);
  bool isAnimationComplete();
  void triggerButtonReaction();

private:
  SystemState currentState;
  AnimationState currentAnimation;
  int currentFrame;
  int animationStep;
  bool animationComplete;
  
  // Animation frame sequences (0-indexed)
  static const int bootSequence[];
  static const int pairingSequence[];
  static const int playingSequence[];
  static const int pausedSequence[];
  static const int buttonSequence[];
  
  static const int bootSequenceLength = 4;
  static const int pairingSequenceLength = 4;
  static const int playingSequenceLength = 6;
  static const int pausedSequenceLength = 3;
  static const int buttonSequenceLength = 3;
};

#endif // STATE_MANAGER_H
