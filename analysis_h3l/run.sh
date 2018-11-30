#!/bin/bash


make distclean
qmake
make -j4
./analysis ./input/data.list ./output/log.out 
