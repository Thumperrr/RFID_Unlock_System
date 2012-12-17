/*
 * RFIDReader.h
 * 
 * Class for controlling ID-12 and ID-20 RFID readers.
 * 
 * Pin1 - GND
 * Pin2 - Reset --- Arduino digital pin 2
 * Pin9 - Data (srx) --- Arduino digital pin 9
 * Pin10 - LED w/ 1K series resistor
 * Pin11 - 5V
 *
 * Created: 8/10/2012 6:19:13 PM
 *  Author: Steve
 */ 
#ifndef RFIDREADER_H_
#define RFIDREADER_H_

#define F_CPU 16000000

#include <Arduino.h>
#include <iterator>
#include <vector>
#include <SoftwareSerial.h>

class RFIDReader
{
public:	
	RFIDReader(uint8_t _rx, uint8_t _tx, long _baud, int _resetPin);	
	~RFIDReader();
	
	RFIDReader(const RFIDReader&); //prevent copying of this class
	RFIDReader &operator = (const RFIDReader&); //prevent copying of this class
	
	bool available(); //returns true if a code is available in the queue
	String get(); //returns the next code in the queue
	
	void read(); //needs to be called on every loop

private:
	int resetPin; //not used in this implementation yet. 
	std::vector<String> codeQueue;
	SoftwareSerial RFIDSerial;
};


#endif /* RFIDREADER_H_ */