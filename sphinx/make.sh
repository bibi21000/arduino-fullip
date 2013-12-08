#!/bin/sh
doxygen fullip.doxy
make joomla
make text
cp -f _build/text/readme.txt ../README.md
mkdir ../doc 2>/dev/null
cp -f _build/text/ftpclient.txt ../doc/
cp -f _build/text/smtpclient.txt ../doc/
cp -f _build/text/telnetserver.txt ../doc/
cp -f _build/text/httpserver.txt ../doc/
#rm -Rf _build/*
