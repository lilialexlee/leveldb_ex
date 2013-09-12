#!/bin/bash

if [ -d out ];
then
rm -r out
fi

rm -f *.Makefile *.mk Makefile *.user *.sln *.vcproj *.suo *.ncb *.rules *.XML
