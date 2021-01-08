#include <iostream>
#include <papi.h>
#include <fstream>
#include <immintrin.h>
#include "ntt-radix4-avx0.h"
#include <cmath>


using namespace std;

typedef unsigned long long ull;
#define isPower4(n) n != 0 && ((n & (n - 1)) == 0) && !(n & 0xAAAAAAAA) // ?
#define log2(N) (int)(log(N) / log(2))
#define log4(N) (int)(log(N) / log(4))
#define MOD(a) a % mod
#define MOD_2(a) a - mod *floor(a * mod_inv);
#define MOD_MULT(cr, a, b, idx, s) cr = (a[s * idx] * b[idx]) % mod
#define MOD_MULT_2(cr, a, b, idx, s) cr = MOD_2((a[s * idx] * b[idx]))
#define MOD_ADD(c, a, b, mod) c = a + b < mod ? a + b : a + b - mod
#define MOD_SUB(c, a, b, mod) c = a >= b ? a - b : a - b + mod // cannot do "<= 0 ?" with unsigned

// Used in dft-rec.cpp
#define MOD_MULT2(cr, a, b, idx, s) cr = (a[s * idx] * 1) % mod
#define MOD_SUB_DFT(c, a, b, mod) c = a - b >= 0 ? a - b : a - b + mod

ull root4, mod, baseroot;
double mod_inv;
ull **DN;
ull *DN_power2;
unsigned *even, *odd;



ull modpow(ull base, ull exp, ull modulus)
{
    ull result = 1;
    while (exp > 0)
    {
        if (exp & 1)
            result = (result * base) % modulus;
        base = (base * base) % modulus;
        exp >>= 1;
    }
    return result;
}

void readFromFiles(std::string infile, unsigned *vec)
{
    std::ifstream instream;
    instream.open(infile, ios::in);
    if (!instream.fail())
    {
        unsigned x;
        int i = 0; // Index used for input vector
        while (instream)
        {
            instream >> x;     // Read number
            instream.ignore(); // Ignore comma
            vec[i] = x;        // Load number into input vector
            i++;
        }
    }
    else { // File failed to open
        std::cout << "File " << infile << " failed to open" << endl;
    }
}

void NTT_radix4_base(unsigned *Y, unsigned *X, ull N, ull _root, ull _mod)
{
    if (isPower4(N))
    {

        NTT_rec(N, log4(N), Y, X, 1);
    }
    else
    {
        ull N_div_2 = N / 2;
        NTT_rec(N_div_2, log4(N_div_2), even, X, 2);
        NTT_rec(N_div_2, log4(N_div_2), odd, X + 1, 2);

        // twiddle 2
        ull w = 1;

        for (int k = 0; k < N_div_2; ++k)
        {
            ull t = MOD(w * odd[k]);
            MOD_ADD(Y[k], even[k], t, _mod);
            MOD_SUB(Y[k + N_div_2], even[k], t, _mod);
            w = DN_power2[k];
        }
    }
}

// s: stride to access X
void NTT_rec(ull N, ull n, unsigned *Y, unsigned *X, ull s)
{
    int j;
    int N_div_4 = N / 4;
    if (N == 4)
    {
        NTT4_base(Y, X, s);
    }
    else
    {
        for (j = 0; j < 4; j++)
        {
            NTT_rec(N_div_4, n - 1, Y + (N_div_4)*j, X + j * s, s * 4);
        }
        for (j = 0; j < N_div_4; j += 4)
        {
            NTT4_twiddle(Y + (j + 0), N_div_4, n, j + 0);
            NTT4_twiddle(Y + (j + 1), N_div_4, n, j + 1);
            NTT4_twiddle(Y + (j + 2), N_div_4, n, j + 2);
            NTT4_twiddle(Y + (j + 3), N_div_4, n, j + 3);
        }
    }
}

