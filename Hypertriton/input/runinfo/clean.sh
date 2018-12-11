#! /bin/bash

for directory in csh err list out report
do
    rm -rf ${directory}/*
    touch ${directory}/README
done


echo "=============================END==============================="
