#ifndef OledManager_h
#define OledManager_h

#include "Arduino.h"
#include <Adafruit_GFX.h>
#include <Adafruit_GrayOLED.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_SPITFT.h>
#include <gfxfont.h>
#include <Adafruit_SSD1306.h>
#include <splash.h>
#include <Wire.h>

// Screen
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

class OledManager
{
public:
    OledManager(TwoWire *i2c);
    void begin();
    void homeScreen();
    void goingUp(float left, float right, String msg = "Going Up");
    void goingDown(float left, float right, String msg = "Going Down");
    void panda(String msg = "");
    void pandaUpDown();
    void pandaError(String msg);
    void showImage(const uint8_t bitmap[]);
    void printText(const char *msg, int x, int y, int size = 1);

private:
    TwoWire *_i2c;
    Adafruit_SSD1306 _display = Adafruit_SSD1306();
};

#endif
