=================
Welcome to FullIP 
=================

FullIP for Arduino (https://github.com/bibi21000/arduino-fullip)

FullIP is a suite of ip protocols implementation for Arduino.

It can be use the official ethernet shield and with Enc28j60.

Copyright (C) 2013 by Sébastien GALLET <bibi21000@gmail.com>

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

Quick start
===========

Install FullIP:

- Download the library from github (https://github.com/bibi21000/arduino-fullip) and install it in your libraries

- Update FullIP.h according to your hardware

   - comment #define FULLIP_UIP to build with official shield (w5100) or
     uncomment it to build with enc28j60 and UIPEthernet
    
   - comment #define FULLIP_SD to disable SD or
     uncomment it to build ftpclient, httpserver, ...

In your sketch :

- import the Ethernet library or the UIPEthernet and the SD if needed :
.. code-block:: c

 // For W5100
 //You must also update FullIP/FullIP.h
 #include <SPI.h>
 #include <Ethernet.h> 
 //For enc28j60
 //You must also update FullIP/FullIP.h
 //#include <UIPEthernet.h>
 #include <SD.h>
 #include <FullIP.h>

- Define a client (FTP in this example)
.. code-block:: c

 FtpClient ftpclient;
 

- Start it in your setup
.. code-block:: c

 ftpclient.begin(server_name,user_name,password);

- Get the file
.. code-block:: c
 
 ftpclient.getFile(filename,local_dir,remote_dir);

- And process it in your loop
.. code-block:: c

 ftpclient.maintain();

That's all !!!

Look at doc and examples to get more informations.
