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

Put a mail in the queue
.. code-block:: c

smtpclient.sendMail(from,to,subject,body)

And send it in your loop
.. code-block:: c

smtpclient.maintain();

Class definition
================

.. doxygenclass:: FtpClient
   :project: fullip
   :members:

