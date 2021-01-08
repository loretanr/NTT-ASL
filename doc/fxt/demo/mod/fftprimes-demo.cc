
#include "mod/primes.h"  // rabin_miller()
#include "mod/factor.h"  // factorization

#include "nextarg.h"
#include "fxttypes.h"  // ulong

#include "fxtio.h"

#include <cmath>  // log(), pow()


//% Find "FFT primes", primes of the form p==y * 2^x + 1.


int
main(int argc, char **argv)
{
    ulong wb = 62;
    NXARG(wb, "word bits, wb<=63");

    double deltab = 0.01;
    NXARGFL(deltab, "results are in the range [wb-deltab, wb]");
    double minb = (double)wb - deltab;
    cout << " minb = " << minb << " = wb-" << deltab << endl;

    ulong minx  = 44;
    NXARG(minx, "log_2(min(fftlen))");

    ulong fq  = 0;
    NXARG(fq, "whether to filter results (for factors 3, 5, and 7)");


    umod_t minv = (umod_t)pow(2, minb);  //  min value
    for (ulong x=minx; x<=wb; ++x)
    {
        cout << "  ---- x = " << x << ": -----" << endl;
        umod_t fftv = (1ULL<<x);
        umod_t ymin = (umod_t)( ceil( (double)minv/(double)fftv )/2 )*2-1;
        if ( 0==(ymin&1) )  ymin = ymin - 1;  // must be odd

        for (umod_t y=ymin;  ; y+=2)
        {
            umod_t p = 1 + fftv * y;
            double bits = log((double)p)/log(2.0);
            if ( (ulong)bits >= wb )  break;

            if  ( bits >= minb )
            {
                if ( rabin_miller(p, 100) )
                {
                    factorization fy(y);

                    if ( fq )
                    {
                        if ( (fy.exponent_of(3)<=1) ||
                             (fy.exponent_of(5)<=1) ||
                             (fy.exponent_of(7)==0)
                             )  continue;
                    }

                    cout << p;
                    cout << " = 0x" << hex << p << dec;
                    cout << " = 1 + " << "2^" << x
                         << " * " << fy;
                    cout << "  (" << bits << " bits)" << endl;
                }
            }
        }
    }

    return 0;
}
// -------------------------

// =================== Typical:
// arg 1: 62 == wb
// arg 2: 0.03 == deltab
//  minb = 61.97 = wb-0.03
// arg 3: 44 == minx
// arg 4: 0 == fq
//   ---- x = 44: -----
// 4518218733973405697 = 0x3eb3f00000000001 = 1 + 2^44 * 256831  (61.9705 bits)
// 4518746499554738177 = 0x3eb5d00000000001 = 1 + 2^44 * 11 * 19 * 1229  (61.9706 bits)
// 4518852052671004673 = 0x3eb6300000000001 = 1 + 2^44 * 13 * 19759  (61.9707 bits)
// 4518957605787271169 = 0x3eb6900000000001 = 1 + 2^44 * 256873  (61.9707 bits)
// [-snip-]
// [-snip-]
// 4604226931544555521 = 0x3fe5800000000001 = 1 + 2^47 * 3^2 * 5 * 727  (61.9977 bits)
// 4605071356474687489 = 0x3fe8800000000001 = 1 + 2^47 * 3 * 13 * 839  (61.9979 bits)
//   ---- x = 48: -----
// 4540472849319591937 = 0x3f03000000000001 = 1 + 2^48 * 3 * 19 * 283  (61.9775 bits)
// 4585508845593296897 = 0x3fa3000000000001 = 1 + 2^48 * 11 * 1481  (61.9918 bits)
//   ---- x = 49: -----
// 4532310074994982913 = 0x3ee6000000000001 = 1 + 2^49 * 8051  (61.975 bits)
// 4545820873877094401 = 0x3f16000000000001 = 1 + 2^49 * 5^2 * 17 * 19  (61.9792 bits)
// 4576220171361845249 = 0x3f82000000000001 = 1 + 2^49 * 11 * 739  (61.9889 bits)
// 4582975570802900993 = 0x3f9a000000000001 = 1 + 2^49 * 7 * 1163  (61.991 bits)
// 4595360469778169857 = 0x3fc6000000000001 = 1 + 2^49 * 3^2 * 907  (61.9949 bits)
//   ---- x = 50: -----
// 4522739925786820609 = 0x3ec4000000000001 = 1 + 2^50 * 3 * 13 * 103  (61.9719 bits)
// 4601552919265804289 = 0x3fdc000000000001 = 1 + 2^50 * 61 * 67  (61.9968 bits)
//   ---- x = 51: -----
// 4546383823830515713 = 0x3f18000000000001 = 1 + 2^51 * 3 * 673  (61.9794 bits)
//   ---- x = 52: -----
//   ---- x = 53: -----
// [-snip-]
//   ---- x = 62: -----



