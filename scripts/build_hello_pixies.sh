#!/bin/bash

TARGET_BUILD_FOLDER=../build

mkdir $TARGET_BUILD_FOLDER
mkdir $TARGET_BUILD_FOLDER/hello_pixies

cd $TARGET_BUILD_FOLDER/hello_pixies
cmake ../../src/host/hello_pixies
make
