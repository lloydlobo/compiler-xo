#!/usr/bin/env bash

cmake -S . -B build && cmake --build build
valgrind ./build/xo test.xo
echo $?

./out
echo $?
