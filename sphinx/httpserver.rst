==================
FullIP HTTP Server
==================

(Work in progress ...)

Installation
============

Download and install FullIP (Look at "Installation instructions").

Uncomment #define FULLIP_SD to enable SD in FullIP.h.


Usage
=====

- Create a rootdir directory on your SD and copy the html files in it

- Define a variable for the HTTP server :

.. code-block:: c

 HttpServer http;

- Initialize the SD and start the HTTP server :

.. code-block:: c

  pinMode(PIN_SD_CS, OUTPUT);
  if (!SD.begin(PIN_SD_CS)) {
      Serial.println("SD failed :");
  } else {
      Serial.println("SD configured :)");
  }
  http.begin(rootdir, parseUrl, parsePage);

- And process client connections in your loop :

.. code-block:: c

  http.maintain();

- Define a callback function to parse the requested URL :

.. code-block:: c

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

- Define a callback function to parse the page content :

.. code-block:: c

	byte parsePage(EthernetClient client, char buffer[], int bsize)
	{
	  if (strcmp_P(buffer,"/state")==0) {
	  }
	  return 0;
	}


Class definition
================

.. doxygentypedef:: httpParsePage
   :project: fullip

.. doxygentypedef:: httpParseUrl
   :project: fullip

.. doxygenclass:: HttpServer
   :project: fullip
   :members:

