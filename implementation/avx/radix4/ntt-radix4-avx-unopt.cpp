// recursive radix-4 NTT implementation
#include "ntt-radix4-avx-unopt.h"
using namespace std;

void avx_NTT_radix4_unopt(unsigned *Y, unsigned *X, ull N, ull _root, ull _mod)
{
    if (isPower4(N))
    {
        avx_NTT_rec_unopt(N, log4(N), Y, X, 1);
    }
    else
    {
        ull N_div_2 = N >> 1;
        unsigned log4_N2 = log4(N_div_2);
        avx_NTT_rec_unopt(N_div_2, log4_N2, even, X, 2);
        avx_NTT_rec_unopt(N_div_2, log4_N2, odd, X + 1, 2);

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
void avx_NTT_rec_unopt(ull N, ull n, unsigned *Y, unsigned *X, ull s)
{
    int j;
    int N_div_4 = N >> 2;
    if (N == 4)
    {
        avx_NTT4_base_unopt(Y, X, s);
    }
    else if (N == 16)
    {
        avx_NTT16_base_and_twiddle_unopt(N, n, Y, X, s);
    }
    else
    {
        for (j = 0; j < 4; j++)
        {
            avx_NTT_rec_unopt(N_div_4, n - 1, Y + (N_div_4)*j, X + j * s, s * 4);
        }
        for (j = 0; j < N / 4; j += 4)
        {

            avx_twiddle16_unopt(Y + j, N / 4, n, j);
        }
    }
}

inline void avx_NTT16_base_and_twiddle_unopt(ull N, ull n, unsigned *Y, unsigned *X, ull s)
{
    __m128i X0, X1, X2, X3, Y0, Y1, Y2, Y3;
    __m128i t0, t1, t2, t3;
    __m128i D0, D1, D2, D3;

    X0 = _mm_set_epi32(*(X + 3 * s + s * 4 * 0), *(X + 2 * s + s * 4 * 0), *(X + 1 * s + s * 4 * 0), *(X + 0 * s + s * 4 * 0));
    X1 = _mm_set_epi32(*(X + 3 * s + s * 4 * 1), *(X + 2 * s + s * 4 * 1), *(X + 1 * s + s * 4 * 1), *(X + 0 * s + s * 4 * 1));
    X2 = _mm_set_epi32(*(X + 3 * s + s * 4 * 2), *(X + 2 * s + s * 4 * 2), *(X + 1 * s + s * 4 * 2), *(X + 0 * s + s * 4 * 2));
    X3 = _mm_set_epi32(*(X + 3 * s + s * 4 * 3), *(X + 2 * s + s * 4 * 3), *(X + 1 * s + s * 4 * 3), *(X + 0 * s + s * 4 * 3));

    t0 = _mm128i_VEC_MOD_ADD(X0, X2, mod);
    t1 = _mm128i_VEC_MOD_SUB(X0, X2, mod);
    t2 = _mm128i_VEC_MOD_ADD(X1, X3, mod);
    t3 = _mm128i_VEC_MOD_SUB(X1, X3, mod);

    t3 = _mm128i_VEC_MOD_MULT(mm128i_baseroot, t3, mod);

    Y0 = _mm128i_VEC_MOD_ADD(t0, t2, mod);
    Y1 = _mm128i_VEC_MOD_ADD(t1, t3, mod);
    Y2 = _mm128i_VEC_MOD_SUB(t0, t2, mod);
    Y3 = _mm128i_VEC_MOD_SUB(t1, t3, mod);

    __m128i x0 = _128i_shuffle(Y0, Y1, 0, 1, 0, 1); // 0 1 4 5
    __m128i x1 = _128i_shuffle(Y0, Y1, 2, 3, 2, 3); // 2 3 6 7
    __m128i x2 = _128i_shuffle(Y2, Y3, 0, 1, 0, 1); // 8 9 12 13
    __m128i x3 = _128i_shuffle(Y2, Y3, 2, 3, 2, 3); // 10 11 14 15

    Y0 = _128i_shuffle(x0, x2, 0, 2, 0, 2);
    Y1 = _128i_shuffle(x0, x2, 1, 3, 1, 3);
    Y2 = _128i_shuffle(x1, x3, 0, 2, 0, 2);
    Y3 = _128i_shuffle(x1, x3, 1, 3, 1, 3);

    // ull *Dj0 = DN[n - 2] + 4 * 0;
    ull *Dj1 = DN[n - 2] + 4 * 1;
    ull *Dj2 = DN[n - 2] + 4 * 2;
    ull *Dj3 = DN[n - 2] + 4 * 3;

    // D0 = _mm_set_epi32(Dj0[3], Dj0[2], Dj0[1], Dj0[0]);
    D1 = _mm_set_epi32(Dj1[3], Dj1[2], Dj1[1], Dj1[0]);
    D2 = _mm_set_epi32(Dj2[3], Dj2[2], Dj2[1], Dj2[0]);
    D3 = _mm_set_epi32(Dj3[3], Dj3[2], Dj3[1], Dj3[0]);

    // __m128i X0, X1, X2, X3;
    // __m128i t0, t1, t2, t3;

    X0 = Y0; //YVEC_MOD_MULT(Y0, D0, mod);
    X1 = _mm128i_VEC_MOD_MULT(Y1, D1, mod);
    X2 = _mm128i_VEC_MOD_MULT(Y2, D2, mod);
    X3 = _mm128i_VEC_MOD_MULT(Y3, D3, mod);

    // operations from NTT4
    t0 = _mm128i_VEC_MOD_ADD(X0, X2, mod);
    t1 = _mm128i_VEC_MOD_SUB(X0, X2, mod);
    t2 = _mm128i_VEC_MOD_ADD(X1, X3, mod);
    t3 = _mm128i_VEC_MOD_SUB(X1, X3, mod);

    t3 = _mm128i_VEC_MOD_MULT(mm128i_baseroot, t3, mod);

    Y0 = _mm128i_VEC_MOD_ADD(t0, t2, mod);
    Y1 = _mm128i_VEC_MOD_ADD(t1, t3, mod);
    Y2 = _mm128i_VEC_MOD_SUB(t0, t2, mod);
    Y3 = _mm128i_VEC_MOD_SUB(t1, t3, mod);
    _mm_maskstore_epi32((int *)Y + (N >> 2) * 0, _mm_set1_epi16(-1), Y0);
    _mm_maskstore_epi32((int *)Y + (N >> 2) * 1, _mm_set1_epi16(-1), Y1);
    _mm_maskstore_epi32((int *)Y + (N >> 2) * 2, _mm_set1_epi16(-1), Y2);
    _mm_maskstore_epi32((int *)Y + (N >> 2) * 3, _mm_set1_epi16(-1), Y3);
}

inline void avx_twiddle16_unopt(unsigned *Y, ull s, ull n, ull j)
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

inline void avx_NTT4_base_unopt(unsigned *Y, unsigned *X, ull s)
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