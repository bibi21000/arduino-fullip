/* 
 FullIP.
 
 FullIP is a suite of ip protocols implementation.
 It can be use the official ethernet shield
 And with Enc28j60
 
 Copyright (C) 2013 by Sébastien GALLET <bibi21000@gmail.com>
 
 -----------------

 Telnet Server implementation
 
 -----------------

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "FullIP.h"
#ifdef FULLIP_ETHERNET
#include "Ethernet.h"
#endif
#ifdef FULLIP_UIP
#include "UIPEthernet.h"
#endif
#include "utility/TelnetServer.h"

/// Start server
uint8_t TelnetServer::begin(telnetParseCommand parsefunc)
{
  _iParse=parsefunc;
  iServer=new EthernetServer(23);
  iServer->begin();
  iTelnetState=0;
  return 1;
}

uint8_t TelnetServer::iParse(char buffer[])
{
	char temp[FULLIP_TELNET_LINE_SIZE];
	//Serial.println(PSTR("quit"));
	if (strcmp_P(buffer,PSTR(""))==0) {
		strcpy_P(temp,PSTR("Welcome to telnet"));
		client.println(temp);
		strcpy_P(temp,PSTR("Type ? to display a list of commands"));
		client.println(temp);
		return _iParse(buffer);
	} else if (strcmp_P(buffer,PSTR("quit"))==0) {
		//strcpy_P(temp,PSTR("Bye"));
		//client.println(temp);
		disconnect();
		return 1;
	} else if (strcmp_P(buffer,PSTR("?"))==0) {
		strcpy_P(temp,PSTR("quit : close the connection"));
		client.println(temp);
		return _iParse(buffer);
	} else {
		uint8_t ret=_iParse(buffer);
		if (ret!=1) {
			strcpy_P(temp,PSTR("command not found"));
			client.println(temp);
		}
	}
	return 0;
}

void TelnetServer::printPrompt()
{
	char buffer[10];
	strcpy_P(buffer,PSTR("telnet> "));
	client.print(buffer);
}

///Must be called in your loop
///Process client requests in background
void TelnetServer::maintain()
{
	Ethernet.maintain();
	char buffer[FULLIP_TELNET_LINE_SIZE];

	delay(100);
	//Serial.println(iServer->available());
	
	if ((client = iServer->available()) && iTelnetState==0) {
#ifdef FULLIP_DEBUG_TELNET
		Serial.println("Telnet client connection");
#endif
		iTelnetState = 1;
		strcpy_P(buffer,PSTR(""));
		iParse(buffer);
		iTelnetTimeout = millis() + FULLIP_TELNET_TIMEOUT;
	}
	
	if (iTelnetState==1) {
		if (client.connected() && client.available()) {
			iTelnetTimeout = millis() + FULLIP_TELNET_TIMEOUT;
			char c = '\0';
			int charsReceived=0;
			while (client.available()>0 && charsReceived<FULLIP_TELNET_LINE_SIZE && c!=0x0d) {
				c = client.read();
				buffer[charsReceived] = c;
				charsReceived++;
			}

#ifdef FULLIP_DEBUG_TELNET
			Serial.print("buffer=<");
			Serial.print(buffer);
			Serial.println(">");
			Serial.print("charsReceived=<");
			Serial.print(charsReceived);
			Serial.println(">");
#endif

			//if CR found go look at received text and execute command
			if(c == 0x0d) {
				buffer[charsReceived-1]='\0';
	#ifdef FULLIP_DEBUG_TELNET
				Serial.print("buffer=<");
				Serial.print(buffer);
				Serial.println(">");
	#endif
				if (strlen(buffer)!=0) {
					iParse(buffer);
				}
				// after completing command, print a new prompt
				printPrompt();
			}

			// if textBuff full without reaching a CR, print an error message
			if(charsReceived >= FULLIP_TELNET_LINE_SIZE-1) {
				while (client.available()>0) {
					c = client.read();
				}
				client.println();
				strcpy_P(buffer,PSTR("Line too long"));
				client.println(buffer);
				printPrompt();
			}
			// if textBuff not full and no CR, do nothing else;  
			// go back to loop until more characters are received			  

		} else if ((long signed)(millis() - iTelnetTimeout) > 0) {
#ifdef FULLIP_DEBUG_TELNET
			Serial.println("Telnet timeout");
#endif
			disconnect();
		}
	} else if (iTelnetState==2 && (long signed)(millis() - iTelnetClose) > 0) {
		uint8_t c;
		while((client.available()) > 0)
			c=client.read();
		client.stop();
#ifdef FULLIP_DEBUG_TELNET
			Serial.println("Telnet client connection closed");
#endif
		iTelnetState = 0;
	}
}

///Show if a client is connected
uint8_t TelnetServer::connected()
{
	return (iTelnetState==1);
}				

///Disconnect the client
void TelnetServer::disconnect()
{
	iTelnetClose = millis() + FULLIP_TELNET_CLOSE;
	iTelnetTimeout = millis() + FULLIP_TELNET_TIMEOUT;
	iTelnetState = 2;
}				
