#include "ntt-radix2-scalar0.h"

void NTT_radix2_iterative_shifts(unsigned *outvec, unsigned *invec, ull n, ull root, ull mod)
{
    //init_r2_rec_opt(n, root, mod);
    for (int i = 0; i < n; i++)
    {
        outvec[i] = invec[i];
    }

    // bit reversal
    for (int i = 1, j = 0; i < n; i++)
    {
        unsigned bit = n >> 1;
        for (; j >= bit; bit = bit >> 1)
            j ^= bit;
        j ^= bit;
        if (i < j)
        {
            unsigned t = outvec[i];
            outvec[i] = outvec[j];
            outvec[j] = t;
        }
    }

    // ntt
    for (int len = 2, half = 1; len <= n; len <<= 1, half <<= 1)
    {
        int i = log2(len) - 1;
        ull *T = precomp_radix2_iterative[i];
        for (int i = 0; i < n; i += len)
        {

            for (int j = 0; j < half; j++)
            {
                ull u = outvec[i + j];
                ull v = (T[j] * outvec[i + j + half]) % mod;
                MOD_ADD(outvec[i + j], u, v, mod);
                MOD_SUB(outvec[i + j + half], u, v, mod);
            }
        }
    }
}