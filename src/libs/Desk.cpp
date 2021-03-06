#include "Desk.h"

Desk::Desk(int enableA, int enableB, int in1, int in2, OledManager *om, VL53L0x *distanceSensor, MPU6050 *tiltSensor, WebServer *webServer, ConfigurationMemory *configurationMemory)
{
    // Set sensors and manager
    _om = om;
    _webServer = webServer;
    _distanceSensor = distanceSensor;
    _tiltSensor = tiltSensor;
    _configurationMemory = configurationMemory;

    // Set motor controls inputs
    _enableA = enableA;
    _in1 = in1;
    _in2 = in2;
    _enableB = enableB;

    // Create motor objects
    _motorLeft = Motor(enableA, in1, in2);
    _motorRight = Motor(enableB, in1, in2);

    // Setup webserver and websockets
    webServer->setWsDataCallback([&](String data)
                                 { websocketHandler(data); });

    // Initialize State
    _targetHeight = 0;

    _configurationMemory->setNamespace("cp");
}

// We cannot updated persisted config within the constructor as nvs is not available immediately,.
void Desk::readPersistedConfig()
{
    // Preset levels
    _presetStandingHeight1 = _configurationMemory->getInt(_PRESET_HEIGHT1, 80);
    _presetStandingHeight2 = _configurationMemory->getInt(_PRESET_HEIGHT2, 90);
    _presetStandingHeight3 = _configurationMemory->getInt(_PRESET_HEIGHT3, 100);
    _presetStandingHeight4 = _configurationMemory->getInt(_PRESET_HEIGHT4, 110);

    // Limits and offsets
    _rollOffset = _configurationMemory->getInt(_ROLL_OFFSET, 180);
    _rollLimit = _configurationMemory->getInt(_ROLL_LIMIT, 2);
    _pitchOffset = _configurationMemory->getInt(_PITCH_OFFSET, 1);
    _pitchLimit = _configurationMemory->getInt(_PITCH_LIMIT, 0);

    // Safety checks
    _safetyEnabled = _configurationMemory->getInt(_SAFETY_ENABLED);

    // Max, Min heights
    _seatedHeight = _configurationMemory->getInt(_MIN_HEIGHT, 70);
    _fullStandingHeight = _configurationMemory->getInt(_MAX_HEIGHT, 120);
}

// Go to a defined height
// Stop to check levels and perform safety checks
void Desk::goToHeight(int requiredHeight)
{

    _stop = false;
    _targetHeight = requiredHeight;

    Serial.print("Going to ");
    Serial.println(_targetHeight);

    updateSensorReadings();
    _startTime = millis();
    safetyChecks();

    // Do we go up or down?
    if ((_currentHeight) <= _targetHeight)
    {
        // Go up and monitor safety check
        while ((_currentHeight < _targetHeight) && _safetyChecksPassed && !_stop)
        {
            up();
            safetyChecks();
        }
    }
    else
    {
        // Go down and monitor safety check
        while ((_currentHeight > _targetHeight) && _safetyChecksPassed && !_stop)
        {
            down();
            safetyChecks();
        }
    }

    stop();

    // If there was an issue then expose it to the user
    if (!_safetyChecksPassed)
    {
        if (_calibrationError)
        {
            String errorMsg = ("Error - Roll: " + String(_roll) + " | Pitch: " + String(_pitch) + "");
            _om->pandaError(errorMsg);
        }
        if (_timedOut)
        {
            _om->pandaError("Error: 2min timeout");
        }

        delay(10000);
    }

    // If exited by stop call then inform the user
    if (!_stop)
    {
        Serial.println("Stopped on user request");
        _webServer->notifyClients("INFO: Stopped by user");
    }
}

