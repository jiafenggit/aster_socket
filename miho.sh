#!/bin/bash
#cp -Rf app_socket.c ../asterisk-13.4.0/apps/
#cd ../asterisk-13.4.0/
#make
gcc -g -Wall -D_REENTRANT -D_GNU_SOURCE -fPIC -DAST_MODULE=\"app_socket\" -c -o app_socket.o app_socket.c
gcc -shared -o app_socket.so  app_socket.o
/etc/init.d/asterisk stop
cp -Rf app_socket.so /usr/lib64/asterisk/modules/
/etc/init.d/asterisk start
