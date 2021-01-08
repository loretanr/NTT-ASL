#include "../utils.h"
#include "avx_operations.h"

void avx69_NTT_radix4_base(unsigned *Y, unsigned *X, ull N, ull _root, ull _mod);
void avx69_NTT_rec(ull N, ull n, unsigned *Y, unsigned *X, ull s);
void avx69_NTT4_base(unsigned *Y, unsigned *X, ull s);
void avx69_NTT16_base_and_twiddle(ull N, ull n, unsigned *Y, unsigned *X, ull s);
void avx69_twiddle16(unsigned *Y, ull s, ull n, ull j);