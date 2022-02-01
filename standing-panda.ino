#include <ArduinoJson.h>
#include "src/libs/OledManager.h"
#include "src/libs/WebServer.h"
#include "src/libs/VL53L0x.h"
#include "src/libs/MPU6050.h"
#include "src/libs/Desk.h"
#include "src/libs/LocalESPStorage.h"
#include "src/libs/ConfigurationMemory.h"
#include <Wire.h>
#include "Config.h"

// Motors

#define enA 0
#define in1 2
#define in2 14
#define enB 15
#define in3 26
#define in4 13

// I2c

#define sda 5
#define scl 4

// Wifi

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;

String ipPart1 = "";
String ipPart2 = "";

// Main scope variables

int loopCount = 0;
int history = 0;

// Managers

TwoWire i2c = TwoWire(0); // Must be initialized using the defined sda & scl.
OledManager om(&i2c);
LocalESPStorage localESPStorage;
WebServer webserver(ssid, password, &localESPStorage);
ConfigurationMemory configurationMemory; // Initialize the persisted configuration with namespace 'cp'
VL53L0x tof(&i2c);
MPU6050 tiltSensor(&i2c);
Desk deskControls(enA, enB, in1, in2, in3, in4, &om, &tof, &tiltSensor, &webserver, &configurationMemory);

void setup()
{
  Serial.begin(115200);
  i2c.begin(sda, scl);
  Serial.println("\n\rBooting..."); // Serial print that the system is booting
  localESPStorage.begin(false);     // Initialize the local SPIFFs storage and do not format.
  om.begin();                       // Initialize the oled manager
  om.panda("Booting...");           // Display the panda boot screen on the oled
  webserver.begin();                // Initialize the webserver
  webserver.serveWebApp("/");       // start serving the webpack
  webserver.initWebSocket();        // Initialize the websockets

  // Split the IP into 2 parts for easy visualization on the oled
  int index = webserver._ip.indexOf('.');
  index = webserver._ip.indexOf('.', index + 1);

  ipPart1 = webserver._ip.substring(0, index);
  ipPart2 = webserver._ip.substring(index + 1);

  // Display the panda home page with IP
  om.panda();
  om.printText(ipPart1.c_str(), 75, 28, 1);
  om.printText(ipPart2.c_str(), 75, 36, 1);

  tof.begin();
  tiltSensor.begin();

  deskControls.readPersistedConfig();
}

void loop()
{

  delay(100);

  // Check the height levels every 1 second.
  loopCount++;
  if (loopCount == 10)
  {
    deskControls.updateSensorReadings();
    loopCount = 0;
  }

  // If someone is connected then print 'Hi' else print the IP address.
  // We need to prevent the Oled from constantly updating, so we use a *history variable.
  if (webserver._connectionCount > 0 && history != 0)
  {
    history = 0;
    om.panda();
    om.printText("HI", 75, 28, 2);
  }

  if (webserver._connectionCount == 0 && history != 1)
  {
    history = 1;
    om.panda();
    om.printText(ipPart1.c_str(), 75, 28, 1);
    om.printText(ipPart2.c_str(), 75, 36, 1);
  }

  // If a user request was issue'd then process it.
  if (deskControls._wsRequestReady)
  {
    history = 2;
    deskControls.processWsRequest();
  }
}