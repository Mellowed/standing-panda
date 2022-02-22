<img src="https://raw.githubusercontent.com/Mellowed/documentation/main/standing-panda/home-page.PNG" align="right" height="96" />

# Standing-Panda

## Table of Contents

* [Introduction](#introduction)
  * [What](#what)
  * [Why](#why)
  * [How](#how)
  * [Demo](#demo)
* [Build](#build)
       
# Introduction

### What

Just some hacky code to make my desk stand and sit. 

### Why

I wanted to build a standing desk with a custom UI and control system. 

### How

Well, I basicly took the following list of materials and glued them together.

Materials: 
- 12A DC power supply @ 12v 
- Sonoff wifi Switch
- ESP32 (Wemos Lolin32) + Oled
- MPU6050 (MEMS)
- VL5310x (TOF Range Sensor)
- L298N H-Bridge
- LM2596 DC-DC step down converter
- 4 Channel Relay
- Buttons and toggles

### Demo

![](https://github.com/Mellowed/documentation/blob/main/standing-panda/Desk-in-action.gif)

### Build

This project relies on https://github.com/Mellowed/standing-panda-ui.

Once the code is flashed you will need to upload the data folder to the esp32 SPIFFS. This can be done using the arduino SPIFFS plugin.
