#include "../../utils.h"


void scalar0_NTT_radix4_base(unsigned *Y, unsigned *X, ull N, ull _root, ull _mod); //This gets called
void scalar0_NTT_rec(ull N, ull n, unsigned *Y, unsigned *X, ull s);
void scalar0_NTT4_base(unsigned *Y, unsigned *X, ull s);
void scalar0_NTT4_twiddle(unsigned *Y, ull s, ull N, ull j);