/****************************************************************
ColorSensor.cpp
APDS-9960 RGB and Gesture Sensor
Shawn Hymel @ SparkFun Electronics, Modified for Raspberry Pi by Will Stull
April 2, 2021
https://github.com/zwartho/APDS9960_RPi

Tests the color and ambient light sensing abilities of the 
APDS-9960. Configures APDS-9960 over I2C and polls the sensor for
ambient light and color levels, which are displayed over the 
serial console.

Hardware Connections:

IMPORTANT: The APDS-9960 can only accept 3.3V!
 
 RPi Pin    APDS-9960 Board    Function
 
 3.3V         VCC              Power
 GND          GND              Ground
 3            SDA              I2C Data
 5            SCL              I2C Clock

Resources:
Include APDS9960_RPi.h

Development environment specifics:
Written using wiringPi library
Tested with Raspberry Pi Zero W

This code is beerware; if you see me (or any other SparkFun 
employee) at the local, and you've found our code helpful, please
buy us a round!

Distributed as-is; no warranty is given.
****************************************************************/
#include <iostream>
#include "APDS9960_RPi.h"

using namespace std;

// Global Variables
APDS9960_RPi apds = APDS9960_RPi();
uint16_t ambient_light = 0;
uint16_t red_light = 0;
uint16_t green_light = 0;
uint16_t blue_light = 0;

int main()
{
	cout << endl;
	cout << "------------------------------------" << endl;
	cout << "  SparkFun APDS-9960 - ColorSensor  " << endl;
	cout << "------------------------------------" << endl;

	// Initialize APDS-9960 (configure I2C and initial values)
	if( apds.init() ) {
		cout << "APDS-9960 initialization complete" << endl;
	} else {
		cout << "Something went wrong during APDS-9960 init!" << endl;
	}

	// Start running the APDS-9960 light sensor (no interrupts)
	if( apds.enableLightSensor(false) ) {
		cout << "Light sensor is now running" << endl;
	} else {
		cout << "Something went wrong during sensor init!" << endl;
	}

	// Wait for initialization and calibration to finish
	delay(500);

	while(1) {
		// Read the light levels (ambient, red, green, blue)
		if (  !apds.readAmbientLight(ambient_light) ||
				!apds.readRedLight(red_light) ||
				!apds.readGreenLight(green_light) ||
				!apds.readBlueLight(blue_light) ) {
			cout << "Error reading light values" << endl;
		}
		else {
			cout << "Ambient: ";
			cout << int(ambient_light);
			cout << " Red: ";
			cout << int(red_light);
			cout << " Green: ";
			cout << int(green_light);
			cout << " Blue: ";
			cout << int(blue_light) << endl;
		}

		// Wait 1 second before next reading
		delay(1000);
	}
	return 0;
}