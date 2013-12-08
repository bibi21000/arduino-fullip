/* 
 FullIP.
 
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


#include "FullIP.h"
#ifdef FULLIP_ETHERNET
#include "Ethernet.h"
#include "Dns.h"
#endif
#ifdef FULLIP_UIP
#include "UIPEthernet.h"
#include "Dns.h"
#endif
#include "utility/SmtpClient.h"

#define FULLIP_SMTP_LAST_STATE 10

/// Initialize SMTP server and domain's sender
uint8_t SmtpClient::begin(const IPAddress& aSMTPServer, const char *domainstr)
{
  iSMTPServer = aSMTPServer;
  iSMTPDomain = (char*)malloc(strlen(domainstr)+1);
  strcpy(iSMTPDomain, domainstr);
  return 1;
}

/// Initialize SMTP server, domain's sender and auth
/// auth must be encoded
uint8_t SmtpClient::begin(const IPAddress& aSMTPServer, const char *domainstr, const char *authstr)
{
  iSMTPAuth = (char*)malloc(strlen(authstr)+1);
  strcpy(iSMTPAuth, authstr);
  return begin(aSMTPServer, domainstr);;
}

/// Release resources
/// Must be called if you don't want to send email anymore
uint8_t SmtpClient::end()
{
  free(iSMTPDomain);
  if (iSMTPAuth) 
	free(iSMTPAuth);
  return 1;
}

/// Resolve the SMPT's server name, initialize SMTP server and domain's sender
uint8_t SmtpClient::begin(const char *aSMTPHost, const char *domainstr)
{
  // Look up the host first
  int ret = 0;
  DNSClient dns;
  IPAddress remote_addr;
  dns.begin(Ethernet.dnsServerIP());
  ret = dns.getHostByName(aSMTPHost, remote_addr);
  if (ret == 1) {
    return begin(remote_addr, domainstr);
  }
  return ret;
}

/// Initialize SMTP server name, domain's sender and auth
/// auth must be encoded
uint8_t SmtpClient::begin(const char *aSMTPHost, const char *domainstr, const char *authstr)
{
  iSMTPAuth = (char*)malloc(strlen(authstr)+1);
  strcpy(iSMTPAuth, authstr);
  return begin(aSMTPHost, domainstr);;
}

///Send a mail
uint8_t SmtpClient::sendMail(const char *from, const char *to, const char *subject,  const char *body)
{
	if (iSMTPState != 0 && iSMTPState != FULLIP_SMTP_LAST_STATE) {
		return 0;
	}
    if (iSMTPClient.connect(iSMTPServer,25)) {
#ifdef FULLIP_DEBUG_SMTP
		Serial.println("SMTP connection opened");
#endif
		iSMTPState = 1;
	    iSMTPFrom = (char*)malloc(strlen(from)+1);
	    strcpy(iSMTPFrom, from);
	    iSMTPTo = (char*)malloc(strlen(to)+1);
	    strcpy(iSMTPTo, to);
	    iSMTPSubject = (char*)malloc(strlen(subject)+1);
	    strcpy(iSMTPSubject, subject);
	    iSMTPBody = (char*)malloc(strlen(body)+1);
	    strcpy(iSMTPBody, body);
	    iSMTPWaitForCode=220;
	    lastResponse=0;
        iSMTPTimeout = millis() + FULLIP_SMTP_TIMEOUT;
		return 1;
    } else {
        lastResponse=421;
        return 0;
	}
}

///Must be called in your loop
///Send the mail in "background"
void SmtpClient::maintain()
{
	if (iSMTPState == 0 || iSMTPState == FULLIP_SMTP_LAST_STATE) {
		return;
	}
	Ethernet.maintain();
	char scode[4];
	if ((long signed)(millis() - iSMTPTimeout) > 0) {
#ifdef FULLIP_DEBUG_SMTP
		Serial.println("SMTP timeout");
#endif
		disconnect();
		lastResponse=554;
		iSMTPState = 0;
	}
	if ((iSMTPState == (FULLIP_SMTP_LAST_STATE-1)) && (long signed)(millis() - iSMTPClose) > 0) {
#ifdef FULLIP_DEBUG_SMTP
		Serial.println("Close SMTP connection");
#endif
		iSMTPState = FULLIP_SMTP_LAST_STATE;
		disconnect();
	}
	while((iSMTPClient.available()) > 0)
		{
	      //Only retrieve the first uint8_t of the response
	      int size;
		  uint8_t* msg = (uint8_t*)malloc(FULLIP_SMTP_RESPONSE_SIZE);
		  size=iSMTPClient.read(msg,FULLIP_SMTP_RESPONSE_SIZE);
#ifdef FULLIP_DEBUG_SMTP
		  Serial.print("Message from server : ");
		  Serial.write(msg,size);
#endif
		  strncpy(scode,(const char *)msg,3);
		  lastResponse=atoi(scode);
#ifdef FULLIP_DEBUG_SMTP
		  Serial.print("lastResponse ");
		  Serial.println(lastResponse);
#endif
		  if (iSMTPWaitForCode==lastResponse) {
			  iSMTPState++;
		  } else {
			  iSMTPState=FULLIP_SMTP_LAST_STATE-1;
		  }
	      //And remove the last bytes in the client queue
		  while((iSMTPClient.available()) > 0)
				iSMTPClient.read(msg,1);
		  free(msg);
 		  if ((iSMTPState == 3) && (!iSMTPAuth)) {
			 // Jump to state 4 if no auth
			 iSMTPState = 4;
		  }
		  char buffer[15];
		  if (iSMTPState==2) {
			  strcpy_P(buffer,PSTR("EHLO "));
			  iSMTPClient.print(buffer);
			  iSMTPClient.print(iSMTPDomain);
			  strcpy_P(buffer,PSTR("\n"));
			  iSMTPClient.print(buffer);
			  iSMTPTimeout = millis() + FULLIP_SMTP_TIMEOUT;
			  iSMTPWaitForCode=250;
		  } else if (iSMTPState==3) {
			  strcpy_P(buffer,PSTR("AUTH PLAIN "));
			  iSMTPClient.print(buffer);
			  iSMTPClient.print(iSMTPAuth);
			  iSMTPTimeout = millis() + FULLIP_SMTP_TIMEOUT;
			  strcpy_P(buffer,PSTR("\n"));
			  iSMTPWaitForCode=235;
		  } else if (iSMTPState==4) {
			  strcpy_P(buffer,PSTR("MAIL FROM:<"));
			  iSMTPClient.print(buffer);
			  iSMTPClient.print(iSMTPFrom);
			  strcpy_P(buffer,PSTR(">\n"));
			  iSMTPClient.print(buffer);
			  iSMTPTimeout = millis() + FULLIP_SMTP_TIMEOUT;
			  iSMTPWaitForCode=250;
		  } else if (iSMTPState==5) {
			  strcpy_P(buffer,PSTR("RCPT TO:<"));
			  iSMTPClient.print(buffer);
			  iSMTPClient.print(iSMTPTo);
			  strcpy_P(buffer,PSTR(">\n"));
			  iSMTPClient.print(buffer);
			  iSMTPTimeout = millis() + FULLIP_SMTP_TIMEOUT;
			  iSMTPWaitForCode=250;
		  } else if (iSMTPState==6) {
			  strcpy_P(buffer,PSTR("DATA\n"));
			  iSMTPClient.print(buffer);
			  iSMTPTimeout = millis() + FULLIP_SMTP_TIMEOUT;
			  iSMTPWaitForCode=354;
		  } else if (iSMTPState==7) {
			  strcpy_P(buffer,PSTR("FROM:<"));
			  iSMTPClient.print(buffer);
			  iSMTPClient.print(iSMTPFrom);
			  strcpy_P(buffer,PSTR(">\n"));
			  iSMTPClient.print(buffer);
			  strcpy_P(buffer,PSTR("TO:<"));
			  iSMTPClient.print(buffer);
			  iSMTPClient.print(iSMTPTo);
			  strcpy_P(buffer,PSTR(">\n"));
			  iSMTPClient.print(buffer);
			  strcpy_P(buffer,PSTR("SUBJECT:"));
			  iSMTPClient.print(buffer);
			  iSMTPClient.print(iSMTPSubject);
			  strcpy_P(buffer,PSTR("\n"));
			  iSMTPClient.print(buffer);
			  strcpy_P(buffer,PSTR(""));
			  iSMTPClient.println(buffer);
			  iSMTPClient.print(iSMTPBody);
			  strcpy_P(buffer,PSTR(""));
			  iSMTPClient.println(buffer);
			  strcpy_P(buffer,PSTR("."));
			  iSMTPClient.println(buffer);
			  iSMTPTimeout = millis() + FULLIP_SMTP_TIMEOUT;
			  iSMTPWaitForCode=250;
		  } else if (iSMTPState==8) {
			  strcpy_P(buffer,PSTR("QUIT\n"));
			  iSMTPClient.print(buffer);
			  iSMTPTimeout = millis() + FULLIP_SMTP_TIMEOUT;
			  iSMTPWaitForCode=221;
		  } else if (iSMTPState==9) {
			  iSMTPTimeout = millis() + FULLIP_SMTP_TIMEOUT;
			  iSMTPClose = millis() + FULLIP_SMTP_CLOSE;
		  }
		}
}

///Disconnect from the SMPT server
void SmtpClient::disconnect()
{	
	iSMTPClient.stop();
	free(iSMTPFrom);
	free(iSMTPTo);
	free(iSMTPSubject);
	free(iSMTPBody);
	//iSMTPState = 0;
#ifdef FULLIP_DEBUG_SMTP
	Serial.println("SMTP connection closed");
#endif
}				

///Show if the las mail was sent
uint8_t SmtpClient::isSent()
{
	return (iSMTPState==FULLIP_SMTP_LAST_STATE && iSMTPWaitForCode==221);
}				
