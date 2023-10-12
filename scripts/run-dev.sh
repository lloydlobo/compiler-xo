#!/usr/bin/env bash

cmake -S . -B build && cmake --build build
valgrind ./build/lumina test.lum
echo $?

./out
echo $?
