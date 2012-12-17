/*
 * Authenticator.cpp
 *
 * Created: 8/11/2012 11:33:11 AM
 *  Author: Steve
 */ 

#include "Authenticator.h"
#include <EEPROM.cpp>

void Authenticator::init(String master) {
	masterKey = master;
	nextWriteAddress = 0;
	
	eepromRead();
}

int Authenticator::check(String key) {
	if(key.equals(masterKey))
		return AUTH_MASTER;
	
	int size = codes.size();
	bool exists = false;
	
	for(int i = 0; i < codes.size(); ++i)  {
		if(codes[i].equals(key)) {
			exists = true;
			break;
		}			
	}
	
	if(exists) {
		return AUTH_ACCEPTED;
	} else {
		return AUTH_DECLINED;
	}		
}

int Authenticator::addKey(String key) {
	if(codes.size() >= 10)
		return AUTH_MAX_CODES;
		
	codes.push_back(key);
	eepromWrite(nextWriteAddress, key);
	return AUTH_CODE_ADDED;
}

int Authenticator::removeKey(String key) {
	int i, size = codes.size();
	bool codeFound = false;
	for(i = 0; i < size; ++i) {
		if(codes[i].equals(key)) {
			codeFound = true;
			codes.erase(codes.begin() + i);
			break;
		}
	}	
	
	if(codeFound) {
		nextWriteAddress = 0;
		eepromClear();
		eepromWriteAll();
		return AUTH_CODE_REMOVED;
	}
	else
		return AUTH_NOTHING_TO_REMOVE;
}		

void Authenticator::eepromWrite(int _addr, String key)
{
	int size = key.length();
	int addr = _addr;
	
	EEPROM.write(addr++, 2);
	for(int i = 0; i < size; ++i) {
		EEPROM.write(addr, byte(key[i]));
		++addr;
	}		
	EEPROM.write(addr, 3);
	nextWriteAddress = addr + 1;
}

void Authenticator::eepromWriteAll() {
	int size = codes.size();
	for(int i = 0; i < size; ++i)
		eepromWrite(nextWriteAddress, codes[i]);
}

void Authenticator::eepromRead() {
	//read through all of EEPROM (address 0 through 120) (Limited to 10 codes = 120 bytes)
	//store all found keys in 'codes'
	//keys are stored in the form: STX <<10 ascii characters>> ETX
	
	String key;
	
	int i;
	int lastEOT;
	byte val;
	
	for(i = 0; i < 120; ++i) {
		val = EEPROM.read(i);
		if(val == 2) { //begin of key
			for(int j = i + 1; j < 120; ++j) {
				val = EEPROM.read(j);
				if(val != 3) //end of key
					key += char(val);
				else {
					i = j;
					codes.push_back(key);
					key = "";
					lastEOT = j;
					break;
				}
			}
		}		
	}
	
	nextWriteAddress = lastEOT+1;
}	

void Authenticator::eepromClear() {
	//write '0' to all 120 bytes of EEPROM
	for(int i = 0; i < 120; ++i)
		EEPROM.write(i, 0);
		
	nextWriteAddress = 0;
}

Authenticator Auth;