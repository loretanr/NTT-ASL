#ifndef PROJECT_TRANSFORM_HHH
#define PROJECT_TRANSFORM_HHH

void DFT4(int *invec, int *outvec, int n, int _root, int _mod);
void DFT_rec(int N, int n, int *Y, int *X, int s);
void init_DFT(int N);
static void DFT4_base(int *Y, int *X, int s);
static void DFT4_twiddle(int *Y, int s, int N, int j);
int modpow(int base, int exp, int modulus);

#endif //PROJECT_TRANSFORM_HHH
