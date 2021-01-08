#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>

#include "../implementation/FTL/nttw/include/number32.h"

#include "../implementation/utils.h"

#include "../implementation/ntt-naive.h"
#include "../implementation/ntt-radix2.h"
#include "../implementation/ntt-radix4.h"

#include "../implementation/scalar/radix2/ntt-radix2-scalar0.h"
#include "../implementation/scalar/radix2/ntt-radix2-scalar1.h"
#include "../implementation/scalar/radix2/ntt-radix2-scalar2.h"
#include "../implementation/scalar/radix2/ntt-radix2-scalar3.h"

#include "../implementation/scalar/radix4/ntt-radix4-scalar0.h"
#include "../implementation/scalar/radix4/ntt-radix4-scalar1.h"
#include "../implementation/scalar/radix4/ntt-radix4-scalar2.h"
#include "../implementation/scalar/radix4/ntt-radix4-scalar-with-init.h"


#include "../implementation/avx/radix2/ntt-radix2-avx0.h"
#include "../implementation/avx/radix2/ntt-radix2-avx1.h"
#include "../implementation/avx/radix2/ntt-radix2-avx2.h"
#include "../implementation/avx/radix2/ntt-radix2-avx3.h"
#include "../implementation/avx/radix2/ntt-radix2-avx4.h"

#include "../implementation/avx/radix4/ntt-radix4-avx0.h"
#include "../implementation/avx/radix4/ntt-radix4-avx-unopt.h"
#include "../implementation/avx/radix4/ntt-radix4-avx-sawtooth.h"
#include "../implementation/avx/radix4/ntt-radix4-avx2.h"
#include "../implementation/avx/radix4/ntt-radix4-avx3.h"
#include "../implementation/avx/radix4/ntt-radix4-avx4.h"
#include "../implementation/avx/radix4/ntt-radix4-avx5.h"
#include "../implementation/avx/radix4/ntt-radix4-avx6.h"
#include "../implementation/avx/radix4/ntt-radix4-avx7.h"
#include "../implementation/avx/radix4/ntt-radix4-avx8.h"

#include "../implementation/avx/radix4/ntt-radix4-avx-all.h"


using namespace std;

struct Transform
{
  transform_func f;
  string name;
  init_func init_f;
  bool requiresInit;
  bool reorderInput;
};

struct Testcase
{
  unsigned *data, *t_data;
  ull size, modulus, primitive_root, n_th_root;
  string name;
};

/* Globals */
vector<Testcase> testcases;
vector<Transform> transforms;

void read_from_files(string name, Testcase &t)
{
    string infile = name + ".in";
    string outfile = name + ".out";
    if (std::ifstream in{infile})
    {
    unsigned x;
    for (unsigned i = 0; i < t.size; i++)
    { // read number, ignore comma, repeat
      (in >> x);
      in.ignore(); // note, last char needs to be comma
      t.data[i] = x;
    }
    }
    else
    { /*failed to open file */
    }
    if (std::ifstream in{outfile})
    {
    unsigned x;
    for (unsigned i = 0; i < t.size; i++)
    {
      (in >> x);
      in.ignore();
      t.t_data[i] = x;
    }
    }
    else
    { /*failed to open file */
    }
}

void register_testcase(Testcase t)
{
  testcases.push_back(t);
}

/* Transform Functions */

