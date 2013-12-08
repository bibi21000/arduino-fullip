/* 
 FullIP.
 
 https://github.com/bibi21000/arduino-fullip
 
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

 -----------------

 Tests process :
 - Transmit a file
 - Try to transmit a non existing file
 - Decrease TIMEOUT to create a timeout
 - Change access rights (-w) on file to create an access denied
*/

/*!
\example examples/FtpClient/FtpClient.ino
 */

#ifndef FTPCLIENT_H
#define FTPCLIENT_H

/// Timeout for a FTP session
#define FULLIP_FTP_TIMEOUT 3000
//#define FULLIP_FTP_TIMEOUT 10000
/// If you've got a memory leak, try to update this parameter
#define FULLIP_FTP_CLOSE 250
/// Length of data sent on every call
//#define FULLIP_FTP_DATA_SIZE 700
#define FULLIP_FTP_DATA_SIZE 400
/// Length of data response read
//#define FULLIP_FTP_RESPONSE_SIZE 120
#define FULLIP_FTP_RESPONSE_SIZE 50
/// Adds stats to evaluate performances
#define FULLIP_DEBUG_STATS

#include "FullIP.h"
#ifdef FULLIP_ETHERNET
#include "EthernetClient.h"
#endif
#ifdef FULLIP_UIP
#include "UIPClient.h"
#endif
#ifdef FULLIP_SD
  #include "SD.h"
#endif

///\brief  The FTP client.
///
class FtpClient {
public:
/// Initialize FTP the ftp client
/// @param aFTPServer the IPAddress of the FTP server.
/// @param userstr the user used to connect to the server.
/// @param passstr the password of the user.
/// @return 1 if the operation succeed. 0 if it fails.
    uint8_t begin(const IPAddress& aFTPServer, const char *userstr, const char *passstr);
    
/// Initialize FTP the ftp client
/// @param aFTPHost the host name of the FTP server.
/// @param userstr the user used to connect to the server.
/// @param passstr the password of the user.
/// @return 1 if the operation succeed. 0 if it fails.
    uint8_t begin(const char *aFTPHost, const char *userstr, const char *passstr);

/// Release ressources
/// Must be called if you don't want to send file anymore
    uint8_t end();

/// Send a file
/// @param filename the name of the file to transfer.
/// @param localdir the local directory.
/// @param remotedir the remote directory.
/// @return 1 if the file is in queue. 0 if it fails.
    uint8_t sendFile(const char *filename, const char *localdir, const char *remotedir);
    
/// Get a file
/// @param filename the name of the file to transfer.
/// @param localdir the local directory.
/// @param remotedir the remote directory.
/// @return 1 if the file is in queue. 0 if it fails.
    uint8_t getFile(const char *filename, const char *localdir, const char *remotedir);

/// Send the file in "background"
/// Must be called in your loop
    void maintain();
    
/// Was the last file sent ?
/// @return 1 if the file was sent.
    uint8_t isSent();

/// Was the last file received ?
/// @return 1 if the file was received.
    uint8_t isReceived();

/// The last response code sent by the server
    long lastResponse;
    
/// The last response code sent by the server for the file transfer
    long lastTransfer;

#ifdef FULLIP_DEBUG_STATS
/// The start of the transfer. Only available is compiled with FULLIP_DEBUG_STATS directive.
    unsigned long stats_start;
/// The duration of the transfer. Only available is compiled with FULLIP_DEBUG_STATS directive.
    unsigned long stats_duration;
/// The size of data sent. Only available is compiled with FULLIP_DEBUG_STATS directive.
    unsigned long stats_size;
#endif

private:
/// Disconnect the client
	void disconnect();
/// Send a quit command to the server.
	void quit();
	
protected:
/// The IPAdresse of the server.
    IPAddress iFTPServer;
/// The command client.
    EthernetClient iFTPClientCmd;
/// The data client
    EthernetClient iFTPClientData;
#ifdef FULLIP_SD
/// The file on the SD.
    File iFTPFile;
#endif
/// This is an upload.
    uint8_t iUpload;
/// This is a download.
    uint8_t iDownload;
/// The user used to connect to the server
    char* iFTPUser;
/// The password
    char* iFTPPassword;
/// The name of the file.
    char* iFTPFilename;
/// The local directory.
    char* iFTPLocalDir;
/// The remote directory.
    char* iFTPRemoteDir;
/// The current state in the state machine.
    uint8_t iFTPState;
/// The server code wher waiting for.
    long iFTPWaitForCode;
/// The next timeout.
    unsigned long iFTPTimeout;
/// The next delayed connection close.
    unsigned long iFTPClose;
};



#endif