void Desk::calibrateLevels(int calibrationHeight)
{
    _targetHeight = calibrationHeight;
    _stop = false;
    updateSensorReadings();
    goToHeight(calibrationHeight - 2);
    goToHeight(calibrationHeight);
    while (abs(_roll) != 0 && !_stop)
    {
        // If roll is positive then then table high on left and low on right. (Leaning right)
        if (_roll < 0)
        {
            // lower left by 2mm
            leftDown();
            delay(500);
            stop();
        }
        else
        {
            // lower right by 2mm
            rightDown();
            delay(500);
            stop();
        }
    }

    // If exited by stop call then inform the user
    if (!_stop)
    {
        Serial.println("Stopped on user request");
        _webServer->notifyClients("INFO: Stopped by user");
    }
}

// Single motor operations
// Perform movement and update heights

void Desk::rightUp()
{
    updateSensorReadings();
    _motorRight.up();
    _motorRight.enable();
    String message = "Up to " + String(_targetHeight) + "cm";
    _om->goingUp(_currentHeight, _roll, message);
}

void Desk::leftUp()
{
    updateSensorReadings();
    _motorLeft.up();
    _motorLeft.enable();
    String message = "Up " + String(_targetHeight) + "cm";
    _om->goingUp(_currentHeight, _roll, message);
}

void Desk::rightDown()
{
    updateSensorReadings();
    _motorRight.down();
    _motorRight.enable();
    String message = "Down to " + String(_targetHeight) + "cm";
    _om->goingDown(_currentHeight, _roll, message);
}

void Desk::leftDown()
{
    updateSensorReadings();
    _motorLeft.down();
    _motorLeft.enable();
    String message = "Down to " + String(_targetHeight) + "cm";
    _om->goingDown(_currentHeight, _roll, message);
}

// General motor operations

void Desk::up()
{
    updateSensorReadings();
    _motorLeft.up();
    _motorRight.up();
    _motorLeft.enable();
    _motorRight.enable();
    String message = "Up to " + String(_targetHeight) + "cm";
    _om->goingUp((_currentHeight), _roll, message);
}

void Desk::up(int ms)
{
    updateSensorReadings();
    _motorLeft.up();
    _motorRight.up();
    _motorLeft.enable();
    _motorRight.enable();
    String message = "Up by " + String(ms) + "cm";
    _om->goingUp((_currentHeight), _roll, message);
    delay(ms);
    _motorRight.disable();
}

void Desk::down()
{
    updateSensorReadings();
    _motorLeft.down();
    _motorRight.down();
    _motorLeft.enable();
    _motorRight.enable();
    String message = "Down to " + String(_targetHeight) + "cm";
    _om->goingDown(_currentHeight, _roll, message);
}

void Desk::down(int ms)
{
    updateSensorReadings();
    _motorLeft.down();
    _motorRight.down();
    _motorLeft.enable();
    _motorRight.enable();
    String message = "Down by " + String(ms) + "cm";
    _om->goingDown(_currentHeight, _roll, message);
    delay(ms);
    _motorRight.disable();
}

void Desk::stop()
{
    updateSensorReadings();
    _motorLeft.stop();
    _motorRight.stop();
    _om->panda("@" + String(_currentHeight, 1) + "cm");
}

void Desk::bounce(int duration)
{
    up();
    delay(duration);
    down();
    delay(duration);
    stop();
}

// Distance

boolean Desk::safetyChecks()
{

    // Initialize values on each check
    _safetyChecksPassed = true;
    _calibrationError = false;
    _timedOut = false;

    if (_stop)
    {
        _webServer->notifyClients("ERROR: Stopped by user");
        return false;
    }

    if (!_safetyEnabled)
    {
        return true;
    }

    // fail if desk is leaning left or right
    if (abs(_roll) > (_rollLimit + _rollOffset))
    {
        _safetyChecksPassed = false;
        _calibrationError = true;
        _webServer->notifyClients("ERROR: Desk is leaning");
    }

    // fail if desk is tilting forward or backwards
    if (abs(_pitch) > (_pitchLimit + _pitchOffset))
    {
        _safetyChecksPassed = false;
        _calibrationError = true;
        _webServer->notifyClients("ERROR: Desk is tiling");
    }

    // Timeout after 2m
    if ((millis() - _startTime) > 120000)
    {
        _safetyChecksPassed = false;
        _timedOut = true;
        _webServer->notifyClients("ERROR: There was a timeout...");
    }

    // TODO check if the motors are moving as expected (5mm a second)
    return _safetyChecksPassed;
}

