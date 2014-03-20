/*
 * FullIP SmtpClient example.
 *
 * FullIP is a suite of ip protocols implementation.
 * It can be use the official ethernet shield
 * And with Enc28j60
 *
 *      -----------------
 *
 * This SMTPClient example gets its local ip-address via dhcp and send
 * emails every 10 seconds.
 *
 * Copyright (C) 2013 by Sébastien GALLET <bibi21000@gmail.com>
 *
 *      -----------------
 *
 * This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include <FullIP.h>
#include <MemoryFree.h>

SmtpClient smtpclient;
unsigned long next;
//char server_name[] = "smtp.isp.mine";
//The name of your SMTP server
char server_name[] = "myservername";
//The domain that your emails will come from
char domain_name[] = "mydomain.com";
//The sender
char from[] = "myduino@mydomain.com";
//The receiver
char to[] = "me@example.com";
//The subject
char subject[] = "This is the subject";
//And the body
char body[] = "And here is the body";

void setup() {

  Serial.begin(115200);
  Serial.println("[SmtpClient]");

  uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x05};
  Ethernet.begin(mac);

  Serial.print("localIP: ");
  Serial.println(Ethernet.localIP());
  smtpclient.begin(server_name,domain_name);
  next = 0;
  delay(500);
}

void loop() {
  //Call this function in your loop to send message in the "background"
  smtpclient.maintain();

  if (((signed long)(millis() - next)) > 0)
    {
      Serial.print("freeMemory()=");
      Serial.println(freeMemory());
      if (smtpclient.isSent()) {
          Serial.println("Message was sent");
      } else {
          Serial.println("Message was NOT sent");
      }
      next = millis() + 10000;
      Serial.println("SMTP Client try to connect to server");
      if (smtpclient.sendMail(from,to,subject,body)==1)
        {
          Serial.println("Message is in queue");
        }
      else
        Serial.println("Can't connect to server");
    }
}
