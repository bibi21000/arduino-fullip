#!/bin/sh
doxygen fullip.doxy
make joomla
make text
cp -f _build/text/readme.txt ../README.md
mkdir ../doc 2>/dev/null
cp -f _build/text/*.txt ../doc/
rm -f ../doc/readme.txt
rm -f ../doc/index.txt
