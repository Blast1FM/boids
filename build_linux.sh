#!/bin/sh

set -xe

CFLAGS="-Wall -Wextra -ggdb"
LIBS="-lraylib"
SOURCES=""

for source in ./src/*.c; do
    if [[ $source != "./src/main.c" ]]; then
        SOURCES+="$source "
    fi
done

mkdir -p ./target/

gcc $CFLAGS -o ./target/boids ./src/main.c $SOURCES $LIBS