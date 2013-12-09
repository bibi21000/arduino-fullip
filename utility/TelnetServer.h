/* 
 FullIP.
 
 https://github.com/bibi21000/arduino-fullip
 
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

/*!
\example examples/TelnetServer/TelnetServer.ino
 */

#ifndef TELNETSERVER_H
#define TELNETSERVER_H

/// Timeout for a telnet session
#define FULLIP_TELNET_TIMEOUT 300000
//#define FULLIP_TELNET_TIMEOUT 60000
/// If you've got a memory leak, try to update this parameter
#define FULLIP_TELNET_CLOSE 250
/// Length of telnet commands and responses
//#define FULLIP_TELNET_LINE_SIZE 40
#define FULLIP_TELNET_LINE_SIZE 80

#include "FullIP.h"
#ifdef FULLIP_ETHERNET
#include "EthernetClient.h"
#include "EthernetServer.h"
#endif
#ifdef FULLIP_UIP
#include "UIPEthernet.h"
#include "UIPClient.h"
#include "UIPServer.h"
#endif

///\brief The user defined callback for parsing commands.
/// @param char : the command sent by the client
typedef uint8_t (*telnetParseCommand)(char[]);

///\brief The Telnet Server.
class TelnetServer
{
public:

/// Initialize the telnet server.
/// @param parsefunc the callback to parse user-defined commands.
/// @return 1 if the operation succeed. 0 if it fails.
    uint8_t begin(telnetParseCommand parsefunc);

/// Release resources.
/// Must be called if you don't want to send email anymore.
/// @return 1 if the operation succeed. 0 if it fails.
    uint8_t end();

///Process client connection in "background".
///Must be called in your loop.
    void maintain();
    
///Is the client connected?
/// @return 1 if the client is connected.
    uint8_t connected();

///The client.
    EthernetClient client;

///Disconnect the client.
    void disconnect();

///Parse the buffer and call the callback. 
/// @param buffer the command line to parse.
/// @return 1 if the operation succeed. 0 if it fails.
    uint8_t iParse(char buffer[]);

///Print the prompt.
    void printPrompt();
    
private:
	
protected:
///The callback function used to parse command line.
    telnetParseCommand _iParse;
///The server.
    EthernetServer* iServer;
///The current state in the state machine.
    uint8_t iTelnetState;
///The next timeout.
    unsigned long iTelnetTimeout;
///The next delayed connection close.
    unsigned long iTelnetClose;

};

#endif
