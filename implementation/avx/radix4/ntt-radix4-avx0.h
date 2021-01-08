#include "../../utils.h"
#include "../avx_operations.h"

void avx0_NTT_radix4_base(unsigned *Y, unsigned *X, ull N, ull _root, ull _mod);
void avx0_NTT_rec(ull N, ull n, unsigned *Y, unsigned *X, ull s);
void avx0_NTT4_base(unsigned *Y, unsigned *X, ull s);
void avx0_twiddle16(unsigned *Y, ull s, ull n, ull j);
void avx0_NTT4_twiddle(unsigned *Y, ull s, ull N, ull j);
void NTT4_quad_twiddle(__m256d &D0, __m256d &D1, __m256d &D2, __m256d &D3, __m256d &Y0, __m256d &Y1, __m256d &Y2, __m256d &Y3);
