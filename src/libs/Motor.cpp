// Ultrasonic Sensors
#include "Arduino.h"
#include "Motor.h"

Motor::Motor() {}

Motor::Motor(int enable, int in1, int in2)
{
    pinMode(enable, OUTPUT); // Sets the trigPin as an Output
    pinMode(in1, OUTPUT);    // Sets the trigPin as an Output
    pinMode(in2, OUTPUT);    // Sets the trigPin as an Output

    _enable = enable;
    _in1 = in1;
    _in2 = in2;

    stop();
}

void Motor::stop()
{
    digitalWrite(_in1, HIGH);
    digitalWrite(_in2, HIGH);
    digitalWrite(_enable, HIGH);
}

void Motor::down()
{
    digitalWrite(_enable, HIGH);
    delay(100);
    digitalWrite(_in1, HIGH);
    digitalWrite(_in2, LOW);
    delay(100);
    digitalWrite(_enable, LOW);
}

void Motor::up()
{
    digitalWrite(_enable, HIGH);
    delay(100);
    digitalWrite(_in1, LOW);
    digitalWrite(_in2, HIGH);
    delay(100);
    digitalWrite(_enable, LOW);
}