void register_transforms()
{
    Transform t;

    /*t.f = &naive_ntt_baseline;
    t.name = "naive_ntt_baseline";
    t.init_f = &dummy_init;
    t.requiresInit = false;
    //transforms.push_back(t);

    t.f = &CT_radix2_recursive;
    t.name = "CT_radix2_recursive";
    t.init_f = &dummy_init;
    t.requiresInit = false;
    transforms.push_back(t);

    t.f = &CT_radix2_iterative;
    t.name = "CT_radix2_iterative";
    t.init_f = &dummy_init;
    t.requiresInit = false;
    transforms.push_back(t);*/

    /*t.f = &NTT_radix2_iterative;
    t.name = "Radix 2 - Iterative - Scalar base";
    t.init_f = &init_r2_iter_opt;
    t.requiresInit = true;
    t.reorderInput = false;
    transforms.push_back(t);

    t.f = &NTT_radix2_iterative_shifts;
    t.name = "Radix 2 - Iterative - Scalar shifts";
    t.init_f = &init_r2_iter_opt;
    t.requiresInit = true;
    t.reorderInput = false;
    transforms.push_back(t);

    t.f = &NTT_radix2_iterative_mod2;
    t.name = "Radix 2 - Iterative - Scalar mod2";
    t.init_f = &init_r2_iter_opt;
    t.requiresInit = true;
    t.reorderInput = false;
    transforms.push_back(t);

    t.f = &NTT_radix2_iterative_loop_unrolling;
    t.name = "Radix 2 - Iterative - Scalar loop unrolling";
    t.init_f = &init_r2_iter_opt;
    t.requiresInit = true;
    t.reorderInput = false;
    transforms.push_back(t);

    t.f = &NTT_radix2_iterative_max_scalar;
    t.name = "Radix 2 - Iterative - Scalar top performance";
    t.init_f = &init_r2_iter_opt;
    t.requiresInit = true;
    t.reorderInput = false;
    transforms.push_back(t);

    t.f = &avx_radix2_iterative_base;
    t.name = "avx0_radix2_iter_opt";
    t.init_f = &init_r2_iter_opt;
    t.requiresInit = true;
    transforms.push_back(t);

    t.f = &avx_radix2_iterative_base;
    t.name = "radix2_iterative_avx_base";
    t.init_f = &init_r2_iter_opt;
    t.requiresInit = true;
    t.reorderInput = false;
    transforms.push_back(t);

    t.f = &avx_radix2_iterative_mod2;
    t.name = "radix2_iterative_avx_mod2";
    t.init_f = &init_r2_iter_opt;
    t.requiresInit = true;
    t.reorderInput = false;
    transforms.push_back(t);

    t.f = &avx_radix2_iterative_set_statements;
    t.name = "radix2_iterative_avx_set_statements";
    t.init_f = &init_r2_iter_opt;
    t.requiresInit = true;
    t.reorderInput = false;
    transforms.push_back(t);

    t.f = &avx_radix2_iterative_loop_unrolling;
    t.name = "radix2_iterative_avx_loop_unrolling";
    t.init_f = &init_r2_iter_opt;
    t.requiresInit = true;
    t.reorderInput = false;
    transforms.push_back(t);

    t.f = &avx_radix2_iterative_loop_unrolling_mod;
    t.name = "radix2_iterative_avx_loop_unrolling_mod";
    t.init_f = &init_r2_iter_opt;
    t.requiresInit = true;
    t.reorderInput = false;
    transforms.push_back(t);

    t.f = &fntt_wrapper;
    t.name = "FNTT";
    t.init_f = &dummy_init;
    t.requiresInit = false;
    transforms.push_back(t);

    t.f = &NTT_radix4_base;
    t.name = "radix4_recursive";
    t.init_f = &init_r4_NTT;
    t.requiresInit = true;
    transforms.push_back(t);

    t.f = &scalar0_NTT_radix4_base;
    t.name = "radix4_recursive_scalar0";
    t.init_f = &init_r4_NTT;
    t.requiresInit = true;
    transforms.push_back(t);

    t.f = &scalar1_NTT_radix4_base;
    t.name = "radix4_recursive_scalar1";
    t.init_f = &init_r4_NTT;
    t.requiresInit = true;
    transforms.push_back(t);

    t.f = &scalar2_NTT_radix4_base;
    t.name = "radix4_recursive_scalar2";
    t.init_f = &init_r4_NTT;
    t.requiresInit = true;
    transforms.push_back(t);

    t.f = &avx0_NTT_radix4_base;
    t.name = "radix4_recursive_avx0";
    t.init_f = &init_r4_NTT;
    t.requiresInit = true;
    t.reorderInput = false;
    transforms.push_back(t);

    t.f = &avx_NTT_radix4_sawtooth;
    t.name = "radix4_recursive_avx_sawtooth";
    t.init_f = &init_r4_NTT;
    t.requiresInit = true;
    t.reorderInput = false;
    transforms.push_back(t);

    t.f = &avx2_NTT_radix4_base;
    t.name = "radix4_recursive_avx2";
    t.init_f = &init_r4_NTT;
    t.requiresInit = true;
    t.reorderInput = true;
    transforms.push_back(t);

    t.f = &avx3_NTT_radix4_base;
    t.name = "radix4_recursive_avx3";
    t.init_f = &init_r4_NTT;
    t.requiresInit = true;
    t.reorderInput = false;
    transforms.push_back(t);

    t.f = &avx4_NTT_radix4_base;
    t.name = "radix4_recursive_avx4";
    t.init_f = &init_r4_NTT;
    t.requiresInit = true;
    t.reorderInput = true;
    transforms.push_back(t);

    t.f = &avx5_NTT_radix4_base;
    t.name = "radix4_recursive_avx5";
    t.init_f = &init_r4_NTT;
    t.requiresInit = true;
    t.reorderInput = false;
    transforms.push_back(t);

    t.f = &avx6_NTT_radix4_base;
    t.name = "radix4_recursive_avx6";
    t.init_f = &init_r4_NTT;
    t.requiresInit = true;
    t.reorderInput = false;
    transforms.push_back(t);

    t.f = &avx7_NTT_radix4_base;
    t.name = "radix4_recursive_avx7";
    t.init_f = &init_r4_NTT;
    t.requiresInit = true;
    t.reorderInput = true;
    transforms.push_back(t);

    t.f = &avx8_NTT_radix4_base;
    t.name = "radix4_recursive_avx8";
    t.init_f = &init_r4_NTT;
    t.requiresInit = true;
    t.reorderInput = true;
    transforms.push_back(t);*/

    t.f = &avx_NTT_radix4_all;
    t.name = "best r4 avx";
    t.init_f = &init_r4_NTT;
    t.requiresInit = true;
    t.reorderInput = false;
    transforms.push_back(t);

    t.f = &avx_NTT_radix4_fma;
    t.name = "best r4 avx with fma";
    t.init_f = &init_r4_NTT;
    t.requiresInit = true;
    t.reorderInput = false;
    transforms.push_back(t);
}

