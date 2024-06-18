#!/bin/sh

set -xe

for obj in ./src/*.o; do
        rm $obj
done