// s: stride, should always be 1
inline void NTT4_base(unsigned *Y, unsigned *X, ull s)
{
    ull X0 = X[0 * s];
    ull X1 = X[1 * s];
    ull X2 = X[2 * s];
    ull X3 = X[3 * s];
    ull t0, t1, t2, t3;

    MOD_ADD(t0, X0, X2, mod);
    MOD_SUB(t1, X0, X2, mod);
    MOD_ADD(t2, X1, X3, mod);
    MOD_SUB(t3, X1, X3, mod);

    t3 = MOD(baseroot * t3);

    MOD_ADD(Y[0], t0, t2, mod);
    MOD_ADD(Y[1], t1, t3, mod);
    MOD_SUB(Y[2], t0, t2, mod);
    MOD_SUB(Y[3], t1, t3, mod);
}

inline void NTT4_twiddle(unsigned *Y, ull s, ull n, ull j)
{
    ull t0, t1, t2, t3,
        X0, X1, X2, X3;
    ull *Dj;
    // complex multiplications from D_N
    Dj = DN[n - 2] + 4 * j;

    MOD_MULT(X0, Y, Dj, 0, s);
    MOD_MULT(X1, Y, Dj, 1, s);
    MOD_MULT(X2, Y, Dj, 2, s);
    MOD_MULT(X3, Y, Dj, 3, s);

    // operations from NTT4
    MOD_ADD(t0, X0, X2, mod);
    MOD_SUB(t1, X0, X2, mod);
    MOD_ADD(t2, X1, X3, mod);
    MOD_SUB(t3, X1, X3, mod);

    t3 = MOD(baseroot * t3);

    MOD_ADD(Y[0 * s], t0, t2, mod);
    MOD_ADD(Y[1 * s], t1, t3, mod);
    MOD_SUB(Y[2 * s], t0, t2, mod);
    MOD_SUB(Y[3 * s], t1, t3, mod);
}



void init_r4_NTT(ull N, ull _root, ull _mod)
{
    // NEW: initialize all the root-stuff + helper arrays here
    if (isPower4(N))
    {
        root4 = _root;
        mod = _mod;
        mod_inv = 1.0 / mod;
        int n = log4(N);
        baseroot = modpow(root4, pow(4, n - 1), mod);
    }
    else
    {
        int N2 = N >> 1;
        int n2 = log4(N2);
        mod = _mod;
        mod_inv = 1.0 / mod;
        root4 = modpow(_root, 2, _mod);
        baseroot = modpow(root4, pow(4, (n2 - 1)), _mod);
        even = (unsigned *)malloc(sizeof(unsigned) * N2);
        odd = (unsigned *)malloc(sizeof(unsigned) * N2);

        N = N2;
    }

    // fill twiddle table
    int i, j, k, size_Dj = 16, n_max = log4(N);
    DN = (ull **)malloc(sizeof(ull *) * (n_max));
    DN_power2 = (ull *)malloc(sizeof(ull) * (N));

    for (j = 1; j < n_max; j++, size_Dj *= 4)
    {
        ull *Dj = DN[j - 1] = (ull *)malloc(sizeof(ull) * size_Dj);
        for (k = 0; k < size_Dj / 4; k++)
        {
            for (i = 0; i < 4; i++)
            {
                *(Dj++) = modpow(root4, pow(4, n_max - j - 1) * (k * i), mod);
            }
        }
    }
    ull wd = _root;
    for (int i = 0; i < N; i++)
    {
        DN_power2[i] = modpow(wd, i + 1, mod);
    }
}

