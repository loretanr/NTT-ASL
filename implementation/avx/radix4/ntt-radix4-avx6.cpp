// recursive radix-4 NTT implementation
#include "ntt-radix4-avx6.h"
#include <iostream>

using namespace std;

void avx6_NTT_radix4_base(unsigned *Y, unsigned *X, ull N, ull _root, ull _mod)
{
    if (isPower4(N))
    {

        avx6_NTT_rec(N, log4(N), Y, X, 1);
    }
    else
    {
        ull N_div_2 = N >> 1;
        avx6_NTT_rec(N_div_2, log4(N_div_2), even, X, 2);
        avx6_NTT_rec(N_div_2, log4(N_div_2), odd, X + 1, 2);

        // twiddle 2
        __m128i w;
        for (int k = 0; k < N_div_2; k+=4)
        {
            if (k == 0){
                w = _mm_set_epi32(DN_power2[2], DN_power2[1], DN_power2[0], 1);
            }
            else {
                w = _mm_set_epi32(DN_power2[k+2], DN_power2[k+1], DN_power2[k], DN_power2[k-1]);
            }
            __m128i odd_vec = _mm_load_si128((__m128i*)(odd + k));
            __m128i even_vec = _mm_load_si128((__m128i*)(even + k));
            __m128i t = _mm128i_VEC_MOD_MULT(w, odd_vec, _mod);
            __m128i t_add = _mm128i_VEC_MOD_ADD(even_vec, t, _mod);
            _mm_store_si128((__m128i*)(Y+k), t_add); // Store t_add in Y[k]
            __m128i t_sub = _mm128i_VEC_MOD_SUB(even_vec, t, _mod);
            _mm_store_si128((__m128i*)(Y + k + N_div_2), t_sub);
        }
    }
}

