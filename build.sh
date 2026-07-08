#!/bin/sh
set -eu

build_exe="./build"
build_src="build.c"

if [ ! -x "$build_exe" ] || [ "$build_src" -nt "$build_exe" ]; then
    cc -std=c11 -Wall -Wextra -o "$build_exe" "$build_src"
fi

"$build_exe" "$@"
