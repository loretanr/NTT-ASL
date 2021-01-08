#include "ntt-radix2-scalar2.h"

void NTT_radix2_iterative_loop_unrolling(unsigned *outvec, unsigned *invec, ull n, ull root, ull mod)
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
    // len = 2, half = 1
    if (n < 2) {
        return;
    }
    ull *T = precomp_radix2_iterative[0];
    ull T_0 = T[0];
    for (int i = 0; i < n; i += 2) {
        ull u = outvec[i];
        ull v = (T_0 * outvec[i + 1]) % mod;
        MOD_ADD(outvec[i    ], u, v, mod);
        MOD_SUB(outvec[i + 1], u, v, mod);
    }

    if (n < 4) {
        return;
    }
    // len = 4, half = 2
    T = precomp_radix2_iterative[1];
    T_0 = T[0];
    ull T_1 = T[1];
    for (int i = 0; i < n; i += 4) {

        int i_1 = i + 1;
        int i_2 = i + 2;
        int i_3 = i + 3;

        ull u_0 = outvec[i];
        ull u_1 = outvec[i_1];

        ull v_0 = (T_0 * outvec[i_2]) % mod;
        ull v_1 = (T_1 * outvec[i_3]) % mod;

        MOD_ADD(outvec[i  ], u_0, v_0, mod);
        MOD_ADD(outvec[i_1], u_1, v_1, mod);

        MOD_SUB(outvec[i_2], u_0, v_0, mod);
        MOD_SUB(outvec[i_3], u_1, v_1, mod);
    }

    // ntt
    for (int len = 8, half = 4; len <= n; len *= 2, half *= 2)
    {
        int i = log2(len) - 1;
        ull *T = precomp_radix2_iterative[i];
        for (int i = 0; i < n; i += len)
        {
            for (int j = 0; j < half; j+=4)
            {
                int j_1 = j + 1;
                int j_2 = j + 2;
                int j_3 = j + 3;

                int i_add_j = i + j;
                int i_add_j_1 = i + j_1;
                int i_add_j_2 = i + j_2;
                int i_add_j_3 = i + j_3;

                int i_j_half = i_add_j + half;
                int i_j_1_half = i_add_j_1 + half;
                int i_j_2_half = i_add_j_2 + half;
                int i_j_3_half = i_add_j_3 + half;

                ull u_0 = outvec[i_add_j];
                ull u_1 = outvec[i_add_j_1];
                ull u_2 = outvec[i_add_j_2];
                ull u_3 = outvec[i_add_j_3];

                ull v_0 = (T[j  ] * outvec[i_j_half  ]) % mod;
                ull v_1 = (T[j_1] * outvec[i_j_1_half]) % mod;
                ull v_2 = (T[j_2] * outvec[i_j_2_half]) % mod;
                ull v_3 = (T[j_3] * outvec[i_j_3_half]) % mod;

                MOD_ADD(outvec[i_add_j], u_0, v_0, mod);
                MOD_ADD(outvec[i_add_j_1], u_1, v_1, mod);
                MOD_ADD(outvec[i_add_j_2], u_2, v_2, mod);
                MOD_ADD(outvec[i_add_j_3], u_3, v_3, mod);

                MOD_SUB(outvec[i_j_half  ], u_0, v_0, mod);
                MOD_SUB(outvec[i_j_1_half], u_1, v_1, mod);
                MOD_SUB(outvec[i_j_2_half], u_2, v_2, mod);
                MOD_SUB(outvec[i_j_3_half], u_3, v_3, mod);
            }
        }
    }
}