void Desk::updateSensorReadings()
{
    _currentHeight = _distanceSensor->getDistance() / 10;
    _tiltSensor->updateReadings();

    _roll = _tiltSensor->_roll;
    _pitch = _tiltSensor->_pitch;

    messageClient();
}

void Desk::messageClient()
{

    _wsData["sensors"]["height"] = _currentHeight * 10;
    _wsData["sensors"]["acceleration"]["x"] = _tiltSensor->_acceleration.acceleration.x;
    _wsData["sensors"]["acceleration"]["y"] = _tiltSensor->_acceleration.acceleration.y;
    _wsData["sensors"]["acceleration"]["z"] = _tiltSensor->_acceleration.acceleration.z;
    _wsData["sensors"]["gyro"]["x"] = _tiltSensor->_gyro.gyro.x;
    _wsData["sensors"]["gyro"]["y"] = _tiltSensor->_gyro.gyro.y;
    _wsData["sensors"]["gyro"]["z"] = _tiltSensor->_gyro.gyro.z;
    _wsData["sensors"]["pitch"] = _tiltSensor->_pitch;
    _wsData["sensors"]["roll"] = _tiltSensor->_roll;
    _wsData["sensors"]["temp"] = _tiltSensor->_temp.temperature;
    _wsData["config"]["preset1"] = _presetStandingHeight1;
    _wsData["config"]["preset2"] = _presetStandingHeight2;
    _wsData["config"]["preset3"] = _presetStandingHeight3;
    _wsData["config"]["preset4"] = _presetStandingHeight4;
    _wsData["config"]["rollOffset"] = _rollOffset;
    _wsData["config"]["rollLimit"] = _rollLimit;
    _wsData["config"]["pitchOffset"] = _pitchOffset;
    _wsData["config"]["pitchLimit"] = _pitchLimit;
    _wsData["config"]["safetyEnabled"] = _safetyEnabled;
    _wsData["config"]["rollOffset"] = _rollOffset;

    String msg = "";

    serializeJson(_wsData, msg);

    _webServer->notifyClients(msg);
}

// Collect and process the websocket data. If valid set the _wsRequestReady to true.
// We try to do as little work as possible to avoid a watch dog reset by the webserver.
void Desk::websocketHandler(String data)
{
    // Sanity print of received data
    Serial.println("Handling request:");
    Serial.println(data);

    // Stop all movements before accepting next request.
    _stop = true;

    // Deserialize the json
    DeserializationError err = deserializeJson(_wsRequest, data);

    // Check for errors : notify client if bad data was received.
    if (err)
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(err.c_str());
        _webServer->notifyClients("Bad JSON Data Received");
        _wsRequestReady = false;
        return;
    }
    else
    {
        _wsRequestReady = true;
    }
}

void Desk::setEmergencyStop()
{
    Serial.println("SETTING STOP TO TRUE");
    _stop = true;
}

// '{"reqType":0,"data":{"type":0,"parameters":"somedata"}}'

void Desk::processWsRequest()
{

    switch ((int)_wsRequest["reqType"])
    {
    case wsMessageTypes::COMMAND:
        processCommandRequest();
        serializeJson(_wsRequest, Serial);
        break;

    case wsMessageTypes::CONFIGURE:
        processConfigurationRequest();
        serializeJson(_wsRequest, Serial);
        break;

    default:
        Serial.println("unexpected data type: ");
        serializeJson(_wsRequest, Serial);
        break;
    }

    _wsRequestReady = false;
}

