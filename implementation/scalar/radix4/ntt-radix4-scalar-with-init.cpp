// recursive radix-4 NTT implementation
#include "ntt-radix4-scalar-with-init.h"

using namespace std;

void scalar_NTT_radix4_base_with_init(unsigned *Y, unsigned *X, ull N, ull _root, ull _mod)
{
    scalar_init_r4_NTT(N, _root, _mod);
    if (isPower4(N))
    {

        scalar_NTT_rec_with_init(N, log4(N), Y, X, 1);
    }
    else
    {
        ull N_div_2 = N >> 1;
        scalar_NTT_rec_with_init(N_div_2, log4(N_div_2), even, X, 2);
        scalar_NTT_rec_with_init(N_div_2, log4(N_div_2), odd, X + 1, 2);

        // twiddle 2
        ull w = 1;

        for (int k = 0; k < N_div_2; ++k)
        {
            ull t = MOD(w * odd[k]);
            MOD_ADD(Y[k], even[k], t, _mod);
            MOD_SUB(Y[k + N_div_2], even[k], t, _mod);
            w = DN_power2[k];
        }
    }
}

// s: stride to access X
void scalar_NTT_rec_with_init(ull N, ull n, unsigned *Y, unsigned *X, ull s)
{
    int j;
    int N_div_4 = N >> 2;
    if (N == 4)
    {
        scalar_NTT4_base_with_init(Y, X, s);
    }
    else
    {
        for (j = 0; j < 4; j++)
        {
            scalar_NTT_rec_with_init(N_div_4, n - 1, Y + (N_div_4) * j, X + j * s, s * 4);
        }
        for (j = 0; j < N_div_4; j += 4)
        {
            scalar_NTT4_twiddle_with_init(Y + (j + 0), N_div_4, n, j + 0);
            scalar_NTT4_twiddle_with_init(Y + (j + 1), N_div_4, n, j + 1);
            scalar_NTT4_twiddle_with_init(Y + (j + 2), N_div_4, n, j + 2);
            scalar_NTT4_twiddle_with_init(Y + (j + 3), N_div_4, n, j + 3);
        }
    }
}

// s: stride, should always be 1
inline void scalar_NTT4_base_with_init(unsigned *Y, unsigned *X, ull s)
{
    ull X0 = X[0 * s];
    ull X1 = X[1 * s];
    ull X2 = X[2 * s];
    ull X3 = X[3 * s];
    ull t0, t1, t2, t3;

    MOD_ADD(t0, X0, X2, mod);
    MOD_SUB(t1, X0, X2, mod);
    MOD_ADD(t2, X1, X3, mod);
    MOD_SUB(t3, X1, X3, mod);

    t3 = MOD(baseroot * t3);

    MOD_ADD(Y[0], t0, t2, mod);
    MOD_ADD(Y[1], t1, t3, mod);
    MOD_SUB(Y[2], t0, t2, mod);
    MOD_SUB(Y[3], t1, t3, mod);
}

inline void scalar_NTT4_twiddle_with_init(unsigned *Y, ull s, ull n, ull j)
{
    ull t0, t1, t2, t3,
        X0, X1, X2, X3;
    ull *Dj;
    // complex multiplications from D_N
    Dj = DN[n - 2] + 4 * j;

    MOD_MULT(X0, Y, Dj, 0, s);
    MOD_MULT(X1, Y, Dj, 1, s);
    MOD_MULT(X2, Y, Dj, 2, s);
    MOD_MULT(X3, Y, Dj, 3, s);

    // operations from scalar1_NTT4
    MOD_ADD(t0, X0, X2, mod);
    MOD_SUB(t1, X0, X2, mod);
    MOD_ADD(t2, X1, X3, mod);
    MOD_SUB(t3, X1, X3, mod);

    t3 = MOD(baseroot * t3);

    MOD_ADD(Y[0 * s], t0, t2, mod);
    MOD_ADD(Y[1 * s], t1, t3, mod);
    MOD_SUB(Y[2 * s], t0, t2, mod);
    MOD_SUB(Y[3 * s], t1, t3, mod);
}

// create twiddle table, and init roots
void scalar_init_r4_NTT(ull N, ull _root, ull _mod)
{
    // NEW: initialize all the root-stuff + helper arrays here
    if (isPower4(N))
    {
        root4 = _root;
        mod = _mod;
        mod_inv = 1.0 / mod;
        int n = log4(N);
        baseroot = modpow(root4, pow(4, n - 1), mod);
    }
    else
    {
        int N2 = N >> 1;
        int n2 = log4(N2);
        mod = _mod;
        mod_inv = 1.0 / mod;
        root4 = modpow(_root, 2, _mod);
        baseroot = modpow(root4, pow(4, (n2 - 1)), _mod);
        even = (unsigned *)malloc(sizeof(unsigned) * N2);
        odd = (unsigned *)malloc(sizeof(unsigned) * N2);

        N = N2;
    }

    // fill twiddle table
    int i, j, k, size_Dj = 16, n_max = log4(N);
    DN = (ull **)malloc(sizeof(ull *) * (n_max));
    DN_power2 = (ull *)malloc(sizeof(ull) * (N));

    for (j = 1; j < n_max; j++, size_Dj *= 4)
    {
        ull *Dj = DN[j - 1] = (ull *)malloc(sizeof(ull) * size_Dj);
        for (k = 0; k < size_Dj / 4; k++)
        {
            for (i = 0; i < 4; i++)
            {
                *(Dj++) = modpow(root4, pow(4, n_max - j - 1) * (k * i), mod);
            }
        }
    }
    ull wd = _root;
    for (int i = 0; i < N; i++)
    {
        DN_power2[i] = modpow(wd, i + 1, mod);
    }
}