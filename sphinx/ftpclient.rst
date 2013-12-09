=================
FullIP FTP Client
=================


Installation
============

Download and install FullIP (Look at "Installation instructions").

Uncomment #define FULLIP_SD to enable SD in FullIP.h.


Usage
=====

- Define a variable for the FTP client :

.. code-block:: c

 FtpClient ftpclient;

- Initialize the SD and start the FTP client :

.. code-block:: c

  pinMode(PIN_SD_CS, OUTPUT);
  if (!SD.begin(PIN_SD_CS)) {
      Serial.println("SD failed :");
  } else {
      Serial.println("SD configured :)");
  }
  ftpclient.begin(server_name,user_name,password);

- Send a file  :

.. code-block:: c

 ftpclient.sendFile(filename2,local_dir,remote_dir)

- Or retrieve one :

.. code-block:: c

 ftpclient.getFile(filename2,local_dir,remote_dir)

- And process it in your loop :

.. code-block:: c

 ftpclient.maintain();
 

Class definition
================

.. doxygenclass:: FtpClient
   :project: fullip
   :members:

