#!/bin/bash

mkdir bin 2> /dev/null
./update.bash
cd bin
cmake ..
make