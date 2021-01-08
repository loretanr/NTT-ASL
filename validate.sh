#! /bin/bash
g++ -std=c++11 -mavx2 -mfma -O3 \
    -Iimplementation/FTL/nttw/include \
    -Iimplementation/FTL/nttw/src \
    implementation/FTL/nttw/src/*.c \
    validation/validate.cpp \
    implementation/utils.cpp \
    implementation/ntt-naive.cpp \
    implementation/ntt-radix*.cpp \
    implementation/scalar/radix2/ntt-radix2-scalar*.cpp \
    implementation/scalar/radix4/ntt-radix4-scalar*.cpp \
    implementation/avx/avx_operations.cpp \
    implementation/avx/radix2/ntt-radix2-avx*.cpp  \
    implementation/avx/radix4/ntt-radix4-avx*.cpp \
    -o validate

./validate
rm ./implementation/*.h.gch 2>/dev/null
rm validate
