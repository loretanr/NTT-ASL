#ifndef PROJECT_TRANSFORM_H_2
#define PROJECT_TRANSFORM_H_2

void NTT_radix4_base(unsigned *Y, unsigned *X, ull N, ull _root, ull _mod); //This gets called
void NTT_rec(ull N, ull n, unsigned *Y, unsigned *X, ull s);
void NTT4_base(unsigned *Y, unsigned *X, ull s);
void NTT4_twiddle(unsigned *Y, ull s, ull N, ull j);
int formula_cost_radix4_rec(int n);
int real_cost_radix4_rec(int n);
void init_r4_NTT(ull N, ull _root, ull _mod);
void destroy_NTT(ull N);

#endif //PROJECT_TRANSFORM_H_2
