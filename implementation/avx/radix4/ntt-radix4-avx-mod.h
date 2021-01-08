#include "../../utils.h"
#include "../avx_operations.h"

void avx_NTT_radix4_mod(unsigned *Y, unsigned *X, ull N, ull _root, ull _mod);
void avx_NTT_rec_mod(ull N, ull n, unsigned *Y, unsigned *X, ull s);
void avx_NTT4_base_mod(unsigned *Y, unsigned *X, ull s);
void avx_NTT16_base_and_twiddle_mod(ull N, ull n, unsigned *Y, unsigned *X, ull s);
void avx_twiddle16_mod(unsigned *Y, ull s, ull n, ull j);