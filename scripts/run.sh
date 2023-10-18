#!/usr/bin/env bash

cmake -S . -B build && cmake --build build
./build/lumina test.lum
echo $?

./out
echo $?
