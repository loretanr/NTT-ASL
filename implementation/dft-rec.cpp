// recursive radix-4 DFT implementation
#include <math.h>
//#include "fft.h"
#include <stdlib.h>
#include <iostream>

#include "utils.h"

using namespace std;

int root;
int mod;
int baseroot;

int **DN;

// top-level call to DFT function
void DFT4(int N, int *Y, int *X, int _root, int _mod)
{
    root = _root;
    mod = _mod;
    baseroot = modpow(root, pow(4, log4(N)-1), mod);
    DFT_rec(N, log4(N), Y, X, 1);
}





// recursive radix-4 DFT function
// N: problem size
// Y: output vector
// X: input vector
// s: stride to access X
void DFT_rec(int N, int n, int *Y, int *X, int s)
{   
    if( N != 4) {
        cout << "-- rec, size " << N << " (stage " << n << ")" << endl;
    }
    int j;
    if (N == 4)
    {
        // Y = DFT_4 X
        DFT4_base(Y, X, s);
    }
    else
    {
        // Y = (I_4 x DFT_N/4)(LˆN_4) X
        for (j = 0; j < 4; j++)
            DFT_rec(N / 4, n - 1, Y + (1 * (N / 4) * j), X + 1 * j * s, s * 4);
        // Y = (DFT_4 x I_{N/4})(D_N,4) Y

        for (j = 0; j < N / 4; j++)
        {
            DFT4_twiddle(Y + 1 * j, N / 4, n, j);
            // for (int i = 0; i < 4; i++)
            // {
            //     std::cout << CYAN << Y[j + i] << " ";
            // }
            // std::cout << std::endl;
        }
    }
}

int modpow(int base, int exp, int modulus)
{
    int result = 1;
    while (exp > 0)
    {
        if (exp & 1)
            result = (result * base) % modulus;
        base = (base * base) % modulus;
        exp >>= 1;
    }
    return result;
}

// s: stride, should always be 1
static void DFT4_base(int *Y, int *X, int s)
{
    cout << "-- base, size " << 4 << endl;
    int t0, t1, t2, t3;
    MOD_ADD(t0, X[0 * s], X[2 * s], mod);
    MOD_SUB_DFT(t1, X[0 * s], X[2 * s], mod);
    MOD_ADD(t2, X[1 * s], X[3 * s], mod);
    MOD_SUB_DFT(t3, X[1 * s], X[3 * s], mod);

    MOD_ADD(Y[0], t0, t2, mod);
    MOD_SUB_DFT(Y[2], t0, t2, mod);
    t3 = baseroot * t3 % mod;
    MOD_ADD(Y[1], t1, t3, mod);
    MOD_SUB_DFT(Y[3], t1, t3, mod);
}

//define PI 3.14159265358979323846
// twiddle table, initialized by init_DFT(N)
// int **DN;

// DFT4*D_j implementation
static void DFT4_twiddle(int *Y, int s, int n, int j)
{
    cout << "-- twiddle (stage " << n << ")" << endl;
    int t0, t1, t2, t3,
        X0, X1, X2, X3;
    int *Dj;
    // complex multiplications from D_N
    Dj = DN[n - 2] + 4 * j;
    MOD_MULT(X0, Y, Dj, 0, s);
    MOD_MULT(X1, Y, Dj, 1, s);
    MOD_MULT(X2, Y, Dj, 2, s);
    MOD_MULT(X3, Y, Dj, 3, s);
    // std::cout << RED << Y[s * 0] << " * " << Dj[0] << " = " << X0 << std::endl;
    // std::cout << RED << Y[s * 1] << " * " << Dj[1] << " = " << X1 << std::endl;
    // std::cout << RED << Y[s * 2] << " * " << Dj[2] << " = " << X2 << std::endl;
    // std::cout << RED << Y[s * 3] << " * " << Dj[3] << " = " << X3 << std::endl;

    // operations from DFT4
    MOD_ADD(t0, X0, X2, mod); //t02
    MOD_SUB_DFT(t1, X0, X2, mod);
    MOD_ADD(t2, X1, X3, mod); //t13
    MOD_SUB_DFT(t3, X1, X3, mod);

    MOD_ADD(Y[0 * s], t0, t2, mod); //<--Stimmt 1/2
    MOD_SUB_DFT(Y[2 * s], t0, t2, mod); //<--Stimmt 1/2
    t3 = baseroot * t3 % mod;
    MOD_ADD(Y[1 * s], t1, t3, mod); //<--Falsch
    MOD_SUB_DFT(Y[3 * s], t1, t3, mod); //<--Falsch
    //cout << WHITE << "Y: " << Y[0 * s] << CYAN << " " << Y[1 * s] << RESET << WHITE << " " << Y[2 * s] << CYAN << " " << Y[3 * s] << RESET << " " << endl;
}

