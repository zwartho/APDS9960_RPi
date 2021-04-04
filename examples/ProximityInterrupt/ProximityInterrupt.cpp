/****************************************************************
ProximityInterrupt.cpp
APDS-9960 RGB and Gesture Sensor
Shawn Hymel @ SparkFun Electronics, Modified for Raspberry Pi by Will Stull
April 2, 2021
https://github.com/zwartho/APDS9960_RPi

Tests the proximity interrupt abilities of the APDS-9960.
Configures the APDS-9960 over I2C and waits for an external
interrupt based on high or low proximity conditions. Move your
hand near the sensor and watch the LED on pin 11.

Hardware Connections:

IMPORTANT: The APDS-9960 can only accept 3.3V!
 
 RPi Pin    APDS-9960 Board    Function
 
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
#include <APDS9960_RPi.h>

using namespace std;

// Pins
#define APDS9960_INT    7  // Needs to be an interrupt pin
#define LED_PIN         0 // LED for showing interrupt 

// Constants
#define PROX_INT_HIGH   50 // Proximity level for interrupt
#define PROX_INT_LOW    0  // No far interrupt

// Global variables
APDS9960_RPi apds = APDS9960_RPi();
uint8_t proximity_data = 0;
int isr_flag = 0;

void setup()
{
	wiringPiSetup();
	// Set interrupt pin as an input
	pinMode(APDS9960_INT, INPUT); 
	// Set LED pin as output
	pinMode(LED_PIN, OUTPUT);
  
	cout << endl;
	cout << "---------------------------------------" << endl;
	cout << "SparkFun APDS-9960 - ProximityInterrupt" << endl;
	cout << "---------------------------------------" << endl;

	// Initialize interrupt service routine
	wiringPiISR(0, INT_EDGE_FALLING, interruptRoutine);

	// Initialize APDS-9960 (configure I2C and initial values)
	if ( apds.init() ) {
		cout << "APDS-9960 initialization complete" << endl;
	} else {
		cout << "Something went wrong during APDS-9960 init!" << endl;
	}

	// Adjust the Proximity sensor gain
	if ( !apds.setProximityGain(PGAIN_2X) ) {
		cout << "Something went wrong trying to set PGAIN" << endl;
	}

	// Set proximity interrupt thresholds
	if ( !apds.setProximityIntLowThreshold(PROX_INT_LOW) ) {
		cout << "Error writing low threshold" << endl;
	}
	if ( !apds.setProximityIntHighThreshold(PROX_INT_HIGH) ) {
		cout << "Error writing high threshold" << endl;
	}

	// Start running the APDS-9960 proximity sensor (interrupts)
	if ( apds.enableProximitySensor(true) ) {
		cout << "Proximity sensor is now running" << endl;
	} else {
		cout << "Something went wrong during sensor init!" << endl;
	}
	}

void loop()
{

	// If interrupt occurs, print out the proximity level
	if ( isr_flag == 1 ) {

		// Read proximity level and print it out
		if ( !apds.readProximity(proximity_data) ) {
			cout << "Error reading proximity value" << endl;
		} else {
			cout << "Proximity detected! Level: " << endl;
			cout << proximity_data << endl;
		}

		// Turn on LED for a half a second
		digitalWrite(LED_PIN, HIGH);
		delay(500);
		digitalWrite(LED_PIN, LOW);

		// Reset flag and clear APDS-9960 interrupt (IMPORTANT!)
		isr_flag = 0;
		if ( !apds.clearProximityInt() ) {
			cout << "Error clearing interrupt" << endl;
		}

	}
}

void interruptRoutine()
{
	isr_flag = 1;
}