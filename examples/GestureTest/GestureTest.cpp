/****************************************************************
GestureTest.cpp
APDS-9960 RGB and Gesture Sensor
Shawn Hymel @ SparkFun Electronics, Modified for Raspberry Pi by Will Stull
April 2, 2021
https://github.com/zwartho/APDS9960_RPi

Tests the gesture sensing abilities of the APDS-9960. Configures
APDS-9960 over I2C and waits for gesture events. Calculates the
direction of the swipe (up, down, left, right) and displays it
on a serial console. 

To perform a NEAR gesture, hold your hand
far above the sensor and move it close to the sensor (within 2
inches). Hold your hand there for at least 1 second and move it
away.

To perform a FAR gesture, hold your hand within 2 inches of the
sensor for at least 1 second and then move it above (out of
range) of the sensor.

Hardware Connections:

IMPORTANT: The APDS-9960 can only accept 3.3V!
 
 RPi Pin    APDS-9960 Board    Function
 
 3.3V         VCC              Power
 GND          GND              Ground
 3            SDA              I2C Data
 5            SCL              I2C Clock
 2            INT              Interrupt

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

// Pins
#define APDS9960_INT 	7 // Needs to be an interrupt pin

// Constants

// Global Variables
APDS9960_RPi apds = APDS9960_RPi();
int isr_flag = 0;

// Function Declarations
void interruptRoutine();
void handleGesture();

int main()
{

	// init wiringPi
	wiringPiSetup();

	// Initialize Serial port
	cout << endl;
	cout << "------------------------------------" << endl;
	cout << "  SparkFun APDS-9960 - GestureTest  " << endl;
	cout << "------------------------------------" << endl;

	// Initialize interrupt service routine
	wiringPiISR(APDS9960_INT, INT_EDGE_FALLING,  interruptRoutine);

	// Initialize APDS-9960 (configure I2C and initial values)
	if ( apds.init() ) {
		cout << "APDS-9960 initialization complete" << endl;
	} else {
		cout << "Something went wrong during APDS-9960 init!" << endl;
	}

	// Start running the APDS-9960 gesture sensor engine
	if ( apds.enableGestureSensor(true) ) {
		cout << "Gesture sensor is now running" << endl;
	} else {
		cout << "Something went wrong during gesture sensor init!" << endl;
	}

	while(1) {
		if( isr_flag == 1 ) {
			handleGesture();
			isr_flag = 0;
		}
	}
	return 0;
}

void interruptRoutine()
{
	isr_flag = 1;
}

void handleGesture()
{
	if ( apds.isGestureAvailable() ) {
		switch ( apds.readGesture() ) {
		case DIR_UP:
			cout << "UP" << endl;
			break;
		case DIR_DOWN:
			cout << "DOWN" << endl;
			break;
		case DIR_LEFT:
			cout << "LEFT" << endl;
			break;
		case DIR_RIGHT:
			cout << "RIGHT" << endl;
			break;
		case DIR_NEAR:
			cout << "NEAR" << endl;
			break;
		case DIR_FAR:
			cout << "FAR" << endl;
			break;
		default:
			cout << "NONE" << endl;
		}
	}
	else {
		cout << "No gesture available" << endl;
	}
}