void init_DFT(int N)
{
    int i, j, k, size_Dj = 16, n_max = log4(N);
    DN = (int **)malloc(sizeof(int *) * (n_max - 1));

    // ... w^4*4*4 | w^4*4 | w^4 | w |
    int *baseroots = new int[log4(N)];
    int b_value = root;
    for(int i=log4(N)-1; i > 0; i--){
        baseroots[i] = b_value;
        b_value = modpow(b_value, 4, mod);
    }

    //int baseroot = modpow(root,4,mod);

    for (j = 1; j < n_max; j++, size_Dj *= 4)
    {
        int *Dj = DN[j - 1] = (int *)malloc(sizeof(int) * size_Dj);
        for (k = 0; k < size_Dj / 4; k++)
        {
            for (i = 0; i < 4; i++)
            {
                *(Dj++) = modpow(baseroots[j], (k * i), mod);
            }
        }
        //baseroot = root;
    }
    cout << "twiddles \n";
    for(int i=0; i<16; i++){
        cout << DN[0][i] << " ";
    } cout << endl;
    for(int i=0; i<16; i++){          //  64 -> exists only 0 and 1
        cout << DN[1][i] << " ";      // last one is included in base
    } cout << endl;

}

void destroy_DFT(int N)
{
    int j, size_Dj = 16, n_max = log4(N);
    for (j = 1; j < n_max; j++, size_Dj *= 4)
    {
        free(DN[j - 1]);
    }
    free(DN);
}

int main()
{
    // size 4
    int modulus;
    mod = modulus = 97;
    int primitive_root = 5;
    int n_th_root;
    root = n_th_root = 22;
    int size = 4;
    int *data = new int[size]{60, 31, 89, 12};
    int *t_data = new int[size]{95, 1, 9, 38};
    int *out = new int[size];
    // size 8
    // modulus = 673; primitive_root = 5; n_th_root = 609; size = 8;
    // data = new int[size] {4,1,4,2,1,3,5,6};
    // t_data = new int[size] {26,115,437,338,2,448,228,457};
/*
    mod = modulus = 769;
    primitive_root = 11;
    root = n_th_root = 136;
    size = 16;
    data = new int[size]{97, 3, 34, 80, 22, 59, 74, 96, 93, 1, 52, 97, 36, 94, 62, 88};
    t_data = new int[size]{219, 559, 627, 329, 451, 731, 245, 129, 721, 425, 358, 761, 370, 212, 67, 731};
    // mod = modulus = 769; primitive_root = 11; root = n_th_root = 136; size = 4;
    // data = new int[size] {97,22,93,36};//,22,59,74,96,93,1,52,97,36,94,62,88};
    // data = new int[size] {3,59,1,94};//,22,59,74,96,93,1,52,97,36,94,62,88};
    // data = new int[size]{219, 559, 627, 329, 451, 731, 245, 129, 721, 425, 358, 761, 370, 212, 67, 731};
    // t_data = new int[size] {14, 639, 223, 735, 576, 14, 63, 16, 719, 767, 415, 175, 352, 511, 544, 48};

    // data = new int[size]{26,115,437,338,2,448,228,457,4,1,4,2,1,3,5,6};
    // t_data = new int[size] {539, 652, 545, 91, 107, 24, 387, 601, 106, 578, 693, 614, 149, 620, 21, 72};
*/
    mod = modulus = 40961;
    root = n_th_root = 14529;
    size = 64;
    data = new int[size]{56,28,11,66,53,32,10,23,58,72,100,31,66,42,69,59,11,61,42,24,68,92,87,53,70,28,90,41,100,1,88,24,37,19,35,54,58,10,98,24,61,79,12,100,46,38,3,37,87,55,43,3,42,31,26,61,91,10,3,41,48,54,18,43};
    t_data = new int[size]{3023,34792,11635,32850,32484,13824,15800,28294,6388,32257,38628,18484,39384,11996,13522,5642,26437,21986,40194,19075,30555,24893,18085,14199,12827,16697,16433,25404,27165,38371,26606,16996,351,30492,35051,16516,25036,31608,30979,35976,4671,6434,26851,7686,13002,22358,26882,32920,14958,18112,4704,16734,18911,5906,15854,25672,17035,10600,19852,15143,17556,20962,27493,3105};
   /* 
    mod = modulus = 40961;
    root = n_th_root = 36043;
    size = 256;
    data = new int[size]{12,27,14,1,25,81,86,76,30,93,37,30,40,50,84,59,2,7,61,75,34,21,57,95,93,91,42,90,10,92,38,28,31,26,74,97,9,43,86,24,54,17,18,88,88,12,0,49,16,23,33,53,64,36,52,25,42,73,6,95,32,50,33,70,8,15,43,74,19,82,95,43,90,86,46,15,0,74,74,46,45,96,6,17,98,97,40,55,6,9,67,46,32,67,75,31,78,7,47,95,42,36,37,17,83,44,54,95,7,33,28,77,25,58,74,1,66,62,25,12,100,84,86,53,50,5,39,20,55,80,55,60,83,95,27,58,80,47,29,62,37,9,78,61,50,98,49,74,31,5,67,21,58,16,37,10,98,98,63,70,87,57,58,80,100,97,73,82,48,92,92,58,57,18,56,3,93,15,30,94,65,92,67,25,87,7,90,14,63,42,82,90,33,16,28,49,94,72,37,30,22,31,14,61,83,97,65,91,89,71,44,53,86,16,15,63,22,91,58,53,99,79,71,56,47,62,85,8,37,90,93,71,54,59,14,19,36,68,55,46,84,72,24,41,48,80,52,60,53,66,27,17,23,4,55,78};
    t_data = new int[size]{13329,15412,37492,35793,9702,34650,21794,5849,27503,31789,26454,34642,5119,5786,32394,502,3227,40146,29073,20559,26978,11310,8902,5900,26052,33816,21592,24437,4602,40628,9905,14548,21945,32464,7800,1781,30615,6712,37968,23208,22027,23670,24251,33087,26170,7188,10678,38955,30980,30686,10469,19554,5242,14366,13170,1645,32593,7673,9217,6447,28342,5147,15191,35470,32630,14205,8836,15672,38398,24161,13553,4152,29003,451,14022,4395,3531,23556,5303,24333,39707,21411,10356,6332,20689,25381,30167,18548,37326,10783,23738,26944,29436,7100,39067,39486,38691,34201,16450,32041,9307,27413,17007,13580,4651,23655,27394,18951,25390,31491,9990,3720,8151,20764,29154,23865,38314,845,7975,24913,2291,36737,33374,20437,20897,699,28352,7744,40792,8028,35154,25384,27442,26577,11245,623,9013,9949,36971,1378,36975,10043,16232,3921,40940,21097,40792,39088,33598,33890,16538,21074,23779,3183,5022,19541,16187,12853,37607,28850,30230,2990,7038,6600,19311,10396,7720,25257,16888,14965,15874,40545,40600,16247,31388,5269,19942,33520,32718,35557,2896,37730,8027,6825,12944,34037,26251,33897,13997,622,39543,14416,8503,29211,29363,8896,17771,19637,1298,23555,12254,18410,23970,26784,31427,2246,6543,21469,10397,13960,6423,29974,22530,25896,17570,38395,8391,18798,38004,29637,36633,18945,26772,21799,32101,7985,35959,11450,36321,22541,33437,554,1120,39390,5113,497,25721,19864,33198,7583,9164,1117,13442,4230,37285,16414,40650,37858,20044,11449,35099,31252,11552,40040,7328,36182};
    */
    out = new int[size];
    init_DFT(size);
    DFT4(size, out, data, n_th_root, modulus);
    
    for (int i = 0; i < size; i++)
    {
        if (out[i] == t_data[i])
            std::cout << BOLDGREEN << out[i] << RESET << " ";
        else
            std::cout << WHITE << out[i] << " ";
    }
    std::cout << std::endl;
    destroy_DFT(size);
}