#!/bin/bash
cp -Rf app_socket.c ../asterisk-13.4.0/apps/
cd ../asterisk-13.4.0/
make
/etc/init.d/asterisk stop
cp apps/app_socket.so /usr/lib64/asterisk/modules/
/etc/init.d/asterisk start
