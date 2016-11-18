#!/bin/bash

TARGET_BUILD_FOLDER=../build

mkdir $TARGET_BUILD_FOLDER
mkdir $TARGET_BUILD_FOLDER/lax_robot_pixies_wash

cd $TARGET_BUILD_FOLDER/lax_robot_pixies_wash
cmake ../../src/host/lax_robot_pixies_wash
make