// s: stride to access X
void avx6_NTT_rec(ull N, ull n, unsigned *Y, unsigned *X, ull s)
{
    int j;
    int N_div_4 = N >> 2;
    if (N == 4)
    {
        avx6_NTT4_base(Y, X, s);
    }
    else if (N == 16)
    {
        __m256d X0, X1, X2, X3, Y0, Y1, Y2, Y3;
        __m256d t0, t1, t2, t3;
        __m256d D0, D1, D2, D3;

        X0 = _mm256_set_pd(*(X + 3 * s + s * 4 * 0), *(X + 2 * s + s * 4 * 0), *(X + 1 * s + s * 4 * 0), *(X + 0 * s + s * 4 * 0));
        X1 = _mm256_set_pd(*(X + 3 * s + s * 4 * 1), *(X + 2 * s + s * 4 * 1), *(X + 1 * s + s * 4 * 1), *(X + 0 * s + s * 4 * 1));
        X2 = _mm256_set_pd(*(X + 3 * s + s * 4 * 2), *(X + 2 * s + s * 4 * 2), *(X + 1 * s + s * 4 * 2), *(X + 0 * s + s * 4 * 2));
        X3 = _mm256_set_pd(*(X + 3 * s + s * 4 * 3), *(X + 2 * s + s * 4 * 3), *(X + 1 * s + s * 4 * 3), *(X + 0 * s + s * 4 * 3));

        //ntt base
        t0 = _mm256d_VEC_MOD_ADD(X0, X2, mod);
        t1 = _mm256d_VEC_MOD_SUB(X0, X2, mod);
        t2 = _mm256d_VEC_MOD_ADD(X1, X3, mod);
        t3 = _mm256d_VEC_MOD_SUB(X1, X3, mod);

        t3 = _mm256d_VEC_MOD_MULT(mm256d_baseroot, t3, mod);

        Y0 = _mm256d_VEC_MOD_ADD(t0, t2, mod);
        Y1 = _mm256d_VEC_MOD_ADD(t1, t3, mod);
        Y2 = _mm256d_VEC_MOD_SUB(t0, t2, mod);
        Y3 = _mm256d_VEC_MOD_SUB(t1, t3, mod);

        // double *y;
        // y = (double *)&Y0;
        // cout << y[0] << " " << y[1] << " " << y[2] << " " << y[3] << endl;
        // y = (double *)&Y1;
        // cout << y[0] << " " << y[1] << " " << y[2] << " " << y[3] << endl;
        // y = (double *)&Y2;
        // cout << y[0] << " " << y[1] << " " << y[2] << " " << y[3] << endl;
        // y = (double *)&Y3;
        // cout << y[0] << " " << y[1] << " " << y[2] << " " << y[3] << endl;

        __m256d unpack0 = _mm256_unpacklo_pd(Y0, Y1);
        __m256d unpack1 = _mm256_unpackhi_pd(Y0, Y1);
        __m256d unpack2 = _mm256_unpacklo_pd(Y2, Y3);
        __m256d unpack3 = _mm256_unpackhi_pd(Y2, Y3);

        Y0 = _mm256_permute2f128_pd(unpack0, unpack2, 0x20);
        Y2 = _mm256_permute2f128_pd(unpack0, unpack2, 0x31);
        Y1 = _mm256_permute2f128_pd(unpack1, unpack3, 0x20);
        Y3 = _mm256_permute2f128_pd(unpack1, unpack3, 0x31);

        ull *Dj0 = DN[n - 2] + 4 * 0;
        ull *Dj1 = DN[n - 2] + 4 * 1;
        ull *Dj2 = DN[n - 2] + 4 * 2;
        ull *Dj3 = DN[n - 2] + 4 * 3;

        // D0 = _mm256_set_pd(Dj0[3], Dj0[2], Dj0[1], Dj0[0]);
        D1 = _mm256_set_pd(Dj1[3], Dj1[2], Dj1[1], Dj1[0]);
        D2 = _mm256_set_pd(Dj2[3], Dj2[2], Dj2[1], Dj2[0]);
        D3 = _mm256_set_pd(Dj3[3], Dj3[2], Dj3[1], Dj3[0]);

        X0 = Y0; //YVEC_MOD_MULT(Y0, D0, mod);
        X1 = _mm256d_VEC_MOD_MULT(Y1, D1, mod);
        X2 = _mm256d_VEC_MOD_MULT(Y2, D2, mod);
        X3 = _mm256d_VEC_MOD_MULT(Y3, D3, mod);

        // operations from NTT4
        t0 = _mm256d_VEC_MOD_ADD(X0, X2, mod);
        t1 = _mm256d_VEC_MOD_SUB(X0, X2, mod);
        t2 = _mm256d_VEC_MOD_ADD(X1, X3, mod);
        t3 = _mm256d_VEC_MOD_SUB(X1, X3, mod);

        t3 = _mm256d_VEC_MOD_MULT(mm256d_baseroot, t3, mod);

        Y0 = _mm256d_VEC_MOD_ADD(t0, t2, mod);
        Y1 = _mm256d_VEC_MOD_ADD(t1, t3, mod);
        Y2 = _mm256d_VEC_MOD_SUB(t0, t2, mod);
        Y3 = _mm256d_VEC_MOD_SUB(t1, t3, mod);

        __m128i I0 = _mm256_cvtpd_epi32(Y0);
        __m128i I1 = _mm256_cvtpd_epi32(Y1);
        __m128i I2 = _mm256_cvtpd_epi32(Y2);
        __m128i I3 = _mm256_cvtpd_epi32(Y3);
        _mm_maskstore_epi32((int *)Y + (N >> 2) * 0, _mm_set1_epi16(-1), I0); // TODO
        _mm_maskstore_epi32((int *)Y + (N >> 2) * 1, _mm_set1_epi16(-1), I1); // TODO
        _mm_maskstore_epi32((int *)Y + (N >> 2) * 2, _mm_set1_epi16(-1), I2); // TODO
        _mm_maskstore_epi32((int *)Y + (N >> 2) * 3, _mm_set1_epi16(-1), I3); // TODO
    }

    else
    {
        for (j = 0; j < 4; j++)
        {
            avx6_NTT_rec(N_div_4, n - 1, Y + (N_div_4)*j, X + j * s, s * 4);
        }
        for (j = 0; j < N / 4; j+=4)
        {
            //avx6_NTT4_twiddle(Y + 1 * j, N / 4, n, j);
            avx6_twiddle16(Y + 1 * j, N / 4, n, j);
        }
    }
}

