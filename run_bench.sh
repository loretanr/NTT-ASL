#!/bin/bash
rm -rf benchmark_results
mkdir -p benchmark_results

# echo "function,flag,root,mod,n,cycles,timing,intops" > benchmark_results/benchmarks.csv

# pip3 install -r requirements.txt

COMPILER=g++
g++ --version | grep g++ > output
COMPILER_VERSION=$(awk '{print $NF}' output)
rm output

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
for i in "${FLAGS[@]}"; do
        echo Compiling using $COMPILER with flags $i
        g++ -std=c++11 -mavx2 -mfma $i \
                -Iimplementation/FTL/nttw/include \
                -Iimplementation/FTL/nttw/src \
                implementation/FTL/nttw/src/*.c \
                implementation/ntt-naive.cpp \
                implementation/ntt-radix2.cpp \
                implementation/ntt-radix4.cpp \
                implementation/avx/radix4/ntt-radix4-avx*.cpp \
                implementation/avx/radix2/ntt-radix2-avx*.cpp  \
                implementation/scalar/radix4/ntt-radix4-scalar*.cpp \
                implementation/scalar/radix2/ntt-radix2-scalar*.cpp \
                implementation/avx/avx_operations.cpp \
                implementation/utils.cpp \
                performance/benchmarks.cpp \
                performance/tsc_x86.h \
                -o bench
        ./bench $COMPILER $i
        rm ./bench
        # python3 -W ignore performance/performance_plot.py $COMPILER $i
        python3 -W ignore performance/performance_plot.py $COMPILER $COMPILER_VERSION $i
done
