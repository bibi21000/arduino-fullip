/*
 * FullIP Telnet server example.
 *
 * FullIP is a suite of ip protocols implementation.
 * It can be use the official ethernet shield
 * And with Enc28j60
 *
 *      -----------------
 *
 * This HTTP Server example gets its local ip-address.
 * Connect to it using your web browser: http://X.X.X.X
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

// For W5100
//You must also update FullIP/FullIP.h
#include <SPI.h>
#include <Ethernet.h>

//For enc28j60
//You must also update FullIP/FullIP.h
//#include <UIPEthernet.h>

#include <SD.h>
#include <FullIP.h>
#include <MemoryFree.h>

#define PIN_SD_CS 48

unsigned long next;
HttpServer http;
char rootdir[]="HTTP";
byte urlStateMachine =0;

void setup() {
  Serial.begin(115200);
  Serial.println("[HttpServer]");
  uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x06};
  Ethernet.begin(mac);
  Serial.print("localIP: ");
  Serial.println(Ethernet.localIP());
  pinMode(PIN_SD_CS, OUTPUT);
  if (!SD.begin(PIN_SD_CS)) {
      Serial.println("SD failed :");
  } else {
      Serial.println("SD configured :)");
  }
  http.begin(rootdir, parseUrl, parsePage);
  Serial.println("Server started");
}

void loop() {
  //Call this function in your loop to send message in the "background"
    http.maintain();
    if (((signed long)(millis() - next)) > 0) {
        //Serial.print("freeMemory()=");
        //Serial.println(freeMemory());
        next = millis() + 30000;
    }
}

byte parsePage(EthernetClient client, char buffer[], int bsize)
{
  if (strcmp_P(buffer,"/state")==0) {
  }
  return 0;
}

byte parseUrl(EthernetClient client, char mode[], char url[], byte state) {
    //Serial.print("freeMemory()=");
    //Serial.println(freeMemory());
    byte ret=0;
    char buffer[80];
    if (state==0) {
        urlStateMachine=0;
        if (strcmp_P(url,PSTR("/state"))==0) {
            http.printJsonHeader(200,0);            
            strcpy_P(buffer,PSTR("{"));
            client.println(buffer);
            strcpy_P(buffer,PSTR("  rooms: ["));
            client.println(buffer);
            urlStateMachine++;
            ret=1;
          }
    } else if (state==1)
        if (strcmp_P(url,PSTR("/state"))==0) {
            if (urlStateMachine==1) {
              strcpy_P(buffer,PSTR("      {key: \"kitchen\", name: \"Cuisine\", devices: ["));
              client.println(buffer);
              sprintf_P(buffer,PSTR("        {key: \"Lampe1\", name: \"plafonnier\", state: %i},"),digitalRead(1));
              client.println(buffer);
              sprintf_P(buffer,PSTR("        {key: \"Lampe2\", name: \"applique\", state: %i}"),digitalRead(2));
              client.println(buffer);
              strcpy_P(buffer,PSTR("      ]},"));
              client.println(buffer);
              urlStateMachine++;
              ret=1;
            } else if (urlStateMachine==2) {
              strcpy_P(buffer,PSTR("      {key: \"bedroom\", name: \"Chambre a coucher\", devices: ["));
              client.println(buffer);
              sprintf_P(buffer,PSTR("        {key: \"Lampe1\", name: \"plafonnier\", state: %i},"),digitalRead(3));
              client.println(buffer);
              sprintf_P(buffer,PSTR("        {key: \"Lampe2\", name: \"applique\", state: %i}"),digitalRead(4));
              client.println(buffer);
              strcpy_P(buffer,PSTR("      ]},"));
              client.println(buffer);
              urlStateMachine++;
              ret=1;
            } else if (urlStateMachine==3) {
              strcpy_P(buffer,PSTR("      {key: \"outdoor\", name: \"Exterieur\", devices: ["));
              client.println(buffer);
              sprintf_P(buffer,PSTR("        {key: \"Lampe\", name: \"appliques\", state: %i}"),digitalRead(5));
              client.println(buffer);
              strcpy_P(buffer,PSTR("      ]}"));
              client.println(buffer);
              strcpy_P(buffer,PSTR("   ]"));
              client.println(buffer);
              strcpy_P(buffer,PSTR("}"));
              client.println(buffer);
              urlStateMachine++;
              ret=0;
            }            
        }
    return ret;
}

