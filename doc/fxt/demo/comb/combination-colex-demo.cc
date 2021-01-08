
#include "comb/combination-colex.h"

#include "comb/comb-print.h"

#include "jjassert.h"

#include "fxttypes.h"
#include "fxtio.h"

#include "nextarg.h"

//% Generating all combinations in co-lexicographic order.

//#define TIMING  // uncomment to disable printing

int
main(int argc, char **argv)
{
    ulong n = 7, k = 4;
    NXARG(n, "Combinations (n choose k) in co-lexicographic order:  n>0");
    NXARG(k, " 0 < k <= n");
    bool rq = 0;
    NXARG(rq, "Whether to reverse order");

    combination_colex C(n, k);

    if ( 0==rq )  C.first();
    else          C.last();

    ulong ct = 0;

#ifdef TIMING
    if ( 0==rq )  do  { ++ct; }  while ( k!=C.next() );
    else          do  { ++ct; }  while ( k!=C.prev() );

#else
    ulong *tt = new ulong[k];  // aux for reversed sets
    ulong j = k - 1;
    do
    {
        ++ct;
        cout << setw(4) << ct << ":";
        C.print_set("    ");
        cout << "  " << j << "  ";
        C.print_deltaset("    ");
        for (ulong i=0; i<k; ++i)  tt[i] = C.data()[k-1-i];
        print_set("    ", tt, k);
        cout << endl;
        if ( 0==rq )  j = C.next();
        else          j = C.prev();
    }
    while ( j < k );
    cout << endl;
#endif  // TIMING

    cout << " ct = " << ct << endl;

#ifndef TIMING
    delete [] tt;
#endif  // TIMING

    return 0;
}
// -------------------------

/*
Timing: (Intel(R) Core(TM) i7-8700K CPU @ 3.70GHz)

 time ./bin 32 20
arg 1: 32 == n  [Combinations (n choose k) in co-lexicographic order:  n>0]  default=7
arg 2: 20 == k  [ 0 < k <= n]  default=4
arg 3: 0 == rq  [Whether to reverse order]  default=0
 ct = 225792840
./bin 32 20  0.54s user 0.00s system 99% cpu 0.535 total
 ==> 225792840/0.54 == 418,134,888 per second

 time ./bin 32 12
arg 1: 32 == n  [Combinations (n choose k) in co-lexicographic order:  n>0]  default=7
arg 2: 12 == k  [ 0 < k <= n]  default=4
arg 3: 0 == rq  [Whether to reverse order]  default=0
 ct = 225792840
./bin 32 12  0.42s user 0.00s system 99% cpu 0.421 total
 ==> 225792840/0.42 == 537,602,000 per second

*/

/// Emacs:
/// Local Variables:
/// MyRelDir: "demo/comb"
/// makefile-dir: "../../"
/// make-target: "1demo DSRC=demo/comb/combination-colex-demo.cc"
/// make-target2: "1demo DSRC=demo/comb/combination-colex-demo.cc DEMOFLAGS=-DTIMING"
/// End:

