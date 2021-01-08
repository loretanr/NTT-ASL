#include <list>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <papi.h>

#include <chrono>
#include <iterator>

#include "tsc_x86.h"

#include "../implementation/FTL/nttw/include/number32.h"

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

#include "../implementation/avx/radix2/ntt-radix2-avx0.h"
#include "../implementation/avx/radix2/ntt-radix2-avx1.h"
#include "../implementation/avx/radix2/ntt-radix2-avx2.h"
#include "../implementation/avx/radix2/ntt-radix2-avx3.h"
#include "../implementation/avx/radix2/ntt-radix2-avx4.h"

#include "../implementation/avx/radix4/ntt-radix4-avx0.h"
#include "../implementation/avx/radix4/ntt-radix4-avx-all.h"
#include "../implementation/avx/radix4/ntt-radix4-avx2.h"
#include "../implementation/avx/radix4/ntt-radix4-avx3.h"
#include "../implementation/avx/radix4/ntt-radix4-avx4.h"
#include "../implementation/avx/radix4/ntt-radix4-avx5.h"
#include "../implementation/avx/radix4/ntt-radix4-avx6.h"
#include "../implementation/avx/radix4/ntt-radix4-avx7.h"
#include "../implementation/avx/radix4/ntt-radix4-avx8.h"

#define REP 20

using namespace std;

int L3_SIZE = 10; // For cache flush, set to >= your L3 size in MB

int UPPER_LIMIT = 19; // chose max input vector size to bench here
                      // [0 -> 2^2 ... 19 -> 2^21]

ull n_s[20] = {4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288, 1048576, 2097152};
ull roots[20] = {21304244, 5048532, 3028610, 20823550, 9826950, 21648390, 16583093, 6321789, 4964576, 9315417, 15161547, 21201314, 4751760, 13263868, 10739434, 10108677, 76160, 8505079, 7664329, 177147};
ull mods[20] = {23068673, 23068673, 23068673, 23068673, 23068673, 23068673, 23068673, 23068673, 23068673, 23068673, 23068673, 23068673, 23068673, 23068673, 23068673, 23068673, 23068673, 23068673, 23068673, 23068673};

struct Testcase
{
    transform_func bm_f;
    bool requiresInit;
    init_func init_f;
    cost_func cost_f;
    bool reorderInput;
    int plotIndex;
};

std::vector<Testcase> benchmarkedFunctions;
std::vector<std::string> functionsNames;

void addFunction(transform_func bm_f, init_func init_f, cost_func cost_f, bool requires_init, bool reorderInput, std::string name, int plotIndex)
{
    Testcase t;
    t.bm_f = bm_f;
    t.cost_f = cost_f;
    t.plotIndex = plotIndex;
    t.reorderInput = reorderInput;
    t.requiresInit = false;
    if (requires_init)
    {
        t.requiresInit = true;
        t.init_f = init_f;
    }
    benchmarkedFunctions.push_back(t);
    functionsNames.push_back(name);
}

char *cache; // dummy global to make sure cache flush not optimized away
void cache_flush()
{
    const int flush_size = 1024 * 1024 * L3_SIZE;
    cache = (char *)malloc(flush_size * sizeof(char));
    for (int i = 0; i < 0xf; i++)
        for (int j = 0; j < flush_size; j++)
            cache[j] = i + j; // write some stuff
    for (int i = 0; i < 0x15; i++)
        for (int j = 0; j < flush_size; j++)
            cache[j] = i + j; // once again
}

void handle_error(int err){
    std::cerr << "PAPI error: " << err << std::endl;
}

void performanceTest(Testcase t, unsigned *vec, ull n, ull root, ull mod, double &cyclesCount, double &memoryCount)
{
    double cycles;
    double num_runs = 5.0;
    double multiplier = 1;
    unsigned long long start, end;

    if (t.requiresInit)
    {
        t.init_f(n, root, mod);
    }
    if (t.reorderInput and n >= 16)
    {
        unsigned *Z = (unsigned *)malloc(sizeof(unsigned) * n);
        if (isPower4(n))
        {
            int s = n / 16;
            for (int i = 0; i < s; i++)
            {
                for (int j = 0; j < 16; j++)
                {
                    Z[16 * i + j] = vec[i + j * s];
                }
            }
        }
        else
        {
            ull N2 = n >> 1;
            int s = N2 / 16;
            unsigned *Z_odd = (unsigned *)malloc(sizeof(unsigned) * N2);
            unsigned *Z_even = (unsigned *)malloc(sizeof(unsigned) * N2);

            for (int i = 0; i < s; i++)
            {
                for (int j = 0; j < 16; j++)
                {
                    Z_even[16 * i + j] = vec[2 * (i + j * s)];
                    Z_odd[16 * i + j] = vec[2 * (i + j * s) + 1];
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

        for (int i = 0; i < n; i++)
        {
            vec[i] = Z[i];
        }
        free(Z);
    }

    std::list<double> cyclesList;

    // ======== Cold Cache Performance Measurement ========
    for (size_t j = 0; j < REP; j++)
    {
        cache_flush();

        start = start_tsc();

        t.bm_f(vec, vec, n, root, mod);

        end = stop_tsc(start);

        cycles = ((double)end);

        cyclesList.push_back(cycles);
    }

    cyclesList.sort();
    cycles = cyclesList.front();
    cyclesCount = 1.0 * cycles;

    // ======== L3 cache misses count with PAPI ========
    const int numEvents = 1;

    long long end_values[numEvents] = {0};
    long long values[numEvents];
    int events[numEvents] = {PAPI_L3_LDM};

    for (int i = 0; i < REP; i++)
    {
        cache_flush();
        int error_code = PAPI_start_counters(events, numEvents);
        if (error_code != PAPI_OK)
            handle_error(error_code);

        t.bm_f(vec, vec, n, root, mod);
        error_code = PAPI_stop_counters(values, numEvents);
        if (error_code != PAPI_OK)
            handle_error(1);

        free(cache); // free

        end_values[0] += values[0];
    }
    end_values[0] /= REP;

    memoryCount = end_values[0];
}

int main(int argc, char *argv[])
{
    std::stringstream args;
    std::copy(argv + 1, argv + argc, std::ostream_iterator<const char *>(args));
    cout << "== Launching benchmark suite ==" << endl;
    cout << "Compiled using flag " << args.str() << " COLD CACHE!" << endl;

    // the ones in Roofline Plot
    addFunction(fntt_wrapper, dummy_init, formula_cost_radix4_rec, false, false, "FTL", 0);
    addFunction(scalar0_NTT_radix4_base, init_r4_NTT, formula_cost_radix4_rec, true, false, "Best Scalar", 0);
    addFunction(avx_NTT_radix4_all, init_r4_NTT, formula_cost_radix4_rec, true, false, "Best AVX", 0);

    // Other NTT versions
    //addFunction(NTT_radix4_base, init_r4_NTT, formula_cost_radix4_rec, true, false, "r4 scalar base", 0);
    //addFunction(scalar1_NTT_radix4_base, init_r4_NTT, formula_cost_radix4_rec, true, false, "shift instead of div", 0);
    //addFunction(scalar2_NTT_radix4_base, init_r4_NTT, formula_cost_radix4_rec, true, false, "mod_2 instead of mod", 0);
    //addFunction(NTT_radix4_base, init_r4_NTT, formula_cost_radix4_rec, true, false, "radix4_scalar_base", 1);
    //addFunction(avx6_NTT_radix4_base, init_r4_NTT, formula_cost_radix4_rec, true, false, "AVX_base double", 1);
    //addFunction(avx5_NTT_radix4_base, init_r4_NTT, formula_cost_radix4_rec, true, false, "AVX_base int", 1);
    //addFunction(avx8_NTT_radix4_base, init_r4_NTT, formula_cost_radix4_rec, true, false, "AVX_base base & twiddle (int)", 1);
    //addFunction(avx2_NTT_radix4_base, init_r4_NTT, formula_cost_radix4_rec, true, true, "radix4_avx_2", 1);
    //addFunction(avx3_NTT_radix4_base, init_r4_NTT, formula_cost_radix4_rec, true, false, "radix4_avx_3", 1);
    //addFunction(avx7_NTT_radix4_base, init_r4_NTT, formula_cost_radix4_rec, true, true, "2er loop without mod_2", 1);
    //addFunction(avx4_NTT_radix4_base, init_r4_NTT, formula_cost_radix4_rec, true, true, "r4 avx all", 1);

    // Plot 2: Radix2 Scalar
    //addFunction(fntt_wrapper, dummy_init, formula_cost_radix4_rec, false, false, "FTL", 2);
    //addFunction(NTT_radix2_iterative, init_r2_iter_opt, formula_cost_radix2_iter, true, false, "Scalar - Base", 2);
    //addFunction(NTT_radix2_iterative_shifts, init_r2_iter_opt, formula_cost_radix2_iter, true, false, "Scalar - Shifts", 2);
    //addFunction(NTT_radix2_iterative_mod2, init_r2_iter_opt, formula_cost_radix2_iter, true, false, "Scalar - Mod", 2);
    //addFunction(NTT_radix2_iterative_loop_unrolling, init_r2_iter_opt, formula_cost_radix2_iter, true, false, "Scalar - Loop Unrolling", 2);
    //addFunction(NTT_radix2_iterative_max_scalar, init_r2_iter_opt, formula_cost_radix2_iter, true, false, "Scalar - Top Performance", 2);

    // Plot 3: Radix2 AVX
    //addFunction(NTT_radix2_iterative_max_scalar, init_r2_iter_opt, formula_cost_radix2_iter, true, false, "Scalar - Top Performance", 3);
    //addFunction(avx_radix2_iterative_base, init_r2_iter_opt, formula_cost_radix2_iter, true, false, "Vectorised - Base", 3);
    //addFunction(avx_radix2_iterative_mod2, init_r2_iter_opt, formula_cost_radix2_iter, true, false, "Vectorised - Mod", 3);
    //addFunction(avx_radix2_iterative_set_statements, init_r2_iter_opt, formula_cost_radix2_iter, true, false, "Vectorised - Set statements", 3);
    //addFunction(avx_radix2_iterative_loop_unrolling, init_r2_iter_opt, formula_cost_radix2_iter, true, false, "Vectorised - Loop unrolling", 3);
    //addFunction(avx_radix2_iterative_loop_unrolling_mod, init_r2_iter_opt, formula_cost_radix2_iter, true, false, "Vectorised - Loop unrolling + Mod", 3);


    std::stringstream fmt;
    fmt << "roofline_results/roofline.csv";

    std::ofstream o(fmt.str());
    o << "function,n,intops,intensity" << std::endl;
    std::ofstream csvFile;
    csvFile.open(fmt.str(), ios::app);

    for (int i = 0; i < benchmarkedFunctions.size(); i++)
    {
        std::ofstream csvFile;
        csvFile.open(fmt.str(), ios::app);
    }

    if (csvFile.is_open())
    {
        for (int i = 3; i <= UPPER_LIMIT; i++) // start from 32
        {

            ull _root = roots[i];
            ull _mod = mods[i];
            ull _n = n_s[i];
            auto *vec = (unsigned *)malloc(_n * sizeof(unsigned));

            string rootString = to_string(_root);
            string modString = to_string(_mod);
            string nString = to_string(_n);

            cout << "- size 2^" << i + 2 << " -------------------" << endl;

            string fileName = string("validation/medium_testcases/").append(modString).append("_").append("3").append("_").append(rootString).append("_").append(nString).append(".in");
            readFromFiles(fileName, vec);

            // double cyclesCount;
            for (int j = 0; j < benchmarkedFunctions.size(); j++)
            {
                double cyclesCount, memoryCount;
                performanceTest(benchmarkedFunctions[j], vec, _n, _root, _mod, cyclesCount, memoryCount);

                // print cycles count & print timing in ms based on frequency of processor
                int operations = benchmarkedFunctions[j].cost_f(_n);
                double intops = operations / cyclesCount;

                memoryCount *= 64; // misses * cache line size, [bytes]

                // both vertors have to be loaded at least once. If the PAPI result is lower
                // we take the 2*n instead. This should be a solid lower bound.
                memoryCount = max(memoryCount, (double) 2 * _n * 4);
                if(memoryCount==(double)2 * _n * 4){cout << "adjusted" << endl;}

                double intensity = operations/memoryCount; 
                
                cout << functionsNames[j] << " - Perf (intops): " << intops << " L3 miss: " << memoryCount << " I=intops/movement: " << intensity << endl;
                csvFile << functionsNames[j] << "," << _n << "," << intops << "," << intensity << endl;
            }
        }
        csvFile.close();
    }
    else
    {
        cout << fmt.str() << " failed to open" << endl;
    }

    return 0;
}
