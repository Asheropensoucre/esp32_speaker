#include "InputManager.h"

InputManager::InputManager() {}

void InputManager::begin() {
    pinMode(BTN_A, INPUT_PULLUP);
    pinMode(BTN_B, INPUT_PULLUP);
    pinMode(BTN_X, INPUT_PULLUP);
    pinMode(BTN_Y, INPUT_PULLUP);
    
    delay(50); // Let pull-up resistors stabilize
    
    lastA = digitalRead(BTN_A);
    lastB = digitalRead(BTN_B);
    lastX = digitalRead(BTN_X);
    lastY = digitalRead(BTN_Y);
}

void InputManager::update() {
    if ((millis() - lastDebounceTime) > debounceDelay) {
        bool currentA = digitalRead(BTN_A);
        bool currentB = digitalRead(BTN_B);
        bool currentX = digitalRead(BTN_X);
        bool currentY = digitalRead(BTN_Y);

        if (currentA == LOW && lastA == HIGH) flagA = true;
        if (currentB == LOW && lastB == HIGH) flagB = true;
        if (currentX == LOW && lastX == HIGH) flagX = true;
        if (currentY == LOW && lastY == HIGH) flagY = true;

        lastA = currentA;
        lastB = currentB;
        lastX = currentX;
        lastY = currentY;

        lastDebounceTime = millis();
    }
}

bool InputManager::isButtonAPressed() { if(flagA) { flagA = false; return true; } return false; }
bool InputManager::isButtonBPressed() { if(flagB) { flagB = false; return true; } return false; }
bool InputManager::isButtonXPressed() { if(flagX) { flagX = false; return true; } return false; }
bool InputManager::isButtonYPressed() { if(flagY) { flagY = false; return true; } return false; }