void init_testcase(ull &mod, ull &n_th_root, ull &size, unsigned *data, unsigned *t_data, int test_size){
    switch(test_size){
        case(64):
            mod = 23068673;n_th_root = 9826950;size = 64;
            readFromFiles("tests/23068673_3_9826950_64.in", data);
            break;
        case(128):
            mod = 23068673;n_th_root = 21648390;size = 128;
            readFromFiles("tests/23068673_3_21648390_128.in", data);
            break;
        case(256):
            mod = 23068673;n_th_root = 16583093;size = 256;
            readFromFiles("tests/23068673_3_16583093_256.in", data);
            break;
        case(512):
            mod = 23068673;n_th_root = 6321789;size = 512;
            readFromFiles("tests/23068673_3_6321789_512.in", data);
            break;
        case(1024):
            mod = 23068673;n_th_root = 4964576;size = 1024;
            readFromFiles("tests/23068673_3_4964576_1024.in", data);
            break;
        case(2048):
            mod = 23068673;n_th_root = 9315417;size = 2048;
            readFromFiles("tests/23068673_3_9315417_2048.in", data);
            break;
        case(1048576):
            mod = 23068673;n_th_root = 7664329;size = 1048576;
            readFromFiles("tests/23068673_3_7664329_1048576.in", data);
            break;
        case(2097152):
            mod = 23068673;n_th_root = 177147;size = 2097152;
            readFromFiles("tests/23068673_3_177147_2097152.out", t_data);
            break;
        default:
            break;
    }
}

void handle_error(int err){
    std::cerr << "PAPI error: " << err << std::endl;
}

// global so cache flush doesnt get optimized
char *cache;

void cache_flush_4MB(){
    const int flush_size = 1024*1024*4; // Allocate 4MB. Which is my L3
        cache = (char *) malloc(flush_size * sizeof(char));
        for (int i = 0; i < 0xf; i++)
            for (int j = 0; j < flush_size; j++)
                cache[j] = i+j; // write some stuff
}
#include <algorithm>

void simple_iter(unsigned *X, ull N, ull _root, ull _mod){
    int gen = 15485867; // coprime -> will generate whole group under addition
    int curr = 0;
    for(int i=0; i<N-1; i++){
        X[curr] += gen; // access
        curr += gen;
        curr %= N;
    }
}

void init_large_random(unsigned *data, int test_size){
    for(int i=0; i<test_size; i++){
        data[i] = i;
    }
    random_shuffle(&data[0], &data[test_size-1]);

    cout << "gen rand done" << endl;
}


int main(int argc, char* argv[]){

    int REP = 1;

    int N = 104395303; // prime size 104 mio

    /*
    ull mod, n_th_root, size; 
    unsigned *data = (unsigned *) malloc(test_size * sizeof(unsigned)); 
    unsigned *t_data = (unsigned *) malloc(test_size * sizeof(unsigned)); 
    init_testcase(mod,n_th_root,size,data,t_data,test_size);

    init_r4_NTT(size, n_th_root, mod);
    */
    unsigned *data = (unsigned *) malloc(N * sizeof(unsigned)); 
    init_large_random(data, N); 
      
    const int numEvents = 3;

    long long end_values[numEvents] = {0,0,0};
    long long values[numEvents];
    int events[numEvents] = {PAPI_L3_LDM, PAPI_L3_TCM, PAPI_PRF_DM};    

    for (int i = 0; i < REP; i++)
    {

        // here flush cache
        cache_flush_4MB();
                

        if (PAPI_start_counters(events, numEvents) != PAPI_OK)
            handle_error(1);

        //NTT_radix4_base(t_data, data, size, n_th_root, mod);
        simple_iter(data, N, 1, 1);

        if ( PAPI_stop_counters(values, numEvents) != PAPI_OK)
            handle_error(1);

        free(cache); // free

        for(int j=0; j<numEvents; j++){
            end_values[j] += values[j];
        }
    }

    //cache[5] = data[7]; // touch
    //cache[17] = end_values[0]; // touch it, anti optimize away

    
    for(int j=0; j<numEvents; j++){
        end_values[j] /= REP;
    }

    std::cout<<"Level 3 load misses: "<< end_values[0] << endl;
    //std::cout<<"Level 3 cache misses: "<< end_values[1] << endl;
    //std::cout<<"Data prefetch cache misses: "<< end_values[2] << endl;

    //cout<<"L3 misses: "<<values[1]<<endl;
    //cout<<"L3 miss/access ratio: "<<(double)values[1]/values[0]<<endl;

    free(data);

    return 0;
}