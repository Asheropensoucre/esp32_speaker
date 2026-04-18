#include "StateManager.h"

// Animation frame sequences (0-indexed)
const int StateManager::bootSequence[] = {0, 6, 13, 15};  // Frames 1, 7, 14, 16
const int StateManager::pairingSequence[] = {4, 1, 5, 8};  // Frames 5, 2, 6, 9
const int StateManager::playingSequence[] = {2, 6, 8, 9, 13, 14};  // Frames 3, 7, 9, 10, 14, 15
const int StateManager::pausedSequence[] = {3, 7, 11};  // Frames 4, 8, 12
const int StateManager::buttonSequence[] = {4, 8, 12};  // Frames 5, 9, 13

StateManager::StateManager() {
  currentState = BOOTING;
  currentAnimation = BOOT_SEQUENCE;
  currentFrame = 0;
  animationStep = 0;
  animationComplete = false;
}

SystemState StateManager::getState() {
  return currentState;
}

void StateManager::setState(SystemState newState) {
  currentState = newState;
  
  // Set appropriate animation for the new state
  switch (newState) {
    case BOOTING:
      setAnimationState(BOOT_SEQUENCE);
      break;
    case PAIRING:
      setAnimationState(PAIRING_SEARCH);
      break;
    case PLAYING:
      setAnimationState(PLAYING_DANCE);
      break;
    case PAUSED:
      setAnimationState(PAUSED_NAP);
      break;
  }
  
  resetAnimation();
}

bool StateManager::isState(SystemState state) {
  return currentState == state;
}

const char* StateManager::getStateName() {
  switch (currentState) {
    case BOOTING:
      return "BOOTING";
    case PAIRING:
      return "PAIRING";
    case PLAYING:
      return "PLAYING";
    case PAUSED:
      return "PAUSED";
    default:
      return "UNKNOWN";
  }
}

// Animation management
int StateManager::getCurrentAnimationFrame() {
  const int* sequence;
  int length;
  
  switch (currentAnimation) {
    case BOOT_SEQUENCE:
      sequence = bootSequence;
      length = bootSequenceLength;
      break;
    case PAIRING_SEARCH:
      sequence = pairingSequence;
      length = pairingSequenceLength;
      break;
    case PLAYING_DANCE:
      sequence = playingSequence;
      length = playingSequenceLength;
      break;
    case PAUSED_NAP:
      sequence = pausedSequence;
      length = pausedSequenceLength;
      break;
    case BUTTON_REACTION:
      sequence = buttonSequence;
      length = buttonSequenceLength;
      break;
    default:
      return 0;
  }
  
  if (animationStep >= length) {
    return sequence[length - 1];
  }
  
  return sequence[animationStep];
}

void StateManager::advanceAnimationFrame() {
  const int* sequence;
  int length;
  
  switch (currentAnimation) {
    case BOOT_SEQUENCE:
      sequence = bootSequence;
      length = bootSequenceLength;
      break;
    case PAIRING_SEARCH:
      sequence = pairingSequence;
      length = pairingSequenceLength;
      break;
    case PLAYING_DANCE:
      sequence = playingSequence;
      length = playingSequenceLength;
      break;
    case PAUSED_NAP:
      sequence = pausedSequence;
      length = pausedSequenceLength;
      break;
    case BUTTON_REACTION:
      sequence = buttonSequence;
      length = buttonSequenceLength;
      break;
    default:
      return;
  }
  
  animationStep++;
  
  // Check if animation is complete
  if (animationStep >= length) {
    // For looping animations, reset
    if (currentAnimation == PAIRING_SEARCH || 
        currentAnimation == PLAYING_DANCE || 
        currentAnimation == PAUSED_NAP) {
      animationStep = 0;
    } else {
      animationComplete = true;
      animationStep = length - 1;
    }
  }
}

void StateManager::resetAnimation() {
  animationStep = 0;
  animationComplete = false;
}

AnimationState StateManager::getAnimationState() {
  return currentAnimation;
}

void StateManager::setAnimationState(AnimationState animState) {
  currentAnimation = animState;
  resetAnimation();
}

bool StateManager::isAnimationComplete() {
  return animationComplete;
}

void StateManager::triggerButtonReaction() {
  // Set to button reaction animation
  setAnimationState(BUTTON_REACTION);
  
  // The main loop will check animationComplete and restore
  // the appropriate animation for the current state
}