// =================== Pushing FFT-length:
// arg 1: 62 == wb
// arg 2: 0.5 == deltab
//  minb = 61.5 = wb-0.5
// arg 3: 52 == minx
//   ---- x = 52: -----
// 3274116929098350593 = 0x2d70000000000001 = 1 + 2^52 * 727  (61.5058 bits)
// 3445253714938429441 = 0x2fd0000000000001 = 1 + 2^52 * 3^2 * 5 * 17  (61.5793 bits)
// 3904620876930220033 = 0x3630000000000001 = 1 + 2^52 * 3 * 17^2  (61.7599 bits)
// 4030721666496593921 = 0x37f0000000000001 = 1 + 2^52 * 5 * 179  (61.8057 bits)
// 4039728865751334913 = 0x3810000000000001 = 1 + 2^52 * 3 * 13 * 23  (61.809 bits)
// 4219872850846154753 = 0x3a90000000000001 = 1 + 2^52 * 937  (61.8719 bits)
// 4472074429978902529 = 0x3e10000000000001 = 1 + 2^52 * 3 * 331  (61.9556 bits)
//   ---- x = 53: -----
// 3377699720527872001 = 0x2ee0000000000001 = 1 + 2^53 * 3 * 5^3  (61.5507 bits)
// 3774016487736475649 = 0x3460000000000001 = 1 + 2^53 * 419  (61.7108 bits)
// 4044232465378705409 = 0x3820000000000001 = 1 + 2^53 * 449  (61.8106 bits)
// 4134304457926115329 = 0x3960000000000001 = 1 + 2^53 * 3^3 * 17  (61.8424 bits)
// 4242390848983007233 = 0x3ae0000000000001 = 1 + 2^53 * 3 * 157  (61.8796 bits)
// 4512606826625236993 = 0x3ea0000000000001 = 1 + 2^53 * 3 * 167  (61.9687 bits)
//   ---- x = 54: -----
//   ---- x = 55: -----
//   ---- x = 56: -----
//   ---- x = 57: -----
// 4179340454199820289 = 0x3a00000000000001 = 1 + 2^57 * 29  (61.858 bits)
//   ---- x = 58: -----
//   ---- x = 59: -----
//   ---- x = 60: -----
//   ---- x = 61: -----
//   ---- x = 62: -----


// =================== Pushing value (close to max):
// arg 1: 62 == wb
// arg 2: 0.001 == deltab
//  minb = 61.999 = wb-0.001
// arg 3: 44 == minx
//   ---- x = 44: -----
// 4610085129497346049 = 0x3ffa500000000001 = 1 + 2^44 * 3^2 * 11 * 2647  (61.9995 bits)
// 4610261051357790209 = 0x3ffaf00000000001 = 1 + 2^44 * 503 * 521  (61.9996 bits)
// 4610472157590323201 = 0x3ffbb00000000001 = 1 + 2^44 * 5^2 * 11 * 953  (61.9996 bits)
// 4610577710706589697 = 0x3ffc100000000001 = 1 + 2^44 * 137 * 1913  (61.9997 bits)
// 4610929554427478017 = 0x3ffd500000000001 = 1 + 2^44 * 3 * 7^2 * 1783  (61.9998 bits)
// 4610999923171655681 = 0x3ffd900000000001 = 1 + 2^44 * 5 * 19 * 31 * 89  (61.9998 bits)
// 4611105476287922177 = 0x3ffdf00000000001 = 1 + 2^44 * 262111  (61.9998 bits)
//   ---- x = 45: -----
// 4609258296753258497 = 0x3ff7600000000001 = 1 + 2^45 * 131003  (61.9992 bits)
// 4609610140474146817 = 0x3ff8a00000000001 = 1 + 2^45 * 3^2 * 14557  (61.9994 bits)
//   ---- x = 46: -----
// 4610208274799656961 = 0x3ffac00000000001 = 1 + 2^46 * 5 * 13103  (61.9995 bits)
// 4611615649683210241 = 0x3fffc00000000001 = 1 + 2^46 * 3 * 5 * 17 * 257  (62 bits)
//   ---- x = 47: -----
//   ---- x = 48: -----
// [-snip-]
//   ---- x = 62: -----

