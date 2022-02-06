#include "Button.h"

Button::Button(int buttonPin)
{
    pinMode(buttonPin, INPUT_PULLUP);
    _buttonPin = buttonPin;
}

boolean Button::isPressed()
{
    int reading = digitalRead(_buttonPin);
    return reading == LOW;
}

boolean Button::isReleased()
{
    int reading = digitalRead(_buttonPin);
    return reading == HIGH;
}