/* 
 FullIP.
 
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

#include "FullIP.h"
#ifdef FULLIP_ETHERNET
#include "Ethernet.h"
#endif
#ifdef FULLIP_UIP
#include "UIPEthernet.h"
#endif
#include "utility/HttpServer.h"

#ifdef FULLIP_SD

/// Start server
uint8_t HttpServer::begin(char rootDir[], httpParseUrl parseurl, httpParsePage parsepage)
{
	_iParseUrl=parseurl;
	_iParsePage=parsepage;
	iHTTPRootDir = (char*)malloc(strlen(rootDir)+1);
	strcpy(iHTTPRootDir, rootDir);
	iServer=new EthernetServer(80);
	iServer->begin();
	iHTTPState=0;
	iHTTPTimeout = millis() + FULLIP_HTTP_TIMEOUT;
	return 1;
}

/// Start server
uint8_t HttpServer::begin(char rootDir[], httpParsePage parsepage)
{
	return begin(rootDir, NULL, parsepage);
}

/// Start server
uint8_t HttpServer::begin(char rootDir[])
{
	return begin(rootDir, NULL, NULL);
}

/// Release resources
/// Must be called if you don't want to serve files anymore
uint8_t HttpServer::end()
{
	free(iHTTPRootDir);
	return 1;
}

/// Print the http header
void HttpServer::printHeader(int code, char mime[], int refresh){
	char buffer[50];
	sprintf_P(buffer, PSTR("HTTP/1.1 %i OK"), code);
	client.println(buffer);
	sprintf_P(buffer, PSTR("Content-Type: %s"), mime);
	client.println(buffer);
	strcpy_P(buffer, PSTR("Connection: close"));
	client.println(buffer);
	if (refresh>0) { 
		sprintf_P(buffer, PSTR("Refresh: %i"), mime);
		client.println(buffer);
	}
	client.println();
}

/// Print the http header
void HttpServer::printHtmlHeader(int code, int refresh){
	char mime[20];
	strcpy_P(mime, PSTR("text/html"));
	printHeader(code, mime, refresh);
}

void HttpServer::printJsonHeader(int code, int refresh){
	char mime[20];
	strcpy_P(mime, PSTR("application/json"));
	printHeader(code, mime, refresh);
}

void HttpServer::printAutoHeader(char filename[], int code, int refresh){
	char mime[20];
	autoMime(filename, mime);
	printHeader(code, mime, refresh);
}

void HttpServer::autoMime(char filename[], char mime[])
{
	char ext[4];
	strcpy(ext,filename+strlen(filename)-3);
	if (strcmp_P(ext,PSTR("htm"))==0) {
		strcpy_P(mime, PSTR("text/html"));
		iHTTPClientBinary=3;
	} else if (strcmp_P(ext,PSTR("jso"))==0) {
		strcpy_P(mime, PSTR("application/json"));
		iHTTPClientBinary=0;
	} else if (strcmp_P(ext,PSTR("jpg"))==0) {
		strcpy_P(mime, PSTR("image/jpeg"));
		iHTTPClientBinary=1;
	} else if (strcmp_P(ext,PSTR("gif"))==0) {
		strcpy_P(mime, PSTR("image/gif"));
		iHTTPClientBinary=1;
	} else if (strcmp_P(ext,PSTR("png"))==0) {
		strcpy_P(mime, PSTR("image/png"));
		iHTTPClientBinary=1;
	} else if (strcmp_P(ext,PSTR("js"))==0) {
		strcpy_P(mime, PSTR("application/javascript"));
		iHTTPClientBinary=2;
	} else if (strcmp_P(ext,PSTR("css"))==0) {
		strcpy_P(mime, PSTR("text/css"));
		iHTTPClientBinary=2;
	} else {
		strcpy_P(mime, PSTR("text/plain"));
		iHTTPClientBinary=0;
	}
}

uint8_t HttpServer::iParsePage(EthernetClient client, char buffer[], int size)
{
	//delay(1);
	client.write((uint8_t *)buffer,size);
}

uint8_t HttpServer::iParseUrl(EthernetClient cli, char myurl[], uint8_t mode)
{
	//return _iParsePage(cli, myurl, mode);
}

///Must be called in your loop
///Process client requests in background
void HttpServer::maintain()
{
	Ethernet.maintain();
	
	if (((long signed)(millis() - iHTTPTimeout) > 0) && iHTTPState!=0) {
#ifdef FULLIP_DEBUG_HTTP
			Serial.println("Http timeout");
#endif
			disconnect();
	}
		
	if ((iServer->available()) && iHTTPState==0) {
		client = iServer->available();
#ifdef FULLIP_DEBUG_HTTP
		Serial.println("Http client connection");
#endif
		iHTTPState = 1;
#ifdef FULLIP_DEBUG_HTTP
		Serial.println(iHTTPState);
#endif
		//iParse(buffer);
		iHTTPTimeout = millis() + FULLIP_HTTP_TIMEOUT;
	}
	
	if (iHTTPState==1) {
		if (client.connected() && client.available()) {
			char buffer[FULLIP_HTTP_HEADER_SIZE]="\0";
			char url[FULLIP_HTTP_URL_SIZE]="\0";
			char params[FULLIP_HTTP_URL_SIZE*2/3]="\0";
			char mode[10]="\0";
			char page[26];
			char c ='\0';
			int charsReceived=0;
			while (client.available()>0 && charsReceived<FULLIP_HTTP_HEADER_SIZE && c!='\n') {
				c = client.read();
				buffer[charsReceived] = c;
				charsReceived++;
				//delay(1);
			}
			buffer[charsReceived] = '\0';
			while (client.available()>0) {
				c = client.read();
			}
#ifdef FULLIP_DEBUG_HTTP
			Serial.print("buffer=<");
			Serial.print(buffer);
			Serial.println(">");
			Serial.print("charsReceived=<");
			Serial.print(charsReceived);
			Serial.println(">");
#endif
			sscanf_P(buffer,PSTR("%s %s\?%s"),mode, url, params);
			if (strlen(url)==1) {
				strcpy_P(url,PSTR("/index.htm"));
			}
#ifdef FULLIP_DEBUG_HTTP
			Serial.print("url=<");
			Serial.print(url);
			Serial.println(">");
			Serial.print("params=<");
			Serial.print(params);
			Serial.println(">");
#endif
			if (_iParseUrl && _iParseUrl(client,mode,url,0)>0) {
				lastTransfer=200;
				//printHeader(lastTransfer,0);
				iHTTPTimeout = millis() + FULLIP_HTTP_TIMEOUT;
				iHTTPClientUrl = (char*)malloc(strlen(url)+1);
				strcpy(iHTTPClientUrl, url);
				iHTTPClientMode = (char*)malloc(strlen(mode)+1);
				strcpy(iHTTPClientMode, mode);
				iHTTPState++;
#ifdef FULLIP_DEBUG_HTTP
				Serial.println(iHTTPState);
#endif
			} else {
				sprintf_P(page, PSTR("%s%s"), iHTTPRootDir, url);
				iHTTPFile = SD.open(page, FILE_READ);
				//Serial.println(page);
				if (iHTTPFile.available() > 0) {
					iHTTPTimeout = millis() + FULLIP_HTTP_TIMEOUT;
					//Serial.println(iHTTPFile.available());
					lastTransfer=200;
					printAutoHeader(page, lastTransfer, 0);
					iHTTPState++;
#ifdef FULLIP_DEBUG_HTTP
					Serial.println(iHTTPState);
#endif
				} else {
					///404
					strcpy_P(url,PSTR("404.htm"));
					sprintf_P(page, PSTR("%s/%s"), iHTTPRootDir, url);
					iHTTPFile = SD.open(page, FILE_READ);
					lastTransfer=404;
					printHtmlHeader(lastTransfer,0);
					if ((iHTTPFile.available()) <= 0) {
						disconnect();
					} else {
						iHTTPTimeout = millis() + FULLIP_HTTP_TIMEOUT;
						iHTTPState++;
#ifdef FULLIP_DEBUG_HTTP
						Serial.println(iHTTPState);
#endif
					}
				}
			}
		} else if ((long signed)(millis() - iHTTPTimeout) > 0) {
#ifdef FULLIP_DEBUG_HTTP
			Serial.println("Http timeout");
#endif
			disconnect();
		}
	} else if (iHTTPState==2) {
		if (_iParseUrl && iHTTPClientUrl && _iParseUrl(client,iHTTPClientMode,iHTTPClientUrl,1)>0) {
			iHTTPTimeout = millis() + FULLIP_HTTP_TIMEOUT;
		} else if (iHTTPFile.available() > 0) {
			iHTTPTimeout = millis() + FULLIP_HTTP_TIMEOUT;
			int size=0;
			if (iHTTPClientBinary==1) {
				uint8_t* msg = (uint8_t*)malloc(FULLIP_HTTP_DATA_SIZE);
				size=iHTTPFile.read(msg,FULLIP_HTTP_DATA_SIZE);
				client.write(msg,size);
				free(msg);
			} else {
				char* msg = (char*)malloc(FULLIP_HTTP_DATA_SIZE);
				char c = '\0';
				while (iHTTPFile.available()>0 && size<FULLIP_HTTP_DATA_SIZE && c!='\n') {
					c = iHTTPFile.read();
					msg[size] = c;
					size++;
				}
				msg[size] = '\0';
				size--;
				if (iHTTPClientBinary==0) {
					client.write((uint8_t*)msg,size);
				} else {
					iParsePage(client, msg, size);
				}
				free(msg);
			}
			//size=iHTTPFile.read(msg,FULLIP_HTTP_DATA_SIZE);
#ifdef FULLIP_DEBUG_HTTP
			Serial.print("Bytes sent to client : ");
			Serial.println(size);
			//Serial.write(msg,size);
#endif
		} else {
			iHTTPTimeout = millis() + FULLIP_HTTP_TIMEOUT;
#ifdef FULLIP_DEBUG_HTTP
			Serial.println("Quit connection");
			//Serial.write(msg,size);
#endif
			if (iHTTPFile) {
				iHTTPFile.close();
			}
			iHTTPState++;
#ifdef FULLIP_DEBUG_HTTP
			Serial.println(iHTTPState);
#endif
		}
	} else if (iHTTPState==3 ) {
			iHTTPTimeout = millis() + FULLIP_HTTP_TIMEOUT;
			iHTTPClose = millis() + FULLIP_HTTP_CLOSE;
			iHTTPState++;
#ifdef FULLIP_DEBUG_HTTP
			Serial.println(iHTTPState);
#endif
	} else if (iHTTPState==4 && (long signed)(millis() - iHTTPClose) > 0) {
		free(iHTTPClientUrl);
		free(iHTTPClientMode);
		client.stop();
		iHTTPState = 0;
#ifdef FULLIP_DEBUG_HTTP
	    Serial.println(iHTTPState);
		Serial.println("Http client connection closed");
#endif
	}
}

///Show if a client is connected
uint8_t HttpServer::connected()
{
	return (iHTTPState==1);
}				

///Disconnect the client
void HttpServer::disconnect()
{
	iHTTPClose = millis() + FULLIP_HTTP_CLOSE;
	iHTTPTimeout = millis() + FULLIP_HTTP_TIMEOUT;
	iHTTPState = 4;
}				
#endif
