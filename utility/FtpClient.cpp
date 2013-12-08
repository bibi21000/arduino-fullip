/* 
 FullIP.
 
 FullIP is a suite of ip protocols implementation.
 It can be use the official ethernet shield
 And with Enc28j60
 
 Copyright (C) 2013 by Sébastien GALLET <bibi21000@gmail.com>
 
 -----------------

 FTP Client implementation
 
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
#include "Dns.h"
#endif
#ifdef FULLIP_UIP
#include "UIPEthernet.h"
#include "Dns.h"
#endif
#ifdef SD_H
#include "SD.h"
#endif

#include "utility/FtpClient.h"

#define FULLIP_FTP_LAST_STATE 11

#ifdef FULLIP_SD

/// Initialize FTP server and domain's sender
uint8_t FtpClient::begin(const IPAddress& aFTPServer, const char *userstr, const char *passstr)
{
	iFTPServer = aFTPServer;
	iFTPUser = (char*)malloc(strlen(userstr)+1);
	strcpy(iFTPUser, userstr);
	iFTPPassword = (char*)malloc(strlen(passstr)+1);
	strcpy(iFTPPassword, passstr);
	iFTPClose=0;
	return 1;
}

/// Release resources
/// Must be called if you don't want to send email anymore
uint8_t FtpClient::end()
{
	free(iFTPUser);
	free(iFTPPassword);
}

/// Resolve the FPT's server name, initialize FTP server, user and password
uint8_t FtpClient::begin(const char *aFTPHost, const char *userstr, const char *passstr)
{
	// Look up the host first
	int ret = 0;
	DNSClient dns;
	IPAddress remote_addr;
	dns.begin(Ethernet.dnsServerIP());
	ret = dns.getHostByName(aFTPHost, remote_addr);
	if (ret == 1) {
	return begin(remote_addr, userstr, passstr);
	}
	return ret;
}

///Send a file
uint8_t FtpClient::sendFile(const char *filename, const char *sourcedir, const char *remotedir)
{
	if (iFTPState != 0 && iFTPState != FULLIP_FTP_LAST_STATE) {
		return 0;
	}
    if (iFTPClientCmd.connect(iFTPServer,21)) {
#ifdef FULLIP_DEBUG_FTP
		Serial.println("FTP Cmd connection opened");
#endif
		iFTPState=1;
#ifdef FULLIP_DEBUG_FTP
			  Serial.println(iFTPState);
#endif
		iUpload=1;
        iDownload=0;
	    iFTPFilename = (char*)malloc(strlen(filename)+1);
	    strcpy(iFTPFilename, filename);
	    iFTPLocalDir = (char*)malloc(strlen(sourcedir)+1);
	    strcpy(iFTPLocalDir, sourcedir);
	    iFTPRemoteDir = (char*)malloc(strlen(remotedir)+1);
	    strcpy(iFTPRemoteDir, remotedir);
	    iFTPWaitForCode=220;
	    lastTransfer=0;
	    lastResponse=0;
	    iFTPClose=0;
        iFTPTimeout = millis() + FULLIP_FTP_TIMEOUT;
#ifdef FULLIP_DEBUG_STATS
		stats_start = millis();
		stats_duration = 0;
		stats_size = 0;
#endif
		return 1;
    } else {
        lastResponse=421;
        return 0;
	}
}

///Get a file
uint8_t FtpClient::getFile(const char *filename, const char *sourcedir, const char *remotedir)
{
	if (iFTPState != 0 && iFTPState != FULLIP_FTP_LAST_STATE) {
		return 0;
	}
    if (iFTPClientCmd.connect(iFTPServer,21)) {
#ifdef FULLIP_DEBUG_FTP
		Serial.println("FTP Cmd connection opened");
#endif
		iFTPState=1;
#ifdef FULLIP_DEBUG_FTP
			  Serial.println(iFTPState);
#endif
		iUpload=0;
        iDownload=1;
	    iFTPFilename = (char*)malloc(strlen(filename)+1);
	    strcpy(iFTPFilename, filename);
	    iFTPLocalDir = (char*)malloc(strlen(sourcedir)+1);
	    strcpy(iFTPLocalDir, sourcedir);
	    iFTPRemoteDir = (char*)malloc(strlen(remotedir)+1);
	    strcpy(iFTPRemoteDir, remotedir);
	    iFTPWaitForCode=220;
	    lastTransfer=0;
	    lastResponse=0;
	    iFTPClose=0;
        iFTPTimeout = millis() + FULLIP_FTP_TIMEOUT;
#ifdef FULLIP_DEBUG_STATS
		stats_start = millis();
		stats_duration = 0;
		stats_size = 0;
#endif
		return 1;
    } else {
        lastResponse=421;
        return 0;
	}
}

///Must be called in your loop
///Send the file in "background"
void FtpClient::maintain()
{
	if (iFTPState == 0 || iFTPState == FULLIP_FTP_LAST_STATE) {
		return;
	}
	Ethernet.maintain();
	char scode[4];
	if ((iFTPState == (FULLIP_FTP_LAST_STATE-1)) && (long signed)(millis() - iFTPClose) > 0) {
#ifdef FULLIP_DEBUG_FTP
		Serial.println("Close FTP connection");
#endif
		disconnect();
		return;
	}
	if ((long signed)(millis() - iFTPTimeout) > 0 && iFTPClose==0) {
#ifdef FULLIP_DEBUG_FTP
		Serial.println("FTP timeout");
#endif
		lastResponse=554;
		quit();
		return;
	}
	if (iFTPState==8) {
		if (iUpload) {
			if ((iFTPFile.available()) > 0) {
 			    iFTPTimeout = millis() + FULLIP_FTP_TIMEOUT;
				int size;
				uint8_t* msg = (uint8_t*)malloc(FULLIP_FTP_DATA_SIZE);
				size=iFTPFile.read(msg,FULLIP_FTP_DATA_SIZE);
#ifdef FULLIP_DEBUG_FTP
				//Serial.print("Bytes sent to server : ");
				//Serial.println(size);
				//Serial.write(msg,size);
#endif
#ifdef FULLIP_DEBUG_STATS
				stats_size = stats_size+size;
#endif
				iFTPClientData.write(msg,size);
				free(msg);
				//If a transfer is in progress
				//we don't read the command connection
				return;
			} else {
 			    iFTPTimeout = millis() + FULLIP_FTP_TIMEOUT;
#ifdef FULLIP_DEBUG_FTP
				//Serial.println("Upload connection closed");
				//Serial.write(msg,size);
#endif
				iFTPFile.flush();
				iFTPFile.close();
				iFTPClientData.flush();
				iFTPClientData.stop();
				//Serial.println(iFTPFile.available());
				//iFTPState++;
			}
		} else if (iDownload) {
			if((iFTPClientData.available()) > 0) {
 			    iFTPTimeout = millis() + FULLIP_FTP_TIMEOUT;
				int size;
				uint8_t* msg = (uint8_t*)malloc(FULLIP_FTP_DATA_SIZE);
				size=iFTPClientData.read(msg,FULLIP_FTP_DATA_SIZE);
#ifdef FULLIP_DEBUG_FTP
				//Serial.print("Bytes reveived from server : ");
				//Serial.println(size);
				//Serial.write(msg,size);				
				//Serial.println();
#endif
				iFTPFile.write(msg,size);
				//iFTPFile.flush();
#ifdef FULLIP_DEBUG_STATS
				stats_size = stats_size+size;
#endif
				free(msg);
				//If a transfer is in progress
				//we don't read the command connection
				return;
				}
			} else {
 			    iFTPTimeout = millis() + FULLIP_FTP_TIMEOUT;
#ifdef FULLIP_DEBUG_FTP
				//Serial.println("Data connection closed");
				//Serial.write(msg,size);
#endif
				//iFTPFile.close();
				//iFTPClientData.flush();
				//iFTPClientData.stop();
				//Serial.println(iFTPFile.available());
				//iFTPState++;
		}
	} else if (iFTPState==9 && lastTransfer!=lastResponse) {
		lastTransfer=lastResponse;
#ifdef FULLIP_DEBUG_FTP
		Serial.print("LastTransfer ");
		Serial.println(lastTransfer);
#endif
		
	}
	while((iFTPClientCmd.available()) > 0)
		{
	      //Only retrieve the first uint8_t of the response
	      int size;
		  uint8_t* msg = (uint8_t*)malloc(FULLIP_FTP_RESPONSE_SIZE);
		  size=iFTPClientCmd.read(msg,FULLIP_FTP_RESPONSE_SIZE);
#ifdef FULLIP_DEBUG_FTP
		  Serial.print("Message from server : ");
		  Serial.write(msg,size);
		  Serial.println();
#endif
		  strncpy(scode,(const char *)msg,3);
		  lastResponse=atoi(scode);
#ifdef FULLIP_DEBUG_FTP
		  Serial.print("lastResponse ");
		  Serial.println(lastResponse);
#endif
//		  if (lastResponse==421) {
//			  quit();
//#ifdef FULLIP_DEBUG_FTP
//			  Serial.println(iFTPState);
//#endif
//		  }

		  if (iFTPWaitForCode==lastResponse) {
			  iFTPState++;
#ifdef FULLIP_DEBUG_FTP
			  Serial.println(iFTPState);
#endif
			  if (iFTPState==7) {
				  char rtnVal[32]; 
				  //char buff[50]; 
				  //strncpy(buff,(const char *)msg,50);
				  sscanf_P((const char *)msg,PSTR("%*s %*s %*s %*s %s"),rtnVal);
				  unsigned int hiPort,loPort;
				  sscanf_P(rtnVal,PSTR("%*c%*d%*c%*d%*c%*d%*c%*d%*c%d%*c%d"),&hiPort,&loPort);
				  hiPort = hiPort << 8;
				  loPort = loPort & 255;
				  hiPort = hiPort | loPort;
#ifdef FULLIP_DEBUG_FTP
				  Serial.print("Port ");
				  Serial.println(hiPort);
#endif
				  if (iFTPClientData.connect(iFTPServer,hiPort)) {
#ifdef FULLIP_DEBUG_FTP
					  Serial.println("FTP Data connection opened");
#endif
					  char filepath[50];
					  sprintf_P(filepath, PSTR("%s/%s"), iFTPLocalDir, iFTPFilename);
					  if (iUpload) {
						  iFTPFile = SD.open(filepath, FILE_READ);
						  if ((iFTPFile.available()) <= 0) {
							  lastTransfer=404;
							  quit();
						  }
					  } else if (iDownload) {
						  if (SD.exists(filepath)) {
							  SD.remove(filepath);
						  }
						  iFTPFile = SD.open(filepath, FILE_WRITE);
					  }
					  iFTPTimeout = millis() + FULLIP_FTP_TIMEOUT;
				  } else {
#ifdef FULLIP_DEBUG_FTP
					  Serial.println("Can't open FTP Data connection");
#endif
 					  quit();
				  }
			  }
//		  } else if (iFTPState==8 && lastResponse==426) {
		  } else if (iFTPState==8) {
			  // continue
		  } else {
			  quit();
			  lastTransfer=lastResponse;
#ifdef FULLIP_DEBUG_FTP
			  Serial.print("Unknown response from server ");
			  Serial.println(lastResponse);
#endif
		  }
	      //And remove the last bytes in the client queue
		  while((iFTPClientCmd.available()) > 0)
				iFTPClientCmd.read(msg,1);
		  free(msg);
		  char buffer[30];
		  if (iFTPState==2) {
#ifdef FULLIP_DEBUG_FTP
			  Serial.println("USER ");
#endif
			  strcpy_P(buffer,PSTR("USER "));
			  iFTPClientCmd.print(buffer);
			  iFTPClientCmd.print(iFTPUser);
			  strcpy_P(buffer,PSTR("\n"));
			  iFTPClientCmd.print(buffer);
			  iFTPTimeout = millis() + FULLIP_FTP_TIMEOUT;
			  iFTPWaitForCode=331;
		  } else if (iFTPState==3) {
#ifdef FULLIP_DEBUG_FTP
			  Serial.println("PASS ");
#endif
			  strcpy_P(buffer,PSTR("PASS "));
			  iFTPClientCmd.print(buffer);
			  iFTPClientCmd.print(iFTPPassword);
			  iFTPTimeout = millis() + FULLIP_FTP_TIMEOUT;
			  strcpy_P(buffer,PSTR("\n"));
			  iFTPClientCmd.print(buffer);
			  iFTPWaitForCode=230;
		  } else if (iFTPState==4) {
#ifdef FULLIP_DEBUG_FTP
			  Serial.println("SYST");
#endif
			  strcpy_P(buffer,PSTR("SYST\n"));
			  iFTPClientCmd.print(buffer);
			  iFTPTimeout = millis() + FULLIP_FTP_TIMEOUT;
			  iFTPWaitForCode=215;
		  } else if (iFTPState==5) {
#ifdef FULLIP_DEBUG_FTP
			  Serial.println("CWD");
#endif
			  strcpy_P(buffer,PSTR("CWD "));
			  iFTPClientCmd.print(buffer);
			  iFTPClientCmd.print(iFTPRemoteDir);
			  strcpy_P(buffer,PSTR("\n"));
			  iFTPClientCmd.print(buffer);
			  iFTPTimeout = millis() + FULLIP_FTP_TIMEOUT;
			  iFTPWaitForCode=250;
		  } else if (iFTPState==6) {
#ifdef FULLIP_DEBUG_FTP
			  Serial.println("PASV");
#endif
			  strcpy_P(buffer,PSTR("PASV\n"));
			  iFTPClientCmd.print(buffer);
			  iFTPTimeout = millis() + FULLIP_FTP_TIMEOUT;
			  iFTPWaitForCode=227;
		  } else if (iFTPState==7) {
				if (iUpload) {
#ifdef FULLIP_DEBUG_FTP
					Serial.println("STOR");
#endif
				    strcpy_P(buffer,PSTR("STOR "));
				    iFTPClientCmd.print(buffer);
				    iFTPClientCmd.print(iFTPFilename);
				    strcpy_P(buffer,PSTR("\n"));
				    iFTPClientCmd.print(buffer);
				} else if (iDownload) {
#ifdef FULLIP_DEBUG_FTP
					Serial.println("RETR");
#endif
				    strcpy_P(buffer,PSTR("RETR "));
				    iFTPClientCmd.print(buffer);
				    iFTPClientCmd.print(iFTPFilename);
				    strcpy_P(buffer,PSTR("\n"));
				    iFTPClientCmd.print(buffer);
				}
 			    iFTPTimeout = millis() + FULLIP_FTP_TIMEOUT;
			    iFTPWaitForCode=150;
		  } else if (iFTPState==8) {
			  iFTPTimeout = millis() + FULLIP_FTP_TIMEOUT;
			  iFTPWaitForCode=226;
		  } else if (iFTPState==9) {
#ifdef FULLIP_DEBUG_FTP
			  Serial.println("QUIT");
#endif
			  strcpy_P(buffer,PSTR("QUIT\n"));
			  iFTPClientCmd.print(buffer);
			  iFTPTimeout = millis() + FULLIP_FTP_TIMEOUT;
			  iFTPWaitForCode=221;
		  } else if (iFTPState==10) {
			  iFTPTimeout = millis() + FULLIP_FTP_TIMEOUT;
			  iFTPClose = millis() + FULLIP_FTP_CLOSE;
		  }
		}
}

///Disconnect from the FTP server
void FtpClient::quit()
{	
	char buffer[10];
	strcpy_P(buffer,PSTR("QUIT\n"));
	iFTPClientCmd.print(buffer);
	iFTPState = FULLIP_FTP_LAST_STATE-1;
	iFTPClose = millis() + FULLIP_FTP_CLOSE;
#ifdef FULLIP_DEBUG_FTP
	Serial.println(iFTPState);
	Serial.println("Quit FTP connection");
#endif
}				


///Disconnect from the FTP server
void FtpClient::disconnect()
{	
#ifdef FULLIP_DEBUG_STATS
	stats_duration = millis() - stats_start;
#endif
	iFTPFile.flush();
    iFTPFile.close();
    iFTPClientData.flush();
	iFTPClientData.stop();
#ifdef FULLIP_DEBUG_FTP
	Serial.println("Data connection closed");
	//Serial.write(msg,size);
	Serial.println("QUIT");
#endif
//	if (iFTPClientCmd.connected()) {
//		char c;
//		while((iFTPClientCmd.available()) > 0) {
//			c=iFTPClientCmd.read();
//		}
//	}
	iFTPClientCmd.flush();
	iFTPClientCmd.stop();
	free(iFTPLocalDir);
	free(iFTPRemoteDir);
	free(iFTPFilename);
	iFTPClose = 0;
	iFTPState = FULLIP_FTP_LAST_STATE;
#ifdef FULLIP_DEBUG_FTP
	Serial.println(iFTPState);
#endif
	//iFTPState = 0;
#ifdef FULLIP_DEBUG_FTP
	Serial.println("FTP connection closed");
#endif
}				

///Show if the last file was sent
uint8_t FtpClient::isSent()
{
	return (iUpload==1 && iFTPState==FULLIP_FTP_LAST_STATE && lastTransfer==226);
}				

///Show if the last file was received
uint8_t FtpClient::isReceived()
{
	return (iDownload==1 && iFTPState==FULLIP_FTP_LAST_STATE && lastTransfer==226);
}				
#endif