// ===================  Filtering:
// arg 1: 62 == wb
// arg 2: 0.1 == deltab
//  minb = 61.9 = wb-0.1
// arg 3: 40 == minx
// arg 4: 1 == fq
//   ---- x = 40: -----
// 4320668380299264001 = 0x3bf6190000000001 = 1 + 2^40 * 3^2 * 5^3 * 7 * 499  (61.906 bits)
// 4327595303554252801 = 0x3c0eb50000000001 = 1 + 2^40 * 3^3 * 5^2 * 7^3 * 17  (61.9083 bits)
// 4403791459359129601 = 0x3d1d690000000001 = 1 + 2^40 * 3^2 * 5^2 * 7 * 2543  (61.9335 bits)
// 4424572229124096001 = 0x3d673d0000000001 = 1 + 2^40 * 3^2 * 5^3 * 7^2 * 73  (61.9402 bits)
// 4434962614006579201 = 0x3d8c270000000001 = 1 + 2^40 * 3^2 * 5^2 * 7 * 13 * 197  (61.9436 bits)
// 4507695308183961601 = 0x3e8e8d0000000001 = 1 + 2^40 * 3^2 * 5^2 * 7 * 19 * 137  (61.9671 bits)
// 4531939539576422401 = 0x3ee4af0000000001 = 1 + 2^40 * 3^2 * 5^2 * 7 * 2617  (61.9748 bits)
// 4594281848871321601 = 0x3fc22b0000000001 = 1 + 2^40 * 3^2 * 5^2 * 7^2 * 379  (61.9945 bits)
//   ---- x = 41: -----
// 4526744347135180801 = 0x3ed23a0000000001 = 1 + 2^41 * 3^2 * 5^2 * 7 * 1307  (61.9732 bits)
// 4589086656430080001 = 0x3fafb60000000001 = 1 + 2^41 * 3^2 * 5^4 * 7 * 53  (61.9929 bits)
//   ---- x = 42: -----
// 4343180880877977601 = 0x3c46140000000001 = 1 + 2^42 * 3^3 * 5^2 * 7 * 11 * 19  (61.9135 bits)
// 4481719345977753601 = 0x3e32440000000001 = 1 + 2^42 * 3^2 * 5^2 * 7 * 647  (61.9588 bits)
//   ---- x = 43: -----
// 4557915501782630401 = 0x3f40f80000000001 = 1 + 2^43 * 3^2 * 5^2 * 7^2 * 47  (61.9831 bits)
//   ---- x = 44: -----
// 4405523190172876801 = 0x3d23900000000001 = 1 + 2^44 * 3^3 * 5^2 * 7 * 53  (61.934 bits)
//   ---- x = 45: -----
//   ---- x = 46: -----
// [-snip-]
//   ---- x = 62: -----


// ===================  32 bit:
// arg 1: 32 == wb
// arg 2: 1 == deltab
//  minb = 31 = wb-1
// arg 3: 24 == minx
// arg 4: 0 == fq
//   ---- x = 24: -----
// 2533359617 = 0x97000001 = 1 + 2^24 * 151  (31.2384 bits)
// 2634022913 = 0x9d000001 = 1 + 2^24 * 157  (31.2946 bits)
// 2868903937 = 0xab000001 = 1 + 2^24 * 3^2 * 19  (31.4179 bits)
// 3238002689 = 0xc1000001 = 1 + 2^24 * 193  (31.5925 bits)
// 3942645761 = 0xeb000001 = 1 + 2^24 * 5 * 47  (31.8765 bits)
// 4076863489 = 0xf3000001 = 1 + 2^24 * 3^5  (31.9248 bits)
//   ---- x = 25: -----
// 2717908993 = 0xa2000001 = 1 + 2^25 * 3^4  (31.3399 bits)
// 4194304001 = 0xfa000001 = 1 + 2^25 * 5^3  (31.9658 bits)
//   ---- x = 26: -----
// 2483027969 = 0x94000001 = 1 + 2^26 * 37  (31.2095 bits)
// 2885681153 = 0xac000001 = 1 + 2^26 * 43  (31.4263 bits)
//   ---- x = 27: -----
// 2281701377 = 0x88000001 = 1 + 2^27 * 17  (31.0875 bits)
// 3892314113 = 0xe8000001 = 1 + 2^27 * 29  (31.858 bits)
//   ---- x = 28: -----
// 3489660929 = 0xd0000001 = 1 + 2^28 * 13  (31.7004 bits)
//   ---- x = 29: -----
//   ---- x = 30: -----
// 3221225473 = 0xc0000001 = 1 + 2^30 * 3  (31.585 bits)
//   ---- x = 31: -----
//   ---- x = 32: -----


/// Emacs:
/// Local Variables:
/// MyRelDir: "demo/mod"
/// makefile-dir: "../../"
/// make-target: "1demo DSRC=demo/mod/fftprimes-demo.cc"
/// make-target2: "1demo DSRC=demo/mod/fftprimes-demo.cc DEMOFLAGS=-DTIMING"
/// End:

