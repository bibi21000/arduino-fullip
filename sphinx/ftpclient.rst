=================
FullIP FTP Client
=================

Installation
============

Download and install FullIP (Look at :doc:`installation`).

Uncomment #define FULLIP_SD to enable SD in FullIP.h.

Usage
=====

Define a variable dor the FTP client

.. code-block:: c

 FtpClient ftpclient;

Start the FTP client

.. code-block:: c

 ftpclient.begin(server_name,user_name,password);

Send a file 

.. code-block:: c

 ftpclient.sendFile(filename2,local_dir,remote_dir)

Or retrieve one 

.. code-block:: c

 ftpclient.getFile(filename2,local_dir,remote_dir)

And process it in your loop

.. code-block:: c

 ftpclient.maintain();

Class definition
================

.. doxygenclass:: FtpClient
   :project: fullip
   :members:

