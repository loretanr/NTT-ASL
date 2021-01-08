#include "../../utils.h"
#include "../avx_operations.h"

void avx_NTT_radix4_sawtooth(unsigned *Y, unsigned *X, ull N, ull _root, ull _mod);
void avx_NTT_rec_sawtooth(ull N, ull n, unsigned *Y, unsigned *X, ull s);
void avx_NTT4_base_sawtooth(unsigned *Y, unsigned *X, ull s);
void avx_NTT16_base_and_twiddle_sawtooth(ull N, ull n, unsigned *Y, unsigned *X, ull s);
void avx_twiddle16_sawtooth(unsigned *Y, ull s, ull n, ull j);