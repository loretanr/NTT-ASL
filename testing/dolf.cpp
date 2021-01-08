#include "tsc_x86.h"
#include <stdlib.h>
#include <iostream>

#include <stdio.h>
#include <random>

#include <immintrin.h>
int mod = 97;

#define MOD_ADD(c, a, b, mod) c = a + b < mod ? a + b : a + b - mod
#define MOD_SUB(c, a, b, mod) c = a >= b ? a - b : a - b + mod // cannot do "<= 0 ?" with unsigned

__m256d VEC_MOD_ADD(__m256d a, __m256d b, int mod)
{
    __m256d c;
    __m256d VEC_MOD = _mm256_set1_pd(mod);
    __m256d add = _mm256_add_pd(a, b);
    __m256d mask_1 = _mm256_cmp_pd(add, VEC_MOD, _CMP_LT_OQ);
    __m256d sub = _mm256_sub_pd(add, VEC_MOD);
    c = _mm256_blendv_pd(sub, add, mask_1);
    return c;
}

__m256d VEC_MOD_SUB(__m256d a, __m256d b, int mod)
{
    __m256d c;
    __m256d VEC_MOD = _mm256_set1_pd(mod);
    __m256d add = _mm256_sub_pd(a, b);
    __m256d mask_1 = _mm256_cmp_pd(a, b, _CMP_GE_OQ);
    __m256d sub = _mm256_add_pd(add, VEC_MOD);
    c = _mm256_blendv_pd(sub, add, mask_1);
    return c;
}

/*
__m256d VEC_MOD_MOD(__m256d a, int mod)
{
    // d % p = d - int(float(d)/float(p))*p.
    __m256d c;
    __m256d k = _mm256_set1_pd(1.0f / mod);
    __m256d VEC_MOD = _mm256_set1_pd(mod);
    __m256d div = _mm256_div_pd(a, VEC_MOD);
    div = _mm256_mul_pd(k,a);
    c = _mm256_mul_pd(div, VEC_MOD);
    c = _mm256_sub_pd(a, c);
    return c;
    // return a - mod * (a / mod);
}


void Func(const int * a, const int * b, size_t size, int p, int * c)
{
    __m256 _k = _mm256_set1_ps(1.0f / p);
    __m256i _p = _mm256_set1_epi32(p);
    for (size_t i = 0; i < size; i += 8)
    {
        __m256i _a = _mm256_loadu_si128((__m256i*)(a + i));
        __m256i _b = _mm256_loadu_si128((__m256i*)(b + i));
        __m256i _d = _mm256_mullo_epi32(_a, _b);
        __m256i _e = _mm256_cvtps_epi32(_mm256_mul_ps(_mm256_cvtepi32_ps(_d), _k)); // e = int(float(d)/float(p));
        __m256i _c = _mm256_sub_epi32(_d, _mm256_mullo_epi32(_e, _p));
        _mm256_storeu_si128((__m256i*)(c + i), _c);
    }            
}*/

__m256i VEC_MOD_MULT(__m256i a, __m256i b, int mod)
{
    __m256i _a = a;
    __m256i _b = b;
    __m256 _k = _mm256_set1_ps(1.0f / mod);
    __m256i _p = _mm256_set1_epi32(mod);
    __m256i _d = _mm256_mullo_epi32(_a, _b);
    __m256i _e = _mm256_cvtps_epi32(_mm256_mul_ps(_mm256_cvtepi32_ps(_d), _k)); // e = int(float(d)/float(p));
    __m256i _c = _mm256_sub_epi32(_d, _mm256_mullo_epi32(_e, _p));
    return _c;
}

void rands(int *m, size_t n)
{
    std::random_device rd;
    std::mt19937 gen{rd()};
    std::uniform_real_distribution<double> dist(1, 200);
    for (size_t i = 0; i < n; ++i)
        m[i] = (int)dist(gen) % mod;
}

int main()
{
    int *x = static_cast<int *>(malloc(4 * sizeof(int)));
    rands(x, 4);
    int *y = static_cast<int *>(malloc(4 * sizeof(int)));
    double *res = static_cast<double *>(malloc(4 * sizeof(double)));

    rands(y, 4);
    std::cout << std::endl;
    std::cout << "=========ADD=========";
    std::cout << std::endl;
    for (int i = 0; i < 4; i++)
    {
        int c;
        MOD_ADD(c, x[i], y[i], mod);
        std::cout << c << " ";
    }
    std::cout << std::endl;
    __m256d a = _mm256_set_pd(x[3], x[2], x[1], x[0]);
    __m256d b = _mm256_set_pd(y[3], y[2], y[1], y[0]);
    __m256d c = VEC_MOD_ADD(a, b, 97);
    _mm256_store_pd(res, c);
    for (size_t i = 0; i < 4; i++)
    {
        std::cout << res[i] << " ";
    }

    std::cout << std::endl;
    std::cout << "=========SUB=========";
    std::cout << std::endl;

    for (int i = 0; i < 4; i++)
    {
        int c;
        MOD_SUB(c, x[i], y[i], mod);
        std::cout << c << " ";
    }
    std::cout << std::endl;

    c = VEC_MOD_SUB(a, b, 97);
    _mm256_store_pd(res, c);
    for (size_t i = 0; i < 4; i++)
    {
        std::cout << res[i] << " ";
    }
    float *res1 = static_cast<float *>(malloc(4 * sizeof(float)));

    std::cout << std::endl;
    std::cout << "=========MULT========";

    __m256i a1 = _mm256_set_epi64x(x[3], x[2], x[1], x[0]);
    __m256i b1 = _mm256_set_epi64x(y[3], y[2], y[1], y[0]);
    __m256i c1 = VEC_MOD_MULT(a1, b1, 97);
    std::cout << std::endl;
    for (int i = 0; i < 4; i++)
    {
        std::cout << x[i] * y[i] % mod << " ";
    }
    std::cout << std::endl;

    _mm256_store_ps(res1, _mm256_cvtepi32_ps(c1));
    // _mm256_store_epi64(res,c1)
    for (size_t i = 0; i < 4; i++)
    {
        std::cout << res1[i] << " ";
    }
    std::cout << std::endl;
}