#!/bin/bash
set -xe

# git clone https://github.com/mxe/mxe.git
# cd mxe && make qtbase
# make -j 4 qt qt5 MXE_TARGETS='x86_64-w64-mingw32.shared i686-w64-mingw32.shared x86_64-w64-mingw32.static i686-w64-mingw32.static'
# make qtbase

# set correct path
MXE_ROOT=$1
export PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:$MXE_ROOT/usr/bin

# run make file generator
cmd=$MXE_ROOT/usr/bin/i686-w64-mingw32.static-qmake-qt5
$cmd

# clean and make
make clean
make
