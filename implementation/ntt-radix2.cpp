#include "utils.h"
#include "ntt-radix2.h"
#include <immintrin.h>
#include "avx/avx_operations.h"

ull **precomp_radix2_iterative;

void NTT_radix2_iterative(unsigned *outvec, unsigned *invec, ull n, ull root, ull mod)
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
                ull v = (T[j] * outvec[i + j + half]) % mod;
                MOD_ADD(outvec[i + j], u, v, mod);
                MOD_SUB(outvec[i + j + half], u, v, mod);
            }
        }
    }
}

// ================== Cost functions ==================

int formula_cost_radix2_iter(int n)
{
    return log2(n)*(n/2)*8;
}

int real_cost_radix2_iter(int n)
{
    // (log2(n)-1)*(n/2)*(log2(n)*8)
    int opsCount = 0;
    for (int len = 2, half = 1; len <= n; len <<= 1, half <<= 1) {
        for (int i = 0; i < n; i += len) {
            for (int j = 0; j < half; j++) {
                opsCount+=8;
            }
        }
    }
    return opsCount;
}

// ================== init and other bench-irrelevant stuff =====================

// move pre-computable stuff for radix2 iterative here
void init_r2_iter_opt(ull n, ull root, ull mod)
{
    precomp_radix2_iterative = (ull **)malloc(sizeof(ull *) * (log2(n)));

    int i = 0;
    for (int len = 2, half = 1; len <= n; len <<= 1, half <<= 1)
    {
        // init_radix2(n);
        ull *Dj = precomp_radix2_iterative[i] = (ull *)malloc(sizeof(ull) * (n / 2));
        ull dw = modpow(root, n / len, mod);
        for (int i = 0; i < n; i += len)
        {
            ull w = 1;
            for (int j = 0; j < half; j++)
            {

                *(Dj++) = w;
                w = (w * dw) % mod;
            }
        }
        ++i;
    }
    mod_inv = 1.0 / mod;
    mm256d_mod = _mm256_set1_pd(mod);
    mm256d_mod_inv = _mm256_set1_pd(mod_inv);
    mm128i_mod = _mm_set1_epi32(mod);
    mm256d_baseroot = _mm256_set1_pd(baseroot);
    mm128i_baseroot = _mm_set1_epi32(baseroot);
}

// ================== Radix2 recursive function =====================

inline void _CT_radix2_recursive(unsigned *vec, ull n, ull root, ull mod, ull size)
{
    // base case
    if (n <= 1)
        return;

    n = n >> 1;

    // divide
    unsigned even[n];
    unsigned odd[n];

    for (int i = 0; i < n; i++)
    {
        even[i] = vec[i << 1];
        odd[i] = vec[(i << 1) + 1];
    }

    // conquer
    _CT_radix2_recursive(even, n, root, mod, size);
    _CT_radix2_recursive(odd, n, root, mod, size);

    ull w = 1; // Can be moved to init
    ull wd = modpow(root, size / (n << 1), mod); // Can be moved to init

    //twiddle2
    for (int k = 0; k < n; ++k)
    {
        ull t = (w * odd[k]) % mod;
        MOD_ADD(vec[k], even[k], t, mod);
        MOD_SUB(vec[k + n], even[k], t, mod);
        w = ((w) * (wd)) % mod; // Can be moved to init
    }
}

void CT_radix2_recursive(unsigned *outvec, unsigned *invec, ull n, ull root, ull mod)
{
    for (int i = 0; i < n; i++)
    {
        outvec[i] = invec[i];
    }

    _CT_radix2_recursive(outvec, n, root, mod, n);
}
