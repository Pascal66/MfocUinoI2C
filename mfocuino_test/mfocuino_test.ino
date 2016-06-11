#include "Arduino.h"

#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>

PN532_I2C pn532_i2c(Wire);
PN532 nfc = PN532(pn532_i2c);

uint8_t buffer[32];

#define PN532_WAKEUP                        (0x55)

void setup()
{
	  Serial.begin(230400); //115200, 230400, 460800
	  Serial.flush();
	  nfc.begin();
}

// The loop function is called in an endless loop
void loop()
{
	int b = Serial.available();
	  if (b >= 5) {
	    Serial.readBytes((char*)buffer, 5);
	    if (buffer[0] == PN532_WAKEUP) {
	      //handle wake up case
	      while (Serial.available() < 5); //wait the command
	      b = Serial.readBytes((char*)buffer + 5, 5);
	      //send raw command to pn532
	      //get length of package :
	      // (PREAMBLE + START + LEN + LCS) + (TFI + DATA[0...n]) + (DCS + POSTAMBLE)
	      uint8_t l = buffer[8] + 2;
	      while (Serial.available() < l); //wait the command
	      //read command from uart
	      Serial.readBytes((char*)buffer + 10, l);
	      //send raw command to pn532
	      nfc.sendRawCommandCheckAck(buffer, l + 10);
	      //read pn532 answer
	      nfc.readRawCommandAnswer(buffer, l + 10);
	    } else {
	      //normal case
	      //get length of package :
	      // (PREAMBLE + START + LEN + LCS) + (TFI + DATA[0...n]) + (DCS + POSTAMBLE)
	      uint8_t l = buffer[3] + 2;
	      //read command from uart

	      //while(Serial.available() < l); //wait the command
	      //Serial.readBytes((char*)buffer+5, l);

	      uint8_t br = l;
	      uint8_t* bufptr = buffer + 5;
	      while (br) {
	        if (Serial.available()) {
	          *bufptr++ = Serial.read();
	          --br;
	        }
	      }

	      //send raw command to pn532
	      nfc.sendRawCommandCheckAck(buffer, l + 5);
	      //read pn532 answer
	      nfc.readRawCommandAnswer(buffer, l + 5);
	    }
	  }

}
