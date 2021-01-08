
#include "comb/signed-perm.h"
#include "comb/perm-trotter.h"
#include "comb/ruler-func.h"

//#include "comb/fact2perm.h"

//#include "perm/perm2ccf.h"
//#include "perm/printcycles.h"
//#include "perm/permq.h"
//#include "perm/perm-genus.h"

#include "fxttypes.h"
#include "fxtio.h"
#include "nextarg.h"
#include "jjassert.h"


//% Signed permutations.

//#define TIMING  // uncomment to disable printing

int
main(int argc, char **argv)
{
    ulong n = 3;
    NXARG(n, "Signed permutations of n elements.");

    signed_perm SP(n);
    SP.first();


#ifdef TIMING
#ifdef PERM_TROTTER_OPT
    cout << "PERM_TROTTER_OPT is defined." << endl;
#endif
    ulong ct = 0;
    do { ++ct; } while ( SP.next() );
    cout << "ct=" << ct << endl;

#else  // TIMING

    ulong ct = 0;
    do
    {
        cout << setw(4) << ct << ":";
        ++ct;

        SP.print("    ", true);
        cout << "  " << ( SP.is_rotation() ? "R" : "F" );
        cout << endl;
    }
    while ( SP.next() );
//    while ( SP.next_rotation() );

    cout << " ct=" << ct << endl;
#endif  // TIMING

    return 0;
}
// -------------------------

/*
Timing: (Intel(R) Core(TM) i7-8700K CPU @ 3.70GHz)
GCC 8.3.0

time ./bin 10
arg 1: 10 == n  [Signed permutations of n elements.]  default=3
PERM_TROTTER_OPT is defined.
ct=3715891200
./bin 10  10.49s user 0.00s system 99% cpu 10.495 total
 ==> 3715891200 / 10.49 == 354,231,763 per second
*/



/// Emacs:
/// Local Variables:
/// MyRelDir: "demo/comb"
/// makefile-dir: "../../"
/// make-target: "1demo DSRC=demo/comb/signed-perm-demo.cc"
/// make-target2: "1demo DSRC=demo/comb/signed-perm-demo.cc DEMOFLAGS=-DTIMING"
/// End:

