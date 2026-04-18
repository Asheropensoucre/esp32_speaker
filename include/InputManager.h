#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H
#include <Arduino.h>

#define BTN_A 25
#define BTN_B 18
#define BTN_X 19
#define BTN_Y 16

class InputManager {
public:
    InputManager();
    void begin();
    void update();
    bool isButtonAPressed();
    bool isButtonBPressed();
    bool isButtonXPressed();
    bool isButtonYPressed();

private:
    bool lastA = HIGH, flagA = false;
    bool lastB = HIGH, flagB = false;
    bool lastX = HIGH, flagX = false;
    bool lastY = HIGH, flagY = false;
    unsigned long lastDebounceTime = 0;
    unsigned long debounceDelay = 50;
};
#endif