inline void avx6_NTT4_base(unsigned *Y, unsigned *X, ull s)
{
    ull X0 = X[0 * s];
    ull X1 = X[1 * s];
    ull X2 = X[2 * s];
    ull X3 = X[3 * s];
    ull t0, t1, t2, t3;
    // c = a + b < mod ? a + b : a + b - mod
    // c = a >= b ? a - b : a - b + mod
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

inline void avx6_twiddle16(unsigned *Y, ull s, ull n, ull j)
{
    __m128i X0, X1, X2, X3, Y0, Y1, Y2, Y3, R0, R1, R2, R3;
    __m128i t0, t1, t2, t3;
    __m128i D0, D1, D2, D3;

    Y0 = _mm_maskload_epi32((int *)(Y + s * 0), _mm_set1_epi16(-1));
    Y1 = _mm_maskload_epi32((int *)(Y + s * 1), _mm_set1_epi16(-1));
    Y2 = _mm_maskload_epi32((int *)(Y + s * 2), _mm_set1_epi16(-1));
    Y3 = _mm_maskload_epi32((int *)(Y + s * 3), _mm_set1_epi16(-1));

    ull *Dj0 = DN[n - 2] + 4 * (j + 0);
    ull *Dj1 = DN[n - 2] + 4 * (j + 1);
    ull *Dj2 = DN[n - 2] + 4 * (j + 2);
    ull *Dj3 = DN[n - 2] + 4 * (j + 3);

    D0 = _mm_set_epi32(Dj0[3], Dj0[2], Dj0[1], Dj0[0]);
    D1 = _mm_set_epi32(Dj1[3], Dj1[2], Dj1[1], Dj1[0]);
    D2 = _mm_set_epi32(Dj2[3], Dj2[2], Dj2[1], Dj2[0]);
    D3 = _mm_set_epi32(Dj3[3], Dj3[2], Dj3[1], Dj3[0]);

    __m128i x0 = _128i_shuffle(D0, D1, 0, 1, 0, 1); // 0 1 4 5
    __m128i x1 = _128i_shuffle(D0, D1, 2, 3, 2, 3); // 2 3 6 7
    __m128i x2 = _128i_shuffle(D2, D3, 0, 1, 0, 1); // 8 9 12 13
    __m128i x3 = _128i_shuffle(D2, D3, 2, 3, 2, 3); // 10 11 1X 15X

    D0 = _128i_shuffle(x0, x2, 0, 2, 0, 2);
    D1 = _128i_shuffle(x0, x2, 1, 3, 1, 3);
    D2 = _128i_shuffle(x1, x3, 0, 2, 0, 2);
    D3 = _128i_shuffle(x1, x3, 1, 3, 1, 3);
    // __m128i X0, X1, X2, X3;
    // __m128i t0, t1, t2, t3;

    X0 = _mm128i_VEC_MOD_MULT(Y0, D0, mod);
    X1 = _mm128i_VEC_MOD_MULT(Y1, D1, mod);
    X2 = _mm128i_VEC_MOD_MULT(Y2, D2, mod);
    X3 = _mm128i_VEC_MOD_MULT(Y3, D3, mod);

    // operations from NTT4
    t0 = _mm128i_VEC_MOD_ADD(X0, X2, mod);
    t1 = _mm128i_VEC_MOD_SUB(X0, X2, mod);
    t2 = _mm128i_VEC_MOD_ADD(X1, X3, mod);
    t3 = _mm128i_VEC_MOD_SUB(X1, X3, mod);

    t3 = _mm128i_VEC_MOD_MULT(mm128i_baseroot, t3, mod);

    R0 = _mm128i_VEC_MOD_ADD(t0, t2, mod);
    R1 = _mm128i_VEC_MOD_ADD(t1, t3, mod);
    R2 = _mm128i_VEC_MOD_SUB(t0, t2, mod);
    R3 = _mm128i_VEC_MOD_SUB(t1, t3, mod);

    _mm_maskstore_epi32((int *)Y + s * 0, _mm_set1_epi16(-1), R0);
    _mm_maskstore_epi32((int *)Y + s * 1, _mm_set1_epi16(-1), R1);
    _mm_maskstore_epi32((int *)Y + s * 2, _mm_set1_epi16(-1), R2);
    _mm_maskstore_epi32((int *)Y + s * 3, _mm_set1_epi16(-1), R3);
}

inline void avx6_NTT4_quad_twiddle(__m256d &D0, __m256d &D1, __m256d &D2, __m256d &D3, __m256d &Y0, __m256d &Y1, __m256d &Y2, __m256d &Y3)
{
    __m256d X0, X1, X2, X3;
    __m256d t0, t1, t2, t3;

    X0 = Y0; //YVEC_MOD_MULT(Y0, D0, mod);
    X1 = _mm256d_VEC_MOD_MULT(Y1, D1, mod);
    X2 = _mm256d_VEC_MOD_MULT(Y2, D2, mod);
    X3 = _mm256d_VEC_MOD_MULT(Y3, D3, mod);

    // operations from NTT4
    t0 = _mm256d_VEC_MOD_ADD(X0, X2, mod);
    t1 = _mm256d_VEC_MOD_SUB(X0, X2, mod);
    t2 = _mm256d_VEC_MOD_ADD(X1, X3, mod);
    t3 = _mm256d_VEC_MOD_SUB(X1, X3, mod);

    t3 = _mm256d_VEC_MOD_MULT(mm256d_baseroot, t3, mod);

    Y0 = _mm256d_VEC_MOD_ADD(t0, t2, mod);
    Y1 = _mm256d_VEC_MOD_ADD(t1, t3, mod);
    Y2 = _mm256d_VEC_MOD_SUB(t0, t2, mod);
    Y3 = _mm256d_VEC_MOD_SUB(t1, t3, mod);
}

inline void avx6_NTT4_twiddle(unsigned *Y, ull s, ull n, ull j)
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