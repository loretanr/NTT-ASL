#!/bin/bash

# note you need to have papi installed
# for me it was  $ sudo apt install papi-tools libpapi5 libpapi-dev
# also note, the plot script uses hardcoded specs of my laptop

mkdir -p roofline
rm -f roofline/*.csv roofline/*.pdf

COMPILER=g++
FLAGS=(# "-O0 "
        # "-O1"
        # "-O2"
        # "-O3"
        "-O3 -march=native"
        # "-O3 -flto"
        # "-Ofast"
        # "-O3 -ffast-math"
        # "-O3 -ffast-math -march=native -flto"
)
# /usr/lib/x86_64-linux-gnu/libpapi.a /usr/lib/x86_64-linux-gnu/libpapi.so
for i in "${FLAGS[@]}"; do
        echo Compiling using $COMPILER with flags $i
        g++  -std=c++11 -mavx2 $i \
                -Iimplementation/FTL/nttw/include \
                -Iimplementation/FTL/nttw/src \
                implementation/FTL/nttw/src/*.c \
                implementation/ntt-radix2.cpp \
                implementation/ntt-radix4.cpp \
                implementation/avx/radix4/ntt-radix4-avx*.cpp \
                implementation/avx/radix2/ntt-radix2-avx*.cpp  \
                implementation/scalar/radix4/ntt-radix4-scalar*.cpp \
                implementation/scalar/radix2/ntt-radix2-scalar*.cpp \
                implementation/avx/avx_operations.cpp \
                implementation/utils.cpp \
                performance/roofline.cpp \
                performance/tsc_x86.h \
                -lpapi \
                -o roof
        sudo ./roof $COMPILER $i
        rm ./roof

        python3 -W ignore roofline/roofline.py
done
