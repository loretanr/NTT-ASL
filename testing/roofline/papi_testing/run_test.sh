#!/bin/bash

# first:
# sudo apt intall papi-tools libpapi-dev

# tried this, doesn't work yet
# https://stackoverflow.com/questions/39709328/why-does-perf-and-papi-give-different-values-for-l3-cache-references-and-misses

g++ code.cpp -o papi1
g++ -lpapi cmp_papi_perf.cpp -o papi2
echo "PAPI:"
sudo ./papi2
echo -e "\nperf:"
# first 3 output values are papi, rest is perf
sudo perf stat -e cache-references:u,cache-misses:u,LLC-loads,LLC-load-misses,LLC-stores,LLC-store-misses ./papi1

rm papi1 papi2