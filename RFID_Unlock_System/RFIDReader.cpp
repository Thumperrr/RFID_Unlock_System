/*
 * RFIDReader.cpp
 *
 * Created: 8/10/2012 6:49:42 PM
 *  Author: Steve
 */ 

#include "RFIDReader.h"
#include "SoftwareSerial.cpp"

RFIDReader::RFIDReader(uint8_t _rx, uint8_t _tx, long _baud, int _resetPin) : RFIDSerial(_rx, _tx) {
	resetPin = _resetPin;
	pinMode(resetPin, OUTPUT);
	digitalWrite(resetPin, HIGH);
	RFIDSerial.begin(_baud);
}

RFIDReader::~RFIDReader() {
}

void RFIDReader::read() {
	byte val = 0;
	byte tempbyte = 0;
	byte bytesread = 0;
	byte ID[6];
	if(RFIDSerial.available() > 0) {
		if((val = RFIDSerial.read()) == 0x02) { //Receiving 0x02 indicates the beginning of data
			bytesread = 0;
			while(bytesread < 12) {
				if(RFIDSerial.available() > 0) {
					val = RFIDSerial.read();
					if(val == 0x0D || val == 0x0A || val == 0x03 || val == 0x02) //stop reading, you're no longer in the data range.
						break;
					
				    //convert received ascii characters to hex (must be in hex to calculate checksum)
					if(val >= '0' && val <= '9')
						val = val - '0';
					else if(val >= 'A' && val <= 'F')
						val = val + 10 - 'A';
					
					//combine every two received hex characters (4 bits each) into one hex byte
					if(bytesread & 1 == 1) {
						ID[bytesread >> 1] = (val | (tempbyte << 4));
					} else {
						tempbyte = val;
					}
					++bytesread;
				}
			}
		}
	}	
	if(bytesread == 12) { //read a full code
		//calculate the checksum for the code
		byte checksum;
		for(int i = 0; i < 5; ++i)
			checksum ^= ID[i];
				
		if(checksum == ID[5]) { //if the checksums are equal, add the ID to the queue.
			String temp;
			for(byte i = 0; i < 5; ++i) {	
				if(ID[i] < 16)
					temp += "0" + String(ID[i], HEX);		
				else 
					temp += String(ID[i], HEX);
			}			
			
			temp.toUpperCase();
			codeQueue.push_back(temp);
		} else {
			codeQueue.push_back("ERROR_CHECKSUM");
		}
	}
}

bool RFIDReader::available()
{
	if(codeQueue.size() > 0)
		return true;
	return false;
}

String RFIDReader::get()
{
	String ret;
	if(available()) {
		ret = codeQueue[0];
		codeQueue.erase(codeQueue.begin());
		return ret;
	} else {
		ret = "NOTHING_IN_QUEUE";
		return ret;
	}
}