
Welcome to FullIP
*****************

FullIP for Arduino (https://github.com/bibi21000/arduino-fullip)

FullIP is a suite of ip protocols implementation for Arduino. It's
compatible with Ethernet/W5100 and UIPEthernet/ENC28j60.

Applications availables :

* FTP client

* SMTP client

* Telnet Server

* HTTP server : under developpment

It can be use the official ethernet shield and with Enc28j60.

Copyright (C) 2013 by Sébastien GALLET <bibi21000@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.


Quick start
===========

Install FullIP:

* Download the library from github (https://github.com/bibi21000
  /arduino-fullip) and install it in your libraries

* Update FullIP.h according to your hardware

     * comment #define FULLIP_UIP to build with official shield
       (w5100) or uncomment it to build with enc28j60 and UIPEthernet

     * comment #define FULLIP_SD to disable SD or uncomment it to
       build ftpclient, httpserver, ...

In your sketch :

* Define a client (FTP in this example)

   FtpClient ftpclient;

* Start it in your setup

   ftpclient.begin(server_name,user_name,password);

* Get the file

   ftpclient.getFile(filename,local_dir,remote_dir);

* And process it in your loop

   ftpclient.maintain();

That's all !!!

Look at documentation in the doc directory and examples to get more
informations.
You can also browse the documentation online here :
http://bibi21000.gallet.info/index.php/fr/component/sphinxdoc/documentation/8-fullip-for-arduino/readme.html
