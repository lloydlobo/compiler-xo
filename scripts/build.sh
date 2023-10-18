#!/usr/bin/env bash

cmake -S . -B build && cmake --build build
echo $?
