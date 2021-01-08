// recursive radix-4 NTT implementation
#include "utils.h"
#include "ntt-radix4.h"
#include <immintrin.h>

using namespace std;

ull root4, mod, baseroot;
double mod_inv;
ull **DN;
ull *DN_power2;
unsigned *even, *odd;

// AVX global vectors
__m256d mm256d_mod;
__m256d mm256d_mod_inv;
__m256d mm256d_baseroot;
__m128i mm128i_mod;
__m128i mm128i_baseroot;

void NTT_radix4_base(unsigned *Y, unsigned *X, ull N, ull _root, ull _mod)
{
    if (isPower4(N))
    {

        NTT_rec(N, log4(N), Y, X, 1);
    }
    else
    {
        ull N_div_2 = N / 2;
        NTT_rec(N_div_2, log4(N_div_2), even, X, 2);
        NTT_rec(N_div_2, log4(N_div_2), odd, X + 1, 2);

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
void NTT_rec(ull N, ull n, unsigned *Y, unsigned *X, ull s)
{
    int j;
    int N_div_4 = N / 4;
    if (N == 4)
    {
        NTT4_base(Y, X, s);
    }
    else
    {
        for (j = 0; j < 4; j++)
        {
            NTT_rec(N_div_4, n - 1, Y + (N_div_4)*j, X + j * s, s * 4);
        }
        for (j = 0; j < N_div_4; j += 4)
        {
            NTT4_twiddle(Y + (j + 0), N_div_4, n, j + 0);
            NTT4_twiddle(Y + (j + 1), N_div_4, n, j + 1);
            NTT4_twiddle(Y + (j + 2), N_div_4, n, j + 2);
            NTT4_twiddle(Y + (j + 3), N_div_4, n, j + 3);
        }
    }
}

// s: stride, should always be 1
void NTT4_base(unsigned *Y, unsigned *X, ull s)
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

void NTT4_twiddle(unsigned *Y, ull s, ull n, ull j)
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

    // operations from NTT4
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

// ================== Cost functions ==================

int formula_cost_radix4_rec(int n)
{
    int n_div_2 = n >> 1;
    int n_div_4 = n >> 2;
    int costBase = 26;    // 24 add, 1 mul, 1 mod
    int costTwiddle = 36; // 25 add, 6 mul, 5 mod
    if (isPower4(n))
    {
        return (log4(n) - 1) * ((n_div_4) * (costTwiddle)) + (n_div_4) * (costBase + 1);
    }
    else
    {
        return 8 * n_div_2 + 2 * formula_cost_radix4_rec(n_div_2);
    }
}

inline int _cost_rec(int n)
{
    int cost = 1;
    int N_div_4 = n / 4;
    if (n == 4)
    {
        cost += 26;
        return cost;
    }
    else
    {
        int r1 = _cost_rec(N_div_4);
        int r2 = _cost_rec(N_div_4);
        int r3 = _cost_rec(N_div_4);
        int r4 = _cost_rec(N_div_4);
        cost = r1 + r2 + r3 + r4;
        for (int j = 0; j < N_div_4; j += 4)
        {
            cost += 4 * 36;
        }
        return cost;
    }
}

int real_cost_radix4_rec(int n)
{
    int N_div_2 = n / 2;
    if (isPower4(n))
    {
        return _cost_rec(n);
    }
    else
    {
        int r1 = _cost_rec(N_div_2);
        int r2 = _cost_rec(N_div_2);

        return 8 * N_div_2 + r1 + r2;
    }
}

// ================== init and other bench-irrelevant stuff =====================

// create twiddle table, and init roots
void init_r4_NTT(ull N, ull _root, ull _mod)
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
    mm256d_mod = _mm256_set1_pd(mod);
    mm256d_mod_inv = _mm256_set1_pd(1.0 / mod);
    mm128i_mod = _mm_set1_epi32(mod);
    mm256d_baseroot = _mm256_set1_pd(baseroot);
    mm128i_baseroot = _mm_set1_epi32(baseroot);

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

void destroy_NTT(ull N)
{
    if (!isPower4(N))
    {
        N >>= 1;
    }
    int j, size_Dj = 16, n_max = log4(N);
    for (j = 1; j < n_max; j++, size_Dj *= 4)
    {
        free(DN[j - 1]);
    }
    free(DN);
    free(even);
    free(odd);
}