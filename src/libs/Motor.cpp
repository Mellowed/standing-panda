#include "Motor.h"

Motor::Motor() {}

Motor::Motor(int enable, int in1, int in2)
{
    pinMode(enable, OUTPUT);
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);

    _enable = enable;
    _in1 = in1;
    _in2 = in2;

    stop();
}

void Motor::enable()
{
    digitalWrite(_enable, LOW);
}

void Motor::disable()
{
    digitalWrite(_enable, HIGH);
}

void Motor::stop()
{
    digitalWrite(_in1, HIGH);
    digitalWrite(_in2, HIGH);
    disable();
}

void Motor::down()
{
    digitalWrite(_in1, HIGH);
    digitalWrite(_in2, LOW);
    enable();
}

void Motor::up()
{
    digitalWrite(_in1, LOW);
    digitalWrite(_in2, HIGH);
    enable();
}