void Desk::processConfigurationRequest()
{
    switch ((int)_wsRequest["data"]["type"])
    {

    case wsMessageCommands::PRESET_HEIGHT1:
        _presetStandingHeight1 = _wsRequest["data"]["parameters"].as<int>();
        _configurationMemory->setInt(_PRESET_HEIGHT2, _presetStandingHeight1);
        break;

    case wsMessageCommands::PRESET_HEIGHT2:
        _presetStandingHeight2 = _wsRequest["data"]["parameters"].as<int>();
        _configurationMemory->setInt(_PRESET_HEIGHT2, _presetStandingHeight2);
        break;

    case wsMessageCommands::PRESET_HEIGHT3:

        _presetStandingHeight3 = _wsRequest["data"]["parameters"].as<int>();
        _configurationMemory->setInt(_PRESET_HEIGHT3, _presetStandingHeight3);
        break;

    case wsMessageCommands::PRESET_HEIGHT4:
        _presetStandingHeight4 = _wsRequest["data"]["parameters"].as<int>();
        _configurationMemory->setInt(_PRESET_HEIGHT4, _presetStandingHeight4);
        break;

    case wsMessageCommands::ROLL_OFFSET:
        _rollOffset = _wsRequest["data"]["parameters"].as<int>();
        _configurationMemory->setInt(_ROLL_OFFSET, _rollOffset);
        break;

    case wsMessageCommands::ROLL_LIMIT:
        _rollLimit = _wsRequest["data"]["parameters"].as<int>();
        _configurationMemory->setInt(_ROLL_LIMIT, _rollLimit);
        break;

    case wsMessageCommands::PITCH_OFFSET:

        _pitchOffset = _wsRequest["data"]["parameters"].as<int>();
        _configurationMemory->setInt(_PITCH_OFFSET, _pitchOffset);
        break;

    case wsMessageCommands::PITCH_LIMIT:
        _pitchLimit = _wsRequest["data"]["parameters"].as<int>();
        _configurationMemory->setInt(_PITCH_LIMIT, _pitchLimit);
        break;

    case wsMessageCommands::SAFETY_ENABLED:
        _safetyEnabled = _wsRequest["data"]["parameters"].as<int>();
        _configurationMemory->setInt(_SAFETY_ENABLED, _safetyEnabled);
        break;

    case wsMessageCommands::MIN_HEIGHT:
        _seatedHeight = _wsRequest["data"]["parameters"].as<int>();
        _configurationMemory->setInt(_MIN_HEIGHT, _seatedHeight);
        break;

    case wsMessageCommands::MAX_HEIGHT:
        _fullStandingHeight = _wsRequest["data"]["parameters"].as<int>();
        _configurationMemory->setInt(_MIN_HEIGHT, _fullStandingHeight);
        break;
    }
}

void Desk::processCommandRequest()
{

    switch ((int)_wsRequest["data"]["type"])
    {
    case wsMessageCommands::SEATED:
        goToHeight(_seatedHeight);
        break;

    case wsMessageCommands::STANDING:
        goToHeight(_fullStandingHeight);
        break;

    case wsMessageCommands::CALIBRATE:
        calibrateLevels(_wsRequest["data"]["parameters"].as<int>());
        break;

    case wsMessageCommands::GO_TO_HEIGHT:
        goToHeight(_wsRequest["data"]["parameters"].as<int>());
        break;

    case wsMessageCommands::CLIMB_FOR_MS:
        up(_wsRequest["data"]["parameters"].as<int>());
        break;

    case wsMessageCommands::DESCEND_FOR_MS:
        down(_wsRequest["data"]["parameters"].as<int>());
        break;

    case wsMessageCommands::PRESET_HEIGHT1:
        goToHeight(_presetStandingHeight1);
        break;

    case wsMessageCommands::PRESET_HEIGHT2:
        goToHeight(_presetStandingHeight2);
        break;

    case wsMessageCommands::PRESET_HEIGHT3:
        goToHeight(_presetStandingHeight3);
        break;

    case wsMessageCommands::PRESET_HEIGHT4:
        goToHeight(_presetStandingHeight4);
        break;

    case wsMessageCommands::STOP:
        setEmergencyStop();
        break;

    default:
        Serial.println("unexpected command type: ");
        serializeJson(_wsRequest, Serial);
        break;
    }
}