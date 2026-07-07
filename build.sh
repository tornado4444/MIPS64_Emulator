#!/bin/sh

if[ ! -f build, ] then 
    cc build.c -o build
fi

./build "$@"