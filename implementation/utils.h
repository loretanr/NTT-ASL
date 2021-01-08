#include <string>
#include <cmath>

/* Types */
typedef unsigned short int usint;
typedef unsigned long long ull;

/* Globals */
extern double mod_inv;
extern ull root4, mod, baseroot;
extern ull **DN;
extern ull *DN_power2;
extern unsigned *even, *odd;
extern ull **precomp_radix2_iterative;

/* Function types */
typedef void (*transform_func)(unsigned *, unsigned *, ull, ull, ull);
typedef void (*init_func)(ull size, ull _root, ull _mod);
typedef int (*cost_func)(int n);

/* Function signatures */
ull modpow(ull base, ull exp, ull modulus);
void dummy_init(ull s, ull t, ull m);
void readFromFiles(std::string infile, unsigned *vec);

/* Mathematical functions */
#define log2(N) (int)(log(N) / log(2))
#define log4(N) (int)(log(N) / log(4))
#define isPower4(n) n != 0 && ((n & (n - 1)) == 0) && !(n & 0xAAAAAAAA) // ?

/* Modulo functions */
#define MOD(a) a % mod
#define MOD_2(a) a - mod *floor(a * mod_inv)
#define MOD_MULT(cr, a, b, idx, s) cr = (a[s * idx] * b[idx]) % mod
#define MOD_MULT_2(cr, a, b, idx, s) cr = MOD_2((a[s * idx] * b[idx]))
#define MOD_ADD(c, a, b, mod) c = a + b < mod ? a + b : a + b - mod
#define MOD_SUB(c, a, b, mod) c = a >= b ? a - b : a - b + mod // cannot do "<= 0 ?" with unsigned

// Used in dft-rec.cpp
#define MOD_MULT2(cr, a, b, idx, s) cr = (a[s * idx] * 1) % mod
#define MOD_SUB_DFT(c, a, b, mod) c = a - b >= 0 ? a - b : a - b + mod

/* Colors */
#define _green_ "\033[1;32m"
#define _red_ "\033[1;31m"
#define _black_ "\033[0m"

#define RESET "\033[0m"
#define BLACK "\033[30m"              /* Black */
#define RED "\033[31m"                /* Red */
#define GREEN "\033[32m"              /* Green */
#define YELLOW "\033[33m"             /* Yellow */
#define BLUE "\033[34m"               /* Blue */
#define MAGENTA "\033[35m"            /* Magenta */
#define CYAN "\033[36m"               /* Cyan */
#define WHITE "\033[37m"              /* White */
#define BOLDBLACK "\033[1m\033[30m"   /* Bold Black */
#define BOLDRED "\033[1m\033[31m"     /* Bold Red */
#define BOLDGREEN "\033[1m\033[32m"   /* Bold Green */
#define BOLDYELLOW "\033[1m\033[33m"  /* Bold Yellow */
#define BOLDBLUE "\033[1m\033[34m"    /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m" /* Bold Magenta */
#define BOLDCYAN "\033[1m\033[36m"    /* Bold Cyan */
#define BOLDWHITE "\033[1m\033[37m"   /* Bold White */