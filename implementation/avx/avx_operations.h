#ifndef EFFICIENT_NTT_AVX_OPERATIONS_H
#define EFFICIENT_NTT_AVX_OPERATIONS_H

#include <immintrin.h>

/* Globals */
extern __m128i mm128i_baseroot;
extern __m256d mm256d_baseroot;
extern __m128i mm128i_mod;
extern __m256d mm256d_mod;
extern __m256d mm256d_mod_inv;


#define combine_4_2bits(n0, n1, n2, n3) (n0 + (n1 << 2) + (n2 << 4) + (n3 << 6))
#define _128_shuffle(x, y, n0, n1, n2, n3) _mm_shuffle_ps(x, y, combine_4_2bits(n0, n1, n2, n3))
#define _128i_shuffle(x, y, n0, n1, n2, n3) _mm_castps_si128(_128_shuffle(_mm_castsi128_ps(x), _mm_castsi128_ps(y), n0, n1, n2, n3))

__m128i _mm128i_VEC_MOD_ADD(__m128i &a, __m128i &b, int mod);
__m128i _mm128i_VEC_MOD_SUB(__m128i &a, __m128i &b, int mod);
__m128i _mm128i_VEC_MOD(__m256d &a, int mod);
__m128i _mm128i_VEC_MOD_2(__m256d &a, int mod);
__m128i _mm128i_VEC_MOD_3(__m256d &a, int mod);
__m128i _mm128i_VEC_MOD_MULT(__m128i &a, __m128i &b, int mod);
__m128i _mm128i_VEC_MOD_MULT_2(__m128i &a, __m128i &b, int mod);
__m128i _mm128i_VEC_MOD_MULT_3(__m128i &a, __m128i &b, int mod);



__m256d _mm256d_VEC_MOD_ADD(__m256d &a, __m256d &b, int mod);
__m256d _mm256d_VEC_MOD_SUB(__m256d &a, __m256d &b, int mod);
__m256d _mm256d_VEC_MOD(__m256d &a, int mod);
__m256d _mm256d_VEC_MOD_MULT(__m256d &a, __m256d &b, int mod);


#endif //EFFICIENT_NTT_AVX_OPERATIONS_H
