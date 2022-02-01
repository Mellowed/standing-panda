// Ultrasonic Sensors
#include "UltraDist.h"

UltraDist::UltraDist(int trig, int echo)
{
    NewPing _sonar(trig, echo, 200);
    _trig = trig;
    _echo = echo;
}

float UltraDist::getDistance()
{
    return _sonar.ping_cm();
}
