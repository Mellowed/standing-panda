#ifndef Motor_h
#define Motor_h

#include "Arduino.h"

class Motor
{
public:
    Motor();
    Motor(int enable, int in1, int in2);
    void up();
    void stop();
    void down();

private:
    int _enable;
    int _in1;
    int _in2;
};

#endif
