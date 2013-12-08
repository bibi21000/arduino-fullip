/*!
 \brief FullIP for Arduino
 \mainpage 

 FullIP is a suite of ip protocols implementation.
 
 https://github.com/bibi21000/arduino-fullip
 
 It can be use the official ethernet shield
 And with Enc28j60.
 
 Copyright (C) 2013 by Sébastien GALLET <bibi21000@gmail.com>
 
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

#ifndef FULLIP_H
#define FULLIP_H

//FullIP configuration parameters
//===============================

///Uncomment to build with the enc28j60 using UIPEthernet
//#define FULLIP_UIP ///Comment this line to build with the official Ethernet library (w5100)

///Uncomment to build the ftp client and the http server 
///Also add SD.h to your sketch
#define FULLIP_SD 

/// Turn on debug messages in smtp client
//#define FULLIP_DEBUG_SMTP

///\def  Turn on debug messages in telnet server
//#define FULLIP_DEBUG_TELNET

/// Turn on debug messages in http server
//#define FULLIP_DEBUG_HTTP

/// Turn on debug messages in ftp client
//#define FULLIP_DEBUG_FTP

//===============================

#ifdef FULLIP_SD
  #include "SD.h"
#endif

#ifndef FULLIP_UIP
  #define FULLIP_ETHERNET
#endif

#ifdef FULLIP_ETHERNET
#include "Ethernet.h"
#include "EthernetClient.h"
#include "EthernetServer.h"
#endif

#ifdef FULLIP_UIP
#include "UIPEthernet.h"
#include "UIPClient.h"
#include "UIPServer.h"
#endif

#include "utility/HttpServer.h"
#include "utility/TelnetServer.h"
#include "utility/SmtpClient.h"
#include "utility/FtpClient.h"

#endif

