
#include "comb/perm-prefix-cond.h"  // class perm_prefix_cond

#include "comb/comb-print.h"

#include "fxtio.h"
#include "fxttypes.h"

//#include "jjassert.h"

#include "nextarg.h"  // NXARG()

//% OEIS sequence A309807:
//% Number of permutations sigma of [n] such that sigma(k)/k > sigma(k+1)/(k+1) for 1 <= k <= n-1.


// Cf. comb/mixedradix-prefix-cond-demo.cc for mixed radix numbers with restricted prefixes.

static ulong N;  // permutations of length N


//#define TIMING  // uncomment to disable printing

bool cond_A309807(const ulong *a, ulong k)
{
//    jjassert( k != 0 );
    if ( k==1 )  return  true;

    return  ( a[k-1] * (k) > a[k] * (k-1) );
}
// -------------------------

#ifdef TIMING
void visit(const ulong *, ulong, ulong)  { ; }
#else // TIMING
void visit(const ulong *a, ulong n, ulong ct)
{
    cout << setw(4) << ct << ":";
    print_perm("  ", a+1, n);

    cout << endl;
}
// -------------------------
#endif  // TIMING


int
main(int argc, char **argv)
{
    ulong n = 7;
    N = n;
    NXARG(n, "Permutations of {1, 2, ..., n}");

    ulong c = 1;
//    NXARG(c, "Condition function:" );

    perm_prefix_cond *perm;
    switch ( c )
    {
    case 1:
        cout << "A309807:" << endl;
        perm = new perm_prefix_cond(n, cond_A309807, visit);
        break;
//    case 2:
//        cout << "?? permutations:" << endl;
//        perm = new perm_prefix_cond(n, cond_updown, visit);  break;
    default:
        cout << "Error!" << endl;
        return 1;
    }

    ulong ct = perm->all();
    cout << " ct=" << ct << endl;

    return 0;
}
// -------------------------

/*
for n in $(seq 1 30); do ./bin $n; done


 */

/// Emacs:
/// Local Variables:
/// MyRelDir: "demo/seq"
/// makefile-dir: "../../"
/// make-target: "1demo DSRC=demo/seq/A309807-demo.cc"
/// make-target2: "1demo DSRC=demo/seq/A309807-demo.cc DEMOFLAGS=-DTIMING"
/// End:

