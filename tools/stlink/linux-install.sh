#!/bin/sh

STLINK=$NOS_HOME/tools/stlink

echo "Install libusb ..."

tar xvf libusb-1.0.9.tar

cd libusb-1.0.9

./configure --prefix=$STLINK

make

make install

cd ..

echo "Install stlink ..."

unzip stlink-master.zip

cd stlink-master

./autogen.sh

export PKG_CONFIG_PATH=$STLINK/lib/pkgconfig

./configure --prefix=$STLINK

make

make install

cd ..

