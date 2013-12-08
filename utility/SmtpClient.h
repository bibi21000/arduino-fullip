/* 
 FullIP.
 
 https://github.com/bibi21000/arduino-fullip
 
 FullIP is a suite of ip protocols implementation.
 It can be use the official ethernet shield
 And with Enc28j60
 
 Copyright (C) 2013 by Sébastien GALLET <bibi21000@gmail.com>
 
 -----------------
 
 SMTP Client implementation
 
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
\example examples/SmtpClient/SmtpClient.ino
 */

#ifndef SMTPCLIENT_H
#define SMTPCLIENT_H

/// If your connection is slow, try to update this parameter
#define FULLIP_SMTP_TIMEOUT 5000
///\def  If you've got a memory leak, try to update this parameter
#define FULLIP_SMTP_CLOSE 250
/// Only the 5 first bytes of the server's response are read
/// Update this parameter to get ùpre char
#define FULLIP_SMTP_RESPONSE_SIZE 5

#include "FullIP.h"
#ifdef FULLIP_ETHERNET
#include "EthernetClient.h"
#endif
#ifdef FULLIP_UIP
#include "UIPClient.h"
#endif

///\brief The SMTP client.
class SmtpClient {
public:

/// Initialize the smtp client.
/// @param aSMTPServer the IPAddress of the SMTP server.
/// @param domainstr the domain to use to send emails.
/// @return 1 if the operation succeed. 0 if it fails.
    uint8_t begin(const IPAddress& aSMTPServer, const char *domainstr);

/// Initialize the smtp client.
/// @param aSMTPServer the IPAddress of the SMTP server.
/// @param domainstr the domain to use to send emails.
/// @param authstr the auth string to use to authenticate.
/// @return 1 if the operation succeed. 0 if it fails.
    uint8_t begin(const IPAddress& aSMTPServer, const char *domainstr, const char *authstr);
    
/// Initialize the smtp client.
/// @param aSMTPHost the host name of the SMTP server.
/// @param domainstr the domain to use to send emails.
/// @return 1 if the operation succeed. 0 if it fails.
    uint8_t begin(const char *aSMTPHost, const char *domainstr);
    
/// Initialize the smtp client.
/// @param aSMTPHost the host name of the SMTP server.
/// @param domainstr the domain to use to send emails.
/// @param authstr the auth string to use to authenticate (use the following command
/// to generate it : perl -MMIME::Base64 -e 'print encode_base64("\000myuser\@yourisp.com\000password")').
/// @return 1 if the operation succeed. 0 if it fails.
    uint8_t begin(const char *aSMTPHost, const char *domainstr, const char *authstr);

/// Release resources.
/// Must be called if you don't want to send email anymore.
/// @return 1 if the operation succeed. 0 if it fails.
    uint8_t end();

/// Send a mail.
/// @param from the sender of the email.
/// @param to the receiver of the email.
/// @param subject the subject of the email.
/// @param body the body of the email.
/// @return 1 if the email is in the queue. 0 if it fails.
    uint8_t sendMail(const char *from, const char *to, 
				 const char *subject,  const char *body);

/// Send the email in "background".
/// Must be called in your loop.
    void maintain();
    
/// Was the las email sent ?
/// @return 1 if the email was sent.
    uint8_t isSent();

/// The last response code sent by the server.
    long lastResponse;
    
private:
/// Disconnect the client.
	void disconnect();
	
protected:
/// The IPAdresse of the SMTP server.
    IPAddress iSMTPServer;
/// The client connected.
    EthernetClient iSMTPClient;
/// The domain used to send the mail.
    char* iSMTPDomain;
/// The auth string.
    char* iSMTPAuth;
/// The sender
    char* iSMTPFrom;
/// The receiver
    char* iSMTPTo;
/// The body.
    char* iSMTPBody;
/// The subject.
    char* iSMTPSubject;
/// The current state in the state machine.
    uint8_t iSMTPState;
/// The server code we wait for.
    long iSMTPWaitForCode;
/// The next timeout.
    unsigned long iSMTPTimeout;
/// The next delayed connection close.
    unsigned long iSMTPClose;
};

#endif
