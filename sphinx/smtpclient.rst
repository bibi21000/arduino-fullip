==================
FullIP SMTP Client
==================


Installation
============

Download and install FullIP (Look at "Installation instructions").

Comment #define FULLIP_SD to disable SD in FullIP.h.


Usage
=====

- Define a variable for the SMTP client :

.. code-block:: c

 SmtpClient smtpclient;

- Start the SMTP client :

.. code-block:: c

 smtpclient.begin(server_name,domain_name);

- or

.. code-block:: c

 smtpclient.begin(server_name,domain_name, authstr);

- If you've planned to use authentication, generate the auth string using the following command in a terminal :

.. code-block:: bash

 $ perl -MMIME::Base64 -e 'print encode_base64("\000myuser\@yourisp.com\000password")'
 $ AG15dXNlckB5b3VyaXNwLmNvbQBwYXNzd29yZA==

- Put a mail in the queue :

.. code-block:: c

 smtpclient.sendMail(from,to,subject,body)

- And send it in your loop :

.. code-block:: c

 smtpclient.maintain();


Class definition
================

.. doxygenclass:: SmtpClient
   :project: fullip
   :members:

