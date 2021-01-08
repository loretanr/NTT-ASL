#include "ntt-radix2-avx3.h"

void avx_radix2_iterative_loop_unrolling(unsigned *outvec, unsigned *invec, ull n, ull root, ull mod) {
    //init_r2_rec_opt(n, root, mod);
    for (int i = 0; i < n; i++) {
        outvec[i] = invec[i];
    }

    // bit reversal
    for (int i = 1, j = 0; i < n; i++) {
        unsigned bit = n >> 1;
        for (; j >= bit; bit = bit >> 1)
            j ^= bit;
        j ^= bit;
        if (i < j) {
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
        ull u_0 = outvec[i];
        ull u_1 = outvec[i + 1];

        ull v_0 = (T_0 * outvec[i + 2]) % mod;
        ull v_1 = (T_1* outvec[i + 3]) % mod;

        MOD_ADD(outvec[i    ], u_0, v_0, mod);
        MOD_ADD(outvec[i + 1], u_1, v_1, mod);

        MOD_SUB(outvec[i + 2], u_0, v_0, mod);
        MOD_SUB(outvec[i + 3], u_1, v_1, mod);
    }

    if (n < 8) {
        return;
    }

    // len = 8, half = 4
    T = precomp_radix2_iterative[2];
    __m128i T_vec = _mm_set_epi32((int)T[3],(int)T[2],(int)T[1],(int)T[0]);

    for (int i = 0; i < n; i += 8) {
        __m128i u_vec = _mm_load_si128 ((__m128i *)(outvec + i));
        __m128i v_vec = _mm_load_si128 ((__m128i *)(outvec + i + 4));
        __m128i v_res = _mm128i_VEC_MOD_MULT(T_vec,v_vec, mod);
        __m128i outvec_res_u = _mm128i_VEC_MOD_ADD(u_vec,v_res, mod);
        __m128i outvec_res_v = _mm128i_VEC_MOD_SUB(u_vec,v_res, mod);
        _mm_store_si128((__m128i *)(outvec + i), outvec_res_u);
        _mm_store_si128((__m128i *)(outvec + i + 4), outvec_res_v);
    }

    int i = 3;
    for (int len = 16, half = 8; len <= n; len <<= 1, half <<= 1) {
        ull *T = precomp_radix2_iterative[i];
        for (int i = 0; i < n; i += len) {
            for (int j = 0; j < half; j+=8) {
                T_vec = _mm_set_epi32((int)T[j+3],(int)T[j+2],(int)T[j+1],(int)T[j+0]);
                __m128i T_vec_2 = _mm_set_epi32((int)T[j+7],(int)T[j+6],(int)T[j+5],(int)T[j+4]);

                __m128i u_vec   = _mm_load_si128 ((__m128i *)(outvec + i + j));
                __m128i u_vec_2 = _mm_load_si128 ((__m128i *)(outvec + i + j + 4));

                __m128i v_vec   = _mm_load_si128 ((__m128i *)(outvec + i + j + half));
                __m128i v_vec_2 = _mm_load_si128 ((__m128i *)(outvec + i + j + 4 + half));

                __m128i v_res   = _mm128i_VEC_MOD_MULT(T_vec  ,v_vec  , mod);
                __m128i v_res_2 = _mm128i_VEC_MOD_MULT(T_vec_2,v_vec_2, mod);

                __m128i outvec_res_u   = _mm128i_VEC_MOD_ADD(u_vec  ,v_res  , mod);
                __m128i outvec_res_u_2 = _mm128i_VEC_MOD_ADD(u_vec_2,v_res_2, mod);

                __m128i outvec_res_v   = _mm128i_VEC_MOD_SUB(u_vec  ,v_res  , mod);
                __m128i outvec_res_v_2 = _mm128i_VEC_MOD_SUB(u_vec_2,v_res_2, mod);

                _mm_store_si128((__m128i *)(outvec + i + j)    , outvec_res_u);
                _mm_store_si128((__m128i *)(outvec + i + j + 4), outvec_res_u_2);

                _mm_store_si128((__m128i *)(outvec + i + j + half)    , outvec_res_v);
                _mm_store_si128((__m128i *)(outvec + i + j + 4 + half), outvec_res_v_2);
            }
        }
        i++;
    }
}
