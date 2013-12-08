/* 
 FullIP.
 
 https://github.com/bibi21000/arduino-fullip
 
 FullIP is a suite of ip protocols implementation.
 It can be use the official ethernet shield
 And with Enc28j60
 
 Copyright (C) 2013 by Sébastien GALLET <bibi21000@gmail.com>
 
 -----------------

 HTTP Server implementation
 
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
\example examples/HttpServer/HttpServer.ino
 */
 
#ifndef HTTPSERVER_H
#define HTTPSERVER_H

/// Timeout for an http session
#define FULLIP_HTTP_TIMEOUT 3000
//#define FULLIP_HTTP_TIMEOUT 5000
/// If you've got a memory leak, try to update this parameter
#define FULLIP_HTTP_CLOSE 250
/// Length of HTTP url
#define FULLIP_HTTP_URL_SIZE 80
/// Length of HTTP data read from sd
#define FULLIP_HTTP_HEADER_SIZE 50
/// Length of HTTP data read from sd
#define FULLIP_HTTP_DATA_SIZE 250

#include "FullIP.h"
#ifdef FULLIP_ETHERNET
#include "EthernetClient.h"
#include "EthernetServer.h"
#endif
#ifdef FULLIP_UIP
#include "UIPClient.h"
#include "UIPServer.h"
#endif
#ifdef FULLIP_SD
  #include "SD.h"
#endif

/// Used to parse a page
/// @param EthernetClient : the client to write to
/// @param char : the url requested by the client
/// @param int : 
typedef uint8_t (*httpParsePage)(EthernetClient, char[], int);

/// \brief Used to act as a rest server
/// By default, the server will server files on the SD
/// The user must implement a state machine to server files
/// The function must return 1 or the number of bytes remaining
/// @param EthernetClient : the client to write to
/// @param char : the url requested by the client
/// @param uint8_t : the type of callback (0=connection,1=transmit data)
typedef uint8_t (*httpParseUrl)(EthernetClient, char[], char[], uint8_t);

/// \brief The HTTP server. 
class HttpServer {
public:

/// Start HTTP server.
/// @param rootDir the directory on the SD card to look for files.
/// @return 1 if the operation succeed. 0 if it fails.
    uint8_t begin(char rootDir[]);

/// Start HTTP server.
/// @param rootDir the directory on the SD card to look for files.
/// @param parsepage the function to use for parsing pages.
/// @return 1 if the operation succeed. 0 if it fails.
    uint8_t begin(char rootDir[], httpParsePage parsepage);

/// Start HTTP server.
/// @param rootDir the directory on the SD card to look for files.
/// @param parseurl the function to use for parsing urls.
/// @param parsepage the function to use for parsing pages.
/// @return 1 if the operation succeed. 0 if it fails.
    uint8_t begin(char rootDir[], httpParseUrl parseurl, httpParsePage parsepage);

/// Release resources used by the Server.
/// Must be called if you don't want to send email anymore.
/// @return 1 if the operation succeed. 0 if it fails.
   uint8_t end();

///Process client connection in "background".
///Must be called in your loop.
    void maintain();
    
///Is a client connected
/// @return 1 if the client is connected.
    uint8_t connected();

///The client 
    EthernetClient client;
    
/// Parse the page and call the page callback  . 
/// @param client the client to write to.
/// @param buffer the keyword found in page.
/// @param size ....
/// @return 1 if the operation succeed. 0 if it fails.
    uint8_t iParsePage(EthernetClient client, char buffer[], int size);

/// Parse the url and call the URL callback   .
/// @param client the client to write to.
/// @param myurl the url to scan.
/// @param mode the mode of callback : 0=connection, 1=send data.
/// @return 1 if the operation succeed. 0 if it fails.
	uint8_t iParseUrl(EthernetClient client, char myurl[], uint8_t mode);
    
/// Print header to the client.
/// @param code the return code (200,404, ...).
/// @param mime the mime type to use (text/html, ...).
/// @param refresh the delay before refreshing page. 0=No refresh.
	void printHeader(int code, char mime[], int refresh);

/// Print HTML header to the client.
/// @param code the return code (200,404, ...).
/// @param refresh the delay before refreshing page. 0=No refresh.
	void printHtmlHeader(int code, int refresh);

/// Print JSON header to the client.
/// @param code the return code (200,404, ...).
/// @param refresh the delay before refreshing page. 0=No refresh.
	void printJsonHeader(int code, int refresh);

/// Print header to the client depending on the filename.
/// @param filename the filename (index.html, image.png, ...).
/// @param code the return code (200,404, ...).
/// @param refresh the delay before refreshing page. 0=No refresh.
	void printAutoHeader(char filename[], int code, int refresh);

/// Retrieve the mime type from the filename.
/// @param filename the filename (index.html, image.png, ...).
/// @param mime the returning mime type (text/html, image/png, ...).
	void autoMime(char filename[], char mime[]);

private:

///Disconnect the client.
    void disconnect();


protected:
///The parse page callback function.
    httpParsePage _iParsePage;
///The parse URL callback function.
    httpParseUrl _iParseUrl;
#ifdef FULLIP_SD
///The file to send.
    File iHTTPFile;
#endif
///The server.
    EthernetServer* iServer;
///The current state in the state machine.
    uint8_t iHTTPState;
///The next timeout.
    unsigned long iHTTPTimeout;
///The date of the delayed close.
	unsigned long iHTTPClose;
///The root directory.
    char* iHTTPRootDir;
///The URL requested by the client.
    char* iHTTPClientUrl;
///The http mode requested by the client (PUT,GET,DELETE,...).
    char* iHTTPClientMode;
///The http requested by the client is a binary file.
    uint8_t iHTTPClientBinary;
///The last transfer result.
    int lastTransfer;
};

#endif
