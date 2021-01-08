#include "../../utils.h"
#include "../avx_operations.h"

void avx_NTT_radix4_all(unsigned *Y, unsigned *X, ull N, ull _root, ull _mod);
void avx_NTT_rec_all(ull N, ull n, unsigned *Y, unsigned *X, ull s);
void avx_NTT4_base_all(unsigned *Y, unsigned *X, ull s);
void avx_NTT16_base_and_twiddle_all(ull N, ull n, unsigned *Y, unsigned *X, ull s);
void avx_twiddle16_all(unsigned *Y, ull s, ull n, ull j);