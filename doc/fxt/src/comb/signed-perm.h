#if !defined  HAVE_SIGNED_PERM_H__
#define       HAVE_SIGNED_PERM_H__
// This file is part of the FXT library.
// Copyright (C) 2020 Joerg Arndt
// License: GNU General Public License version 3 or later,
// see the file COPYING.txt in the main directory.

#include "comb/perm-trotter.h"
#include "comb/ruler-func.h"

#include <iostream>


class signed_perm
// Signed permutations.
// There are n! * 2^n signed reflections of n elements.
{
private:
    perm_trotter P;  // permutation of coordinates
    ruler_func R;
    bool * refl_q;    // refl_q[j] says whether coordinate j is reflected
    bool perm_is_rot;    // whether permutation alone is a rotation
    bool refl_is_rot;    // whether reflections alone give a rotation
    ulong n;

    signed_perm(const signed_perm&) = delete;
    signed_perm & operator = (const signed_perm&) = delete;

public:
    signed_perm(ulong tn)
        : P(tn), R(tn), n(tn)
    {
        refl_q = new bool[n];
        first();
    }
    ~signed_perm()
    {
        delete [] refl_q;
    }

private:
    void first_refl()
    {
        R.first();
        // no coordinate reflected:
        refl_is_rot = false;
        for (ulong j=0; j<n; ++j)
            refl_q[j] = false;
    }

public:
    void first()
    {
        P.first();
        perm_is_rot = true;

        first_refl();
    }

    bool next()
    {
        // next reflection:
        const ulong r = R.next();
        refl_is_rot = ! refl_is_rot;  // one coordinate (namely r) is reversed
        if ( r < n )
        {
            refl_q[r] = ! refl_q[r];  // flip reflection for coordinate r
            return true;
        }
        // done all reflections
        first_refl();

        // next permutation:
        perm_is_rot = ! perm_is_rot;
        return P.next();
    }

    bool next_rotation()
    {
        do
        {
            if ( ! next() )  return false;  // last
        }
        while ( ! is_rotation() );  // max two in a row
        return true;
    }

    bool is_rotation()  const
    {
        return refl_is_rot ^ perm_is_rot;
    }

    const ulong * perm_data()  const
    {
        return P.data();
    }

    const bool * refl_data()  const
    {
        return refl_q;
    }

    void print(const char * bla, bool offset_one_q=false)  const
    {
        using std::cout;
        cout << bla;
        cout << "(";
        for (ulong j =0; j<n; ++j)
        {
            cout << ( refl_data()[j] ? '-' : '+' )
                 << perm_data()[j] + offset_one_q
                 << ( j+1 == n ? "" : ", ");
        }
        cout << ")";

//        cout << "    (";
//        for (ulong j =0; j<n; ++j)
//        {
//            cout << ( refl_data()[j] )
//                 << " "
//                 << perm_data()[j] + offset_one_q
//                 << ( j+1 == n ? "" : ",  ");
//        }
//        cout << ")";
    }
};
// -------------------------

#endif // !defined HAVE_SIGNED_PERM_H__
