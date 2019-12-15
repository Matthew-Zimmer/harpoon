#!/bin/bash

if ! test -d bin
then
    mkdir bin
fi
./update.bash
cd bin
cmake ..
make