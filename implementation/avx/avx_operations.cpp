#include "avx_operations.h"

/* ==================== AVX 4x32bits integer operations ==================== */
__m128i _mm128i_VEC_MOD_ADD(__m128i &a, __m128i &b, int mod)
{
    __m128i add = _mm_add_epi32(a, b);
    __m128i mask_1 = _mm_cmplt_epi32(add, mm128i_mod);
    __m128i sub = _mm_sub_epi32(add, mm128i_mod);
    __m128i c = _mm_blendv_epi8(sub, add, mask_1);
    return c;
}

__m128i _mm128i_VEC_MOD_SUB(__m128i &a, __m128i &b, int mod)
{
    __m128i add = _mm_sub_epi32(a, b);
    __m128i mask_3 = _mm_cmplt_epi32(add, _mm_set1_epi32(0));
    __m128i sub = _mm_add_epi32(add, mm128i_mod);
    __m128i c = _mm_blendv_epi8(add, sub, mask_3);
    return c;
}

__m128i _mm128i_VEC_MOD(__m256d &a, int mod)
{
    __m256d divided = _mm256_div_pd(a, mm256d_mod);
    __m256d truncated_div = _mm256_floor_pd(divided);
    __m256d mod_times_divided = _mm256_mul_pd(mm256d_mod, truncated_div);
    __m256d modulo_end_result = _mm256_sub_pd(a, mod_times_divided);
    return _mm256_cvtpd_epi32(modulo_end_result);
}

__m128i _mm128i_VEC_MOD_2(__m256d &a, int mod)
{
    __m256d divided = _mm256_mul_pd(a, mm256d_mod_inv);
    __m256d truncated_div = _mm256_floor_pd(divided);
    __m256d mod_times_divided = _mm256_mul_pd(mm256d_mod, truncated_div);
    __m256d modulo_end_result = _mm256_sub_pd(a, mod_times_divided);
    return _mm256_cvtpd_epi32(modulo_end_result);
}

__m128i _mm128i_VEC_MOD_3(__m256d &a, int mod)
{
    __m256d divided = _mm256_mul_pd(a, mm256d_mod_inv);
    __m256d truncated_div = _mm256_floor_pd(divided);
    //__m256d mod_times_divided = _mm256_mul_pd(mm256d_mod, truncated_div);
    //__m256d modulo_end_result = _mm256_sub_pd(a, mod_times_divided);
    __m256d fmresult = _mm256_fnmadd_pd(mm256d_mod, truncated_div, a);
    return _mm256_cvtpd_epi32(fmresult);
}

__m128i _mm128i_VEC_MOD_MULT_2(__m128i &a, __m128i &b, int mod)
{
    __m256d c = _mm256_mul_pd(_mm256_cvtepi32_pd(a), _mm256_cvtepi32_pd(b));
    return _mm128i_VEC_MOD_2(c, mod);
}

__m128i _mm128i_VEC_MOD_MULT_3(__m128i &a, __m128i &b, int mod)
{
    __m256d c = _mm256_mul_pd(_mm256_cvtepi32_pd(a), _mm256_cvtepi32_pd(b));
    return _mm128i_VEC_MOD_3(c, mod);
}

__m128i _mm128i_VEC_MOD_MULT(__m128i &a, __m128i &b, int mod)
{
    __m256d c = _mm256_mul_pd(_mm256_cvtepi32_pd(a), _mm256_cvtepi32_pd(b));
    return _mm128i_VEC_MOD(c, mod);
}

/* ==================== AVX 4x64bits double operations ==================== */
__m256d _mm256d_VEC_MOD_ADD(__m256d &a, __m256d &b, int mod)
{
    __m256d add = _mm256_add_pd(a, b);
    __m256d mask_1 = _mm256_cmp_pd(add, mm256d_mod, _CMP_LT_OQ);
    __m256d sub = _mm256_sub_pd(add, mm256d_mod);
    __m256d c = _mm256_blendv_pd(sub, add, mask_1);
    return c;
}

__m256d _mm256d_VEC_MOD_SUB(__m256d &a, __m256d &b, int mod)
{
    __m256d add = _mm256_sub_pd(a, b);
    __m256d mask_1 = _mm256_cmp_pd(a, b, _CMP_GE_OQ);
    __m256d sub = _mm256_add_pd(add, mm256d_mod);
    __m256d c = _mm256_blendv_pd(sub, add, mask_1);
    return c;
}

__m256d _mm256d_VEC_MOD(__m256d &a, int mod)
{
    __m256d divided = _mm256_div_pd(a, mm256d_mod);
    __m256d truncated_div = _mm256_floor_pd(divided);
    __m256d mod_times_divided = _mm256_mul_pd(mm256d_mod, truncated_div);
    __m256d modulo_end_result = _mm256_sub_pd(a, mod_times_divided);
    return modulo_end_result;
}

__m256d _mm256d_VEC_MOD_MULT(__m256d &a, __m256d &b, int mod)
{
    __m256d c;
    c = _mm256_mul_pd(a, b);
    return _mm256d_VEC_MOD(c, mod);
}