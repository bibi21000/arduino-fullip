/*
 * FullIP Telnet server example.
 *
 * FullIP is a suite of ip protocols implementation.
 * It can be use the official ethernet shield
 * And with Enc28j60
 *
 *      -----------------
 *
 * This Telnet Server example gets its local ip-address.
 * Connect to it using : telnet X.X.X.X 23
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
 * Copyright (C) 2013 by Sébastien GALLET <bibi21000@gmail.com>
 */

// For W5100
//You must also update FullIP/FullIP.h
#include <SPI.h>
#include <Ethernet.h>

//For enc28j60
//You must also update FullIP/FullIP.h
//#include <UIPEthernet.h>
#include <FullIP.h>
#include <MemoryFree.h>

TelnetServer telnet;
unsigned long next;

void setup() {

  Serial.begin(115200);
  Serial.println("[TelnetServer]");

  uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x05};
  Ethernet.begin(mac);

  Serial.print("localIP: ");
  Serial.println(Ethernet.localIP());
  telnet.begin(parse);
  Serial.println("Server started");
  next = 0;
}

void loop() {
    //Call this function in your loop to process request from telnet clients
    telnet.maintain();
    if (((signed long)(millis() - next)) > 0) {
        Serial.print("freeMemory()=");
        Serial.println(freeMemory());
        if (telnet.connected()) {
          Serial.println("A client is connected");
        }
        next = millis() + 10000;
    }
}

byte parse(char buffer[]) {
  char buff2[4];
  int pin;
  if (strcmp_P(buffer,PSTR(""))==0) {
      telnet.client.println("Here is my telnet");
      return 1;
  } else if (strcmp_P(buffer,PSTR("?"))==0) {
      telnet.client.println("get pin XX : show the state of the pin XX");
      telnet.client.println("set pin XX : change the state of the pin XX");
      return 1;
  } else if (strncmp_P(buffer,PSTR("get pin "),8)==0) {
      buff2[0]=buffer[8];
      buff2[1]=buffer[9];
      buff2[2]='\0';
      pin=atoi(buff2);
      int val=digitalRead(pin);
      telnet.client.print("pin ");
      telnet.client.print(buff2);
      telnet.client.print(" : ");
      if (digitalRead(pin)==HIGH) {
        telnet.client.println("HIGH");
      } else {
        telnet.client.println("LOW");
      }
      return 1;
  } else if (strncmp_P(buffer,PSTR("set pin "),8)==0) {
      buff2[0]=buffer[8];
      buff2[1]=buffer[9];
      buff2[2]='\0';
      pin=atoi(buff2);
      int val=digitalRead(pin);
      telnet.client.print("pin ");
      telnet.client.print(buff2);
      telnet.client.print(" : ");
      if (digitalRead(pin)==HIGH) {
        digitalWrite(pin,LOW);
        telnet.client.println("LOW");
      } else {
        digitalWrite(pin,HIGH);
        telnet.client.println("HIGH");
      }
      return 1;
  }
  return 0;
}
