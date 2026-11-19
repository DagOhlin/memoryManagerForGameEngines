#!/usr/bin/env bash
# kom ihåg permission chmod +x build.sh
set -e

if [ ! -f "build/CMakeCache.txt" ]; then
    if [ "${1,,}" = "ninja" ]; then
        cmake -B build -G Ninja
    else
        cmake -B build
    fi
fi

cmake --build build