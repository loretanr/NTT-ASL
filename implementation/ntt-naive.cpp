//
// Created by pfpanhaleux on 06.05.20.
//
#include "utils.h"
#include "ntt-naive.h"

// Q = 4 n^2 bytes + 8n bytes
void naive_ntt_baseline(unsigned *outvec, unsigned *invec, ull n, ull root, ull mod)
{
    for (int i = 0; i < n; i++)
    {
        ull sum = 0;
        for (int j = 0; j < n; j++)
        {
            // Q = (reads)
            // Q = n * usigned = 4n bytes
            // sum and temp are temporal local
            ull temp = invec[j] * modpow(root, i * j, mod) + sum;
            // Q = temp and sum reused, temporal locality
            sum = (temp % mod);
        }
        // Q = n * (long longs) = 8n bytes
        outvec[i] = sum; // TODO: implicit data conversion from long to int.
    }
}


