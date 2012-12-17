/*
 * RFID_Unlock_System.cpp
 *
 * Created: 8/9/2012 9:11:25 PM
 *  Author: Steve
 */ 

#define ARDUINO 100

enum LED_STATUS {
	LED_RED = 1,
	LED_GREEN,
	LED_BLUE,
	LED_PURPLE,
	LED_OFF
};
	

#include "Arduino.h"
#include <pnew.cpp>
#include "RFIDReader.h"
#include "Authenticator.h"
//#include "Authenticator.cpp"


//////// Pins //////////////////////////////////////////
const int stx = 8;
const int srx = 9;
const int resetPin = 2;
const int rpin = 6;
const int gpin = 5;
const int bpin = 4;
const int doorUnlock = 13;
////////////////////////////////////////////////////////

/////// Prototypes ////////////////////////////////////
void setup();
void loop();
void blinkRGBLED(int status, int del, int times);
void setRGBLED(int status);
void unlock();
///////////////////////////////////////////////////////

////// Globals ////////////////////////////////////////
RFIDReader reader(srx, stx, 9600, resetPin);
///////////////////////////////////////////////////////

//Serial print operator overload
template<class T> 
inline Print &operator <<(Print &obj, T arg) {
	obj.print(arg);
	return obj;
}
const char endl = '\n';

void setup() {
	pinMode(rpin, OUTPUT);
	pinMode(gpin, OUTPUT);
	pinMode(bpin, OUTPUT);
	pinMode(doorUnlock, OUTPUT);
	digitalWrite(13, LOW);
	digitalWrite(bpin, HIGH);
	Serial.begin(57600);
	Auth.init("4F00889C24");
}	

void loop() {
	reader.read();
	
	if(reader.available()) {
		String code = reader.get();
		
		int check = Auth.check(code);
		if(check == AUTH_ACCEPTED) {
			setRGBLED(LED_GREEN);
			unlock();
			delay(500);
			setRGBLED(LED_BLUE);
		}
		else if(check == AUTH_DECLINED) {
			setRGBLED(LED_RED);
			delay(500);
			setRGBLED(LED_BLUE);
		}
		else if(check == AUTH_MASTER) {
			setRGBLED(LED_PURPLE);
			bool nextCardScanned = false;
			while(!nextCardScanned) {
				reader.read();
				
				if(reader.available()) {
					code = reader.get();
					check = Auth.check(code);
					if(check == AUTH_DECLINED) {
						if(Auth.addKey(code) == AUTH_MAX_CODES) {
							blinkRGBLED(LED_RED, 50, 5);
						} else {
							blinkRGBLED(LED_GREEN, 50, 5);
						}						
					}						
					else if(check == AUTH_ACCEPTED) {
						blinkRGBLED(LED_RED, 50, 5);
						Auth.removeKey(code);
					}						
					
					nextCardScanned = true;
				}
			}
			setRGBLED(LED_BLUE);
		}
	}		
	
}

void setRGBLED(int status) {
	switch(status) {
		case LED_RED: {
			digitalWrite(rpin, HIGH);
			digitalWrite(gpin, LOW);
			digitalWrite(bpin, LOW);
			break;
		}
		case LED_GREEN: {
			digitalWrite(rpin, LOW);
			digitalWrite(gpin, HIGH);
			digitalWrite(bpin, LOW);
			break;
		}
		case LED_BLUE: {
			digitalWrite(rpin, LOW);
			digitalWrite(gpin, LOW);
			digitalWrite(bpin, HIGH);
			break;
		}
		case LED_PURPLE: {
			digitalWrite(rpin, HIGH);
			digitalWrite(gpin, LOW);
			digitalWrite(bpin, HIGH);
			break;
		}			
		case LED_OFF: {
			digitalWrite(rpin, LOW);
			digitalWrite(gpin, LOW);
			digitalWrite(bpin, LOW);
			break;
		}
		default: break;	
	}	
}

void unlock() {
	digitalWrite(doorUnlock, HIGH);
	delay(50);
	digitalWrite(doorUnlock, LOW);
}

void blinkRGBLED(int status, int del, int times)
{
	for(int i = 0; i < times; ++i)
	{
		setRGBLED(LED_OFF);
		delay(del);
		setRGBLED(status);
		delay(del);
	}
}