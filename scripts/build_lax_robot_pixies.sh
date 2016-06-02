#!/bin/bash

TARGET_BUILD_FOLDER=../build

mkdir $TARGET_BUILD_FOLDER
mkdir $TARGET_BUILD_FOLDER/lax_robot_pixies

cd $TARGET_BUILD_FOLDER/lax_robot_pixies
cmake ../../src/host/lax_robot_pixies
make
