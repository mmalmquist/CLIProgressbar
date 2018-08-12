#!/bin/sh

if [ ! -d Demo/build ]; then
    mkdir Demo/build
fi

cd Demo/build
cmake ../..
make
cd ../..
mv Demo/build/Demo/demo Demo
