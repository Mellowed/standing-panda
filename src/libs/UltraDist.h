#ifndef UltraDist_h
#define UltraDist_h

#include "Arduino.h"
#include <NewPing.h>

#define SOUND_SPEED 0.034

class UltraDist
{
public:
    UltraDist(int trig, int echo);
    float getDistance();

private:
    int _trig;
    int _echo;
    NewPing _sonar = NewPing(1, 1, 200);
};

#endif
