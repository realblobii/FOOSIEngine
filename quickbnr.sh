#!/bin/bash

if [ "$2" == "--clean" ]; then
    ./cmake-clean.sh
    if [ "$1" == "--gmake" ]; then
        cmake .
        make
        cd ./game
        ./build/game
    elif [ "$1" == "--ninja" ]; then
        cmake . -G Ninja
        ninja
        cd ./game
        ./build/game
    fi
else
    echo "Usage: quickbnr.sh [--gmake|--ninja] [--clean]"
    echo "  --gmake : build using GNU Make (default)"
    echo "  --ninja : build using Ninja"
    echo "  --clean : clean build files before building"
fi
