#ifndef VL53L0x_h
#define VL53L0x_h

#include "Arduino.h"
#include "Adafruit_VL53L0X.h"
#include <Wire.h>

class VL53L0x
{
public:
    VL53L0x(TwoWire *i2c);
    boolean begin();
    int getDistance();

private:
    TwoWire *_i2c;
    boolean _bootOk;
};

#endif
