#ifndef Desk_h
#define Desk_h

#include "Arduino.h"
#include "WebServer.h"
#include <ArduinoJson.h>

// Internal libraries
#include "./Motor.h"
#include "./OledManager.h"
#include "./VL53L0x.h"
#include "./MPU6050.h"
#include "./ConfigurationMemory.h"

class Desk
{
public:
    Desk(int enableA, int enableB, int in1, int in2, int in3, int in4, OledManager *oledManager, VL53L0x *distanceSensor, MPU6050 *tiltSensor, WebServer *webServer, ConfigurationMemory *configurationMemory);
    void bounce(int duration);
    void up();
    void down();
    void stop();
    void leftUp();
    void rightUp();
    void leftDown();
    void rightDown();
    void setEmergencyStop();
    void goToHeight(int requiredHeight);
    void calibrateLevels(int calibrationHeight);
    void updateSensorReadings();
    void messageClient();
    boolean safetyChecks();
    void readPersistedConfig();
    void processWsRequest();            // process the request from client
    void processCommandRequest();       // handle requests command coming from client
    void processConfigurationRequest(); // handle requests command coming from client
    void setConfigurationItem(int &localVal, int newValue, String nameSpace);
    // Websocket Data
    // Find the required sizing here; https://arduinojson.org/v6/assistant/
    StaticJsonDocument<1024> _wsData;
    StaticJsonDocument<1024> _wsRequest;
    boolean _wsRequestReady = false;
    volatile boolean _stop = false;

    // heights
    float _currentHeight;
    int _targetHeight;
    int _seatedHeight = 70;
    int _fullStandingHeight = 112;
    int _presetStandingHeight1 = 80;
    int _presetStandingHeight2 = 90;
    int _presetStandingHeight3 = 100;
    int _presetStandingHeight4 = 120;

    // tilt and angles
    int _rollLimit = 1;
    int _pitchLimit = 1;
    int _rollOffset = 0;
    int _pitchOffset = 0;
    float _pitch; // + forard -down back
    float _roll;  // neg when down right | positive when leaning left (holes closest to me)

    // Safety
    int _safetyEnabled = 1;

    // Configuration Spaces
    const String _PRESET_HEIGHT1 = "PH1";
    const String _PRESET_HEIGHT2 = "PH2";
    const String _PRESET_HEIGHT3 = "PH3";
    const String _PRESET_HEIGHT4 = "PH4";
    const String _ROLL_OFFSET = "ROS";
    const String _ROLL_LIMIT = "RL";
    const String _PITCH_OFFSET = "POS";
    const String _PITCH_LIMIT = "POS";
    const String _SAFETY_ENABLED = "SE";
    const String _MIN_HEIGHT = "MinH";
    const String _MAX_HEIGHT = "MaxH";

private:
    WebServer *_webServer;
    OledManager *_om;
    VL53L0x *_distanceSensor;
    MPU6050 *_tiltSensor;
    ConfigurationMemory *_configurationMemory;

    typedef enum
    {
        COMMAND,
        CONFIGURE,

    } wsMessageTypes;

    typedef enum
    {
        SEATED,
        STANDING,
        CALIBRATE,
        GO_TO_HEIGHT,
        PRESET_HEIGHT1,
        PRESET_HEIGHT2,
        PRESET_HEIGHT3,
        PRESET_HEIGHT4,
        ROLL_OFFSET,
        ROLL_LIMIT,
        PITCH_OFFSET,
        PITCH_LIMIT,
        SAFETY_ENABLED,
        MIN_HEIGHT,
        MAX_HEIGHT,
        STOP,
        DISPLAY_ON_OFF

    } wsMessageCommands;

    void websocketHandler(String data); // handle requests coming from client

    // Safety
    unsigned long _startTime;
    boolean _safetyChecksPassed;
    boolean _timedOut;
    boolean _calibrationError;

    // Motor A pins
    int _enableA;
    int _in1;
    int _in2;
    Motor _motorLeft;

    // Motor B pins
    int _enableB;
    int _in3;
    int _in4;
    Motor _motorRight;
};

#endif
