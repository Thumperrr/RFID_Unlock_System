/*
 * Authenticator.h
 *
 * Class for access control that uses any type of string key code
 * In this case, RFID reader. 
 *
 * Created: 8/11/2012 11:30:24 AM
 *  Author: Steve
 */ 


#ifndef AUTHENTICATOR_H_
#define AUTHENTICATOR_H_

#include <Arduino.h>
#include <iterator>
#include <vector>
#include <EEPROM.h>

enum AUTH_CODES {
	AUTH_MASTER = 1,
	AUTH_ACCEPTED,
	AUTH_DECLINED,
	AUTH_CODE_ADDED,
	AUTH_MAX_CODES,
	AUTH_CODE_REMOVED,
	AUTH_NOTHING_TO_REMOVE
};

class Authenticator
{
public:
	Authenticator() { }
		
	void init(String master); //Initializes the class. 
	int check(String key); //Checks to see if a key is in the system. 
	int addKey(String key);	//Adds a key to the system. 
	int removeKey(String key); //Removes a key from the system. 
	
	std::vector<String> codes; //vector to store all available keys. Limited to 10. 
private:	
	Authenticator(const Authenticator&); // no copy
	Authenticator &operator = (const Authenticator&); //no copy
	
private:	
	void eepromRead(); //Reads through EEPROM and puts all read keys into 'codes'
	void eepromClear(); //Entirely clears EEPROM
	void eepromWrite(int addr, String key); //Writes one key to EEPROM
	void eepromWriteAll(); //Writes all the keys in 'codes' to EEPROM
	
	int nextWriteAddress; //next address in EEPROM to start writing at. 
	String masterKey; //Master key, used for adding/removing keys. 
};

extern Authenticator Auth;

#endif /* AUTHENTICATOR_H_ */