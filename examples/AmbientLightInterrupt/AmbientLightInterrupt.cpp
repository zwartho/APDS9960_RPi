/****************************************************************
AmbientLightInterrupt.cpp
APDS-9960 RGB and Gesture Sensor
Shawn Hymel @ SparkFun Electronics, Modified for Raspberry Pi by Will Stull
April 2, 2021
https://github.com/zwartho/APDS9960_RPi

Tests the ambient light interrupt abilities of the APDS-9960.
Configures the APDS-9960 over I2C and waits for an external
interrupt based on high or low light conditions. Try covering
the sensor with your hand or bringing the sensor close to a
bright light source. You might need to adjust the LIGHT_INT_HIGH
and LIGHT_INT_LOW values to get the interrupt to work correctly.

Hardware Connections:

IMPORTANT: The APDS-9960 can only accept 3.3V!
 
 Arduino Pin  APDS-9960 Board  Function
 
 3.3V         VCC              Power
 GND          GND              Ground
 3            SDA              I2C Data
 5            SCL              I2C Clock
 2            INT              Interrupt
 11           -                LED

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
#define APDS9960_INT    7  // Needs to be an interrupt pin
#define LED_PIN         0 // LED for showing interrupt

// Constants
#define LIGHT_INT_HIGH  1000 // High light level for interrupt
#define LIGHT_INT_LOW   10   // Low light level for interrupt

// Global variables
SparkFun_APDS9960 apds = SparkFun_APDS9960();
uint16_t ambient_light = 0;
uint16_t red_light = 0;
uint16_t green_light = 0;
uint16_t blue_light = 0;
int isr_flag = 0;
uint16_t threshold = 0;

void setup() 
{
	wiringPiSetup();
  
	// Set Interrupt Pin as an input
	pinMode(APDS9960_INT, INPUT);
	// Set LED pin as output
	pinMode(LED_PIN, OUTPUT);

	cout << endl;
	cout << "-------------------------------------" << endl;
	cout << "SparkFun APDS-9960 - Light Interrupts" << endl;
	cout << "-------------------------------------" << endl;
  
	// Initialize interrupt service routine
	wiringPiISR(0, INT_EDGE_FALLING, interruptRoutine);
  
	// Initialize APDS-9960 (configure I2C and initial values)
	if ( apds.init() ) {
		cout << "APDS-9960 initialization complete" << endl;
	} else {
		cout << "Something went wrong during APDS-9960 init!" << endl;
	}
  
	// Set high and low interrupt thresholds
	if ( !apds.setLightIntLowThreshold(LIGHT_INT_LOW) ) {
		cout << "Error writing low threshold" << endl;
	}
	if ( !apds.setLightIntHighThreshold(LIGHT_INT_HIGH) ) {
		cout << "Error writing high threshold" << endl;
	}

	// Start running the APDS-9960 light sensor (no interrupts)
	if ( apds.enableLightSensor(false) ) {
		cout << "Light sensor is now running" << endl;
	} else {
		cout << "Something went wrong during light sensor init!" << endl;
	}

	// Read high and low interrupt thresholds
	if ( !apds.getLightIntLowThreshold(threshold) ) {
		cout << "Error reading low threshold" << endl;
	} else {
		cout << "Low Threshold: " << threshold << endl;
	}
	if ( !apds.getLightIntHighThreshold(threshold) ) {
		cout << "Error reading high threshold" << endl;
	} else {
		cout << "High Threshold: " << threshold << endl;
	}

	// Enable interrupts
	if ( !apds.setAmbientLightIntEnable(1) ) {
		cout << "Error enabling interrupts" << endl;
	}

	// Wait for initialization and calibration to finish
	delay(500);
}

void loop()
{
	// If interrupt occurs, print out the light levels
	if ( isr_flag == 1 ) {
		// Read the light levels (ambient, red, green, blue) and print
		if (  !apds.readAmbientLight(ambient_light) ||
		!apds.readRedLight(red_light) ||
		!apds.readGreenLight(green_light) ||
		!apds.readBlueLight(blue_light) ) {
			cout << "Error reading light values" << endl;
		} else {
			cout << "Interrupt! Ambient: ";
			cout << int(ambient_light);
			cout << " R: ";
			cout << int(red_light);
			cout << " G: ";
			cout << int(green_light);
			cout << " B: ";
			cout << int(blue_light) << endl;
		}
		
		// Turn on LED for a half a second
		digitalWrite(LED_PIN, HIGH);
		delay(500);
		digitalWrite(LED_PIN, LOW);
		
		// Reset flag and clear APDS-9960 interrupt (IMPORTANT!)
		isr_flag = 0;
		if ( !apds.clearAmbientLightInt() ) {
			cout << "Error clearing interrupt" << endl;
		}
	}
}

void interruptRoutine()
{
	isr_flag = 1;
}