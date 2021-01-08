typedef unsigned long long ull;


void NTT_radix4_base(unsigned *Y, unsigned *X, ull N, ull _root, ull _mod); //This gets called
void NTT_rec(ull N, ull n, unsigned *Y, unsigned *X, ull s);
void NTT4_base(unsigned *Y, unsigned *X, ull s);
void NTT4_twiddle(unsigned *Y, ull s, ull N, ull j);
