#include "MPU6050.h"

Adafruit_MPU6050 mpu;

MPU6050::MPU6050(TwoWire *i2c)
{
    _i2c = i2c;
}

boolean MPU6050::begin()
{
    Serial.println("Booting MPU");
    _bootOk = mpu.begin((uint8_t)104U, _i2c);

    if (!_bootOk)
    {
        Serial.println(F("Failed to boot MPU6050"));
        return false;
    }

    Serial.println("MPU6050 Boot Ok...");

    mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
    mpu.setGyroRange(MPU6050_RANGE_250_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

    return true;
}

void MPU6050::printReadings()
{

    updateReadings();
    /* Print out the values */
    Serial.print("Acceleration X: ");
    Serial.print(_acceleration.acceleration.x);
    Serial.print(", Y: ");
    Serial.print(_acceleration.acceleration.y);
    Serial.print(", Z: ");
    Serial.print(_acceleration.acceleration.z);
    Serial.println(" m/s^2");

    Serial.print("Rotation X: ");
    Serial.print(_gyro.gyro.x);
    Serial.print(", Y: ");
    Serial.print(_gyro.gyro.y);
    Serial.print(", Z: ");
    Serial.print(_gyro.gyro.z);
    Serial.println(" rad/s");

    Serial.print("Temperature: ");
    Serial.print(_temp.temperature);
    Serial.println(" degC");
}

void MPU6050::updateReadings()
{
    // Check if the device has booted correctly
    if (!_bootOk)
    {
        Serial.println("Device not configured");
    }

    // update the readings
    mpu.getEvent(&_acceleration, &_gyro, &_temp);

    // Calculate the roll & pitch
    double x_Buff = float(_acceleration.acceleration.x);
    double y_Buff = float(_acceleration.acceleration.y);
    double z_Buff = float(_acceleration.acceleration.z);
    _roll = atan2(y_Buff, z_Buff) * 57.3;
    _pitch = atan2((-x_Buff), sqrt(y_Buff * y_Buff + z_Buff * z_Buff)) * 57.3;
}
