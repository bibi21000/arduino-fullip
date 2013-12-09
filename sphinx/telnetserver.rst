====================
FullIP Telnet Server
====================


Installation
============

Download and install FullIP (Look at "Installation instructions").

Comment #define FULLIP_SD to disable SD in FullIP.h.


Usage
=====

- Define a variable for the Telnet server :

.. code-block:: c

 TelnetServer telnet;

- Start the Telnet server :

.. code-block:: c

 telnet.begin(parse);

- And process client connections in your loop :

.. code-block:: c

 telnet.maintain();

- Define a callback function to parse the client commands and send result to it :

.. code-block:: c

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

This function must return 1 if the command is recognized and 0 if not.

Class definition
================

.. doxygentypedef:: telnetParseCommand
   :project: fullip

.. doxygenclass:: TelnetServer
   :project: fullip
   :members:
