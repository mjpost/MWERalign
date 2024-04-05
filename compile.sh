#!/bin/bash -xe
#
# Compiled under the "intel" architecture
# gcc version 4.0.2 (Debian 4.0.2-2)
#
#
g++ -I./include/ -g -O2 -c -o mwerAlign-test.o mwerAlign-test.cc
g++ -g -O2 -static -o mwerAlignTest mwerAlign-test.o  -L./include/ libs/libMwerAlign.a -lz
