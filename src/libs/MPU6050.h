#ifndef MPU6050_h
#define MPU6050_h

#include "Arduino.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

class MPU6050
{
public:
    MPU6050(TwoWire *i2c);
    boolean begin();
    void updateReadings();
    void printReadings();

    sensors_event_t _acceleration, _gyro, _temp;
    double _roll, _pitch;

private:
    TwoWire *_i2c;
    boolean _bootOk;
};

#endif
