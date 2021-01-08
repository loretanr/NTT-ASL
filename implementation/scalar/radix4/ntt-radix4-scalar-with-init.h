#include "../../utils.h"

void scalar_NTT_radix4_base_with_init(unsigned *Y, unsigned *X, ull N, ull _root, ull _mod); //This gets called
void scalar_NTT_rec_with_init(ull N, ull n, unsigned *Y, unsigned *X, ull s);
void scalar_NTT4_base_with_init(unsigned *Y, unsigned *X, ull s);
void scalar_NTT4_twiddle_with_init(unsigned *Y, ull s, ull N, ull j);
void scalar_init_r4_NTT(ull N, ull _root, ull _mod);