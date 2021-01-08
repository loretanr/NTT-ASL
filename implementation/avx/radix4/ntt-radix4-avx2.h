#include "../../utils.h"
#include "../avx_operations.h"

void avx2_NTT_radix4_base(unsigned *Y, unsigned *X, ull N, ull _root, ull _mod);
void avx2_NTT_rec(ull N, ull n, unsigned *Y, unsigned *X, ull s);
void avx2_NTT4_base(unsigned *Y, unsigned *X, ull s);
void avx2_twiddle16(unsigned *Y, ull s, ull n, ull j);