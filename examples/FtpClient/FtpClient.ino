/*
 * FullIP FtpClient example.
 *
 * FullIP is a suite of networks applications for Arduino.
 *
 * It needs :
 * - the official ethernet library for W5100
 * - the UIPEthernet library for enc28j60. Download and install it : https://github.com/ntruchsess/arduino_uip
 *
 *      -----------------
 *
 * This FTPClient example gets its local ip-address via dhcp and send
 * file store on your SD every 30 seconds.
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
#define FULLIP_UPLOAD
//#define FULLIP_DOWNLOAD

FtpClient ftpclient;
unsigned long next;
//The name of your FTP server
char server_name[] = "server";
//The domain that your emails will come from
char user_name[] = "user";
//The sender
char password[] = "pass";
//The receiver
char local_dir[] = "config";
//The subject
char remote_dir[] = "temp";
//And the body
char filename1[] = "article.pdf";
char filename2[] = "archive.rar";

void setup() {

  Serial.begin(115200);
  Serial.println("[FtpClient]");
  //SPI.setClockDivider(SPI_CLOCK_DIV2);

  uint8_t mac[6] = {0x00,0x02,0x02,0x03,0x04,0x05};
  Ethernet.begin(mac);

  Serial.print("localIP: ");
  Serial.println(Ethernet.localIP());
  pinMode(PIN_SD_CS, OUTPUT);
  if (!SD.begin(PIN_SD_CS)) {
      Serial.println("SD failed :");
  } else {
      Serial.println("SD configured :)");
  }
  ftpclient.begin(server_name,user_name,password);
  next = 0;
  delay(500);
}

void loop() {
  //Call this function in your loop to send message in the "background"
  ftpclient.maintain();

  if (((signed long)(millis() - next)) > 0)
    {
      Serial.print("freeMemory()=");
      Serial.println(freeMemory());
#ifdef FULLIP_UPLOAD
      if (ftpclient.isSent()) {
#endif
#ifdef FULLIP_DOWNLOAD
      if (ftpclient.isReceived()) {
#endif
          Serial.println("File was transfered");
#ifdef FULLIP_DEBUG_STATS
          Serial.print("Size : ");
          Serial.print(ftpclient.stats_size/1024);
          Serial.println(" kbytes");
          Serial.print("Duration : ");
          Serial.print((float)ftpclient.stats_duration/1000);
          Serial.println(" seconds");
          Serial.print("Speed : ");
          Serial.print(((float)ftpclient.stats_size/1024)/((float)ftpclient.stats_duration/1000));
          Serial.println(" kbytes/seconds");
#endif

      } else {
          Serial.println("File was NOT transfered");
      }
     // Serial.print("freeMemory()=");
      //Serial.println(freeMemory());
      //Serial.print("lastResponse=");
      //Serial.println(ftpclient.lastResponse);
      Serial.print("lastTransfer=");
      Serial.println(ftpclient.lastTransfer);
      next = millis() + 30000;
#ifdef FULLIP_UPLOAD
      Serial.println("FTP Client try to upload file");
#endif
#ifdef FULLIP_DOWNLOAD
      Serial.println("FTP Client try to download file");
#endif
#ifdef FULLIP_UPLOAD
      if (ftpclient.sendFile(filename2,local_dir,remote_dir)==1)
#endif
#ifdef FULLIP_DOWNLOAD
      if (ftpclient.getFile(filename2,local_dir,remote_dir)==1)
#endif
        {
          Serial.println("File is in queue");
        }
      else
        Serial.println("Can't transfer file");
    }
}