/* Testcases */

/*
void add_nayuki_testcases(){
  Testcase t;   // length 5 -> not for radix

  t.modulus = 11; t.n_th_root = 3; t.size = 5;
  t.data = new unsigned short int[t.size] {6,0,10,7,2};
  t.t_data = new unsigned short int[t.size] {3,7,0,5,4};
  t.name = "nayuki1";
  register_testcase(t);
}*/

void add_random_testcases()
{
    Testcase t;

    t.modulus = 97;
    t.primitive_root = 5;
    t.n_th_root = 22;
    t.size = 4;
    t.data = new unsigned[t.size]{60, 31, 89, 12};
    t.t_data = new unsigned[t.size]{95, 1, 9, 38};
    t.name = "random (size 4)";
    register_testcase(t);

    t.modulus = 673;
    t.primitive_root = 5;
    t.n_th_root = 609;
    t.size = 8;
    t.data = new unsigned[t.size]{4, 1, 4, 2, 1, 3, 5, 6};
    t.t_data = new unsigned[t.size]{26, 115, 437, 338, 2, 448, 228, 457};
    t.name = "random (size 8)";
    register_testcase(t);

    t.modulus = 769;
    t.primitive_root = 11;
    t.n_th_root = 136;
    t.size = 16;
    t.data = new unsigned[t.size]{97, 3, 34, 80, 22, 59, 74, 96, 93, 1, 52, 97, 36, 94, 62, 88};
    t.t_data = new unsigned[t.size]{219, 559, 627, 329, 451, 731, 245, 129, 721, 425, 358, 761, 370, 212, 67, 731};
    t.name = "random (size 16)";
    register_testcase(t);
}

void add_large_prime_testcases()
{
    Testcase t;

    t.modulus = 40961;
    t.primitive_root = 3;
    t.n_th_root = 8603;
    t.size = 512;
    t.data = new unsigned[t.size];
    t.t_data = new unsigned[t.size];
    read_from_files("validation/large_testcases/40961_8603_512", t);
    t.name = "random (large prime=40961), size 512";
    register_testcase(t);

    t.modulus = 40961;
    t.primitive_root = 3;
    t.n_th_root = 243;
    t.size = 8192;
    t.data = new unsigned[t.size];
    t.t_data = new unsigned[t.size];
    read_from_files("validation/large_testcases/40961_243_8192", t);
    t.name = "random (large prime=40961), size 8192";
    register_testcase(t);
}

void add_huge_testcases()
{
    Testcase t;
    t.modulus = 4253024257;
    t.primitive_root = 5;
    t.n_th_root = 399187831;
    t.size = 512;
    t.data = new unsigned[t.size];
    t.t_data = new unsigned[t.size];
    read_from_files("validation/huge_testcases/4253024257_5_399187831_512", t);
    t.name = "random (huge prime), size 512";
    register_testcase(t);

    t.modulus = 4253024257;
    t.primitive_root = 5;
    t.n_th_root = 3413964419;
    t.size = 4096;
    t.data = new unsigned[t.size];
    t.t_data = new unsigned[t.size];
    read_from_files("validation/huge_testcases/4253024257_5_3413964419_4096", t);
    t.name = "random (huge prime), size 4096";
    register_testcase(t);
}

