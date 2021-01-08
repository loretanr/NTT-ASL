#include "ntt-radix2-scalar1.h"

void NTT_radix2_iterative_mod2(unsigned *outvec, unsigned *invec, ull n, ull root, ull mod)
{
    //init_r2_rec_opt(n, root, mod);
    for (int i = 0; i < n; i++)
    {
        outvec[i] = invec[i];
    }

    // bit reversal
    for (int i = 1, j = 0; i < n; i++)
    {
        unsigned bit = n / 2;
        for (; j >= bit; bit = bit / 2)
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
    for (int len = 2, half = 1; len <= n; len *= 2, half *= 2)
    {
        int i = log2(len) - 1;
        ull *T = precomp_radix2_iterative[i];
        for (int i = 0; i < n; i += len)
        {
            for (int j = 0; j < half; j++)
            {
                ull u = outvec[i + j];
                ull v = MOD_2((T[j] * outvec[i + j + half]));
                MOD_ADD(outvec[i + j], u, v, mod);
                MOD_SUB(outvec[i + j + half], u, v, mod);
            }
        }
    }
}