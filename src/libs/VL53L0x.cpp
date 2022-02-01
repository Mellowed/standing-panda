#include "VL53L0x.h"

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

VL53L0x::VL53L0x(TwoWire *i2c)
{
    _i2c = i2c;
}

boolean VL53L0x::begin()
{

    _bootOk = lox.begin((uint8_t)41U, false, _i2c);

    if (!_bootOk)
    {
        Serial.println(F("Failed to boot VL53L0X"));
        return false;
    }

    return true;
}

int VL53L0x::getDistance()
{
    // Check if the device has booted correctly
    if (!_bootOk)
    {
        Serial.println("Device not configured");
        return begin();
    }

    int avg = 0;
    int count = 0;

    // Avarage over 10 measurements.
    for (int x = 0; x < 10; x++)
    {

        VL53L0X_RangingMeasurementData_t measure;
        lox.rangingTest(&measure, false);

        if (measure.RangeStatus != 4)
        { // phase failures have incorrect data
            avg = avg + measure.RangeMilliMeter;
            count++;
        }
        else
        {
            Serial.println(" out of range ");
        }
    }

    return avg / count;
}
