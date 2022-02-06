#ifndef Button_h
#define Button_h

#include "Arduino.h"

class Button
{
public:
    Button(int _buttonPin);
    boolean isPressed();
    boolean isReleased();

private:
    int _buttonPin;
};

#endif
