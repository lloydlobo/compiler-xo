#!/usr/bin/env bash

cmake -S . -B build && cmake --build build
valgrind -s --track-origins=yes --leak-check=full ./build/lumina test.lum
echo $?

./out
echo $?