void add_medium_testcases()
{             // prime < sqrt(2^52) -> should therefore not overflow a double multiplication
    Testcase t; // this however only allows input vector lengths up to 2^21
    t.modulus = 23068673;
    t.primitive_root = 3;
    t.n_th_root = 6321789;
    t.size = 512;
    t.data = new unsigned[t.size];
    t.t_data = new unsigned[t.size];
    read_from_files("validation/medium_testcases/23068673_3_6321789_512", t);
    t.name = "random (medium prime), size 512";
    register_testcase(t);

    t.modulus = 23068673;
    t.primitive_root = 3;
    t.n_th_root = 15161547;
    t.size = 4096;
    t.data = new unsigned[t.size];
    t.t_data = new unsigned[t.size];
    read_from_files("validation/medium_testcases/23068673_3_15161547_4096", t);
    t.name = "random (medium prime), size 4096";
    register_testcase(t);
}

void add_edgecases()
{
    Testcase t;

    t.modulus = 193;
    t.primitive_root = 5;
    t.n_th_root = 112;
    t.size = 4;
    t.data = new unsigned[t.size]{0, 0, 0, 0};
    t.t_data = new unsigned[t.size]{0, 0, 0, 0};
    t.name = "edge_zeros";
    register_testcase(t);

    t.modulus = 97;
    t.primitive_root = 5;
    t.n_th_root = 22;
    t.size = 4;
    t.data = new unsigned[t.size]{1, 1, 1, 1};
    t.t_data = new unsigned[t.size]{4, 0, 0, 0};
    t.name = "edge_ones";
    register_testcase(t);
}

void test_transform(Testcase t, Transform f)
{
    cout << "----------------------" << endl;

    unsigned *expected = t.t_data;
    unsigned *actual = (unsigned *)malloc(t.size * sizeof(unsigned));
    unsigned *input = (unsigned *)malloc(t.size * sizeof(unsigned));

    for (unsigned i = 0; i < t.size; i++)
    {
        input[i] = t.data[i];
    }

    if (f.requiresInit)
    {
        f.init_f(t.size, t.n_th_root, t.modulus);
    }

    if (f.reorderInput and t.size >= 16)
    {
        unsigned *Z = (unsigned *)malloc(sizeof(unsigned) * t.size);
        if (isPower4(t.size))
        {
            int s = t.size / 16;
            for (int i = 0; i < s; i++)
            {
                for (int j = 0; j < 16; j++)
                {
                    Z[16 * i + j] = input[i + j * s];
                }
            }
        }
        else
        {
            ull N2 = t.size >> 1;
            int s = N2 / 16;
            unsigned *Z_odd = (unsigned *)malloc(sizeof(unsigned) * N2);
            unsigned *Z_even = (unsigned *)malloc(sizeof(unsigned) * N2);

            for (int i = 0; i < s; i++)
            {
                for (int j = 0; j < 16; j++)
                {
                    Z_even[16 * i + j] = input[2 * (i + j * s)];
                    Z_odd[16 * i + j] = input[2 * (i + j * s) + 1];
                }
            }
            for (int i = 0; i < N2; i++)
            {
                Z[i] = Z_even[i];
                Z[i + N2] = Z_odd[i];
            }
            free(Z_even);
            free(Z_odd);
        }

        for (int i = 0; i < t.size; i++)
        {
            input[i] = Z[i];
        }
        free(Z);
    }

    if ((f.name == "FNTT"))
    {
        f.f(actual, input, t.size, t.primitive_root, t.modulus); // for FNTT
    }
    else
    {
        f.f(actual, input, t.size, t.n_th_root, t.modulus); // normal calls
    }

    for (unsigned i = 0; i < t.size; i++)
    {

    if (expected[i] != actual[i])
    {
        cout << _red_ << "FAILED " << _black_ << f.name + " || " + t.name << endl;
        cout << "   expected:  ";
        for (unsigned j = 0; j < std::min(t.size, (ull)10); j++)
        {
        cout << expected[j] << " ";
        }
        cout << " ..." << endl;
        cout << "        got:  ";
        for (unsigned j = 0; j < std::min(t.size, (ull)10); j++)
        {
        cout << actual[j] << " ";
        }
        cout << " ..." << endl;
        return;
        }
    }
    cout << _green_ << "PASSED " << _black_ << f.name + " || " + t.name << endl;
}

int main(void)
{

    //add_nayuki_testcases();
    add_random_testcases();
    add_edgecases();
    add_large_prime_testcases();
    // add_huge_testcases();
    add_medium_testcases();

    register_transforms();

    for (auto trans : transforms)
    {
        for (auto testcase : testcases)
        {
            test_transform(testcase, trans);
        }
    }
}