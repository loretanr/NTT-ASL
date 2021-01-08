#include <list>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

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
#include "../implementation/scalar/radix4/ntt-radix4-scalar-with-init.h"

#include "../implementation/avx/radix2/ntt-radix2-avx0.h"
#include "../implementation/avx/radix2/ntt-radix2-avx1.h"
#include "../implementation/avx/radix2/ntt-radix2-avx2.h"
#include "../implementation/avx/radix2/ntt-radix2-avx3.h"
#include "../implementation/avx/radix2/ntt-radix2-avx4.h"

#include "../implementation/avx/radix4/ntt-radix4-avx0.h"
#include "../implementation/avx/radix4/ntt-radix4-avx2.h"
#include "../implementation/avx/radix4/ntt-radix4-avx3.h"
#include "../implementation/avx/radix4/ntt-radix4-avx4.h"
#include "../implementation/avx/radix4/ntt-radix4-avx5.h"
#include "../implementation/avx/radix4/ntt-radix4-avx6.h"
#include "../implementation/avx/radix4/ntt-radix4-avx7.h"
#include "../implementation/avx/radix4/ntt-radix4-avx8.h"

#include "../implementation/avx/radix4/ntt-radix4-avx-unopt.h"
#include "../implementation/avx/radix4/ntt-radix4-avx-sawtooth.h"
#include "../implementation/avx/radix4/ntt-radix4-avx-mod.h"
#include "../implementation/avx/radix4/ntt-radix4-avx-all.h"

#define CYCLES_REQUIRED 1e8
#define REP 20
#define FREQ 3.6 // for timing, change to frequency of own processor

using namespace std;

int UPPER_LIMIT = 16; // chose max input vector size to bench here
                      // [0 -> 2^2 ... 19 -> 2^21]

ull n_s[20] = {4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288, 1048576, 2097152};
ull roots[20] = {21304244, 5048532, 3028610, 20823550, 9826950, 21648390, 16583093, 6321789, 4964576, 9315417, 15161547, 21201314, 4751760, 13263868, 10739434, 10108677, 76160, 8505079, 7664329, 177147};
ull mods[20] = {23068673, 23068673, 23068673, 23068673, 23068673, 23068673, 23068673, 23068673, 23068673, 23068673, 23068673, 23068673, 23068673, 23068673, 23068673, 23068673, 23068673, 23068673, 23068673, 23068673};

/*ull n_s[18] = {128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288, 1048576, 2097152, 4194304, 8388608, 16777216};
ull roots[18] = {2934558033, 2296140600, 399187831, 3467198049, 1466974116, 3413964419, 2659557663,
                 3078059304, 1672278522, 1473534360, 2224220640, 558006320, 470402314, 1743728224, 2520933051, 3662831914, 4114580606, 538601489};
ull mods[18] = {4253024257, 4253024257, 4253024257, 4253024257, 4253024257, 4253024257, 4253024257,
                4253024257, 4253024257, 4253024257, 4253024257, 4253024257, 4253024257, 4253024257, 4253024257, 4253024257, 4253024257, 3221225473};*/

struct Testcase
{
    transform_func bm_f;
    bool requiresInit;
    init_func init_f;
    cost_func cost_f;
    bool reorderInput;
    int plotIndex;
    std::string graphTitle;
};

std::vector<Testcase> benchmarkedFunctions;
std::vector<std::string> functionsNames;

void addFunction(transform_func bm_f, init_func init_f, cost_func cost_f, bool requires_init, bool reorderInput, std::string name, int plotIndex, std::string graphTitle)
{
    Testcase t;
    t.bm_f = bm_f;
    t.cost_f = cost_f;
    t.plotIndex = plotIndex;
    t.reorderInput = reorderInput;
    t.graphTitle = graphTitle;

    if (requires_init)
    {
        t.requiresInit = true;
        t.init_f = init_f;
    }
    else
    {
        t.requiresInit = false;
    }

    benchmarkedFunctions.push_back(t);
    functionsNames.push_back(name);
}

double performanceTest(Testcase t, unsigned *vec, ull n, ull root, ull mod)
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

    // Warm-up
    do
    {
        num_runs = num_runs * multiplier;
        start = start_tsc();
        for (size_t i = 0; i < num_runs; i++)
        {
            t.bm_f(vec, vec, n, root, mod);
        }
        end = stop_tsc(start);

        cycles = (double)end;
        multiplier = (CYCLES_REQUIRED) / (cycles);

    } while (multiplier > 2);

    std::list<double> cyclesList;

    // Actual performance measurements repeated REP times.
    for (size_t j = 0; j < REP; j++)
    {

        start = start_tsc();
        for (size_t i = 0; i < num_runs; ++i)
        {
            t.bm_f(vec, vec, n, root, mod);
        }
        end = stop_tsc(start);

        cycles = ((double)end) / num_runs;

        cyclesList.push_back(cycles);
    }

    cyclesList.sort();
    cycles = cyclesList.front();
    return 1.0 * cycles;
}

int main(int argc, char *argv[])
{
    // std::cout << std::format("Hello {}!\n", "world");
    std::stringstream args;
    std::copy(argv + 1, argv + argc, std::ostream_iterator<const char *>(args));
    cout << "== Launching benchmark suite ==" << endl;
    cout << "Compiled using flag " << args.str() << endl;

    std::string radix4scalar = "Radix 4 Recursive - Scalar";
    std::string radix4avx = "Radix 4 Recursive - Vectorized";
    std::string radix2scalar = "Radix 2 Iterative - Scalar";
    std::string radix2avx = "Radix 2 Iterative - Vectorized";
    std::string comparison = "Overview";
    int radix4scalarIndex = 0;
    int radix4avxIndex = 1;
    int radix2scalarIndex = 2;
    int radix2avxIndex = 3;
    int comparisonIndex = 4;

    // Plot 0: Radix4 Scalar
    //addFunction(naive_ntt_baseline, dummy_init, formula_cost_radix2_iter, false, false, "Naive matrix-vector mult", radix4scalarIndex, radix2scalar);
    //addFunction(fntt_wrapper, dummy_init, formula_cost_radix2_iter, false, false, "FTL", radix4scalarIndex, radix4scalar);
    //addFunction(scalar_NTT_radix4_base_with_init, dummy_init, formula_cost_radix4_rec, false, false, "Init included", radix4scalarIndex, radix4scalar);
    //addFunction(NTT_radix4_base, init_r4_NTT, formula_cost_radix4_rec, true, false, "Basic", radix4scalarIndex, radix4scalar);
    //addFunction(scalar1_NTT_radix4_base, init_r4_NTT, formula_cost_radix4_rec, true, false, "Shifts instead of div", radix4scalarIndex, radix4scalar);
    //addFunction(scalar2_NTT_radix4_base, init_r4_NTT, formula_cost_radix4_rec, true, false, "Floored division", radix4scalarIndex, radix4scalar);
    //addFunction(scalar0_NTT_radix4_base, init_r4_NTT, formula_cost_radix4_rec, true, false, "All optimizations", radix4scalarIndex, radix4scalar);

    // Plot 1: Radix4 AVX
    /*addFunction(scalar0_NTT_radix4_base, init_r4_NTT, formula_cost_radix4_rec, true, false, "Best Scalar", radix4avxIndex, radix4avx);
    addFunction(avx_NTT_radix4_unopt, init_r4_NTT, formula_cost_radix4_rec, true, false, "Basic", radix4avxIndex, radix4avx);
    addFunction(avx_NTT_radix4_sawtooth, init_r4_NTT, formula_cost_radix4_rec, true, false, "Sawtooth Fixed", radix4avxIndex, radix4avx);
    addFunction(avx_NTT_radix4_mod, init_r4_NTT, formula_cost_radix4_rec, true, false, "Precomp. modulo", radix4avxIndex, radix4avx);*/
    addFunction(avx_NTT_radix4_all, init_r4_NTT, formula_cost_radix4_rec, true, false, "Sawtooth + Modulo precomputation", radix4avxIndex, radix4avx);
    //addFunction(avx0_NTT_radix4_base, init_r4_NTT, formula_cost_radix4_rec, true, false, "AVX_base int", radix4avxIndex, radix4avx);
    addFunction(avx_NTT_radix4_fma, init_r4_NTT, formula_cost_radix4_rec, true, false, "same with fma", radix4avxIndex, radix4avx);
    /*addFunction(avx8_NTT_radix4_base, init_r4_NTT, formula_cost_radix4_rec, true, true, "Vectorised - Base and Twiddle combined", radix4avxIndex, radix4avx);
    addFunction(avx6_NTT_radix4_base, init_r4_NTT, formula_cost_radix4_rec, true, false, "Vectorised - partial usage of pd", radix4avxIndex, radix4avx);
    addFunction(avx2_NTT_radix4_base, init_r4_NTT, formula_cost_radix4_rec, true, true, "radix4_avx_2", radix4avxIndex, radix4avx);
    addFunction(avx3_NTT_radix4_base, init_r4_NTT, formula_cost_radix4_rec, true, false, "radix4_avx_3", radix4avxIndex, radix4avx);
    addFunction(avx7_NTT_radix4_base, init_r4_NTT, formula_cost_radix4_rec, true, true, "Vectorised - Precomp. floored div", radix4avxIndex, radix4avx);
    addFunction(avx4_NTT_radix4_base, init_r4_NTT, formula_cost_radix4_rec, true, true, "All improvements", radix4avxIndex, radix4avx);*/

    // Plot 2: Radix2 Scalar
    /*addFunction(fntt_wrapper, dummy_init, formula_cost_radix2_iter, false, false, "FTL", radix2scalarIndex, radix2scalar);
    addFunction(NTT_radix2_iterative, init_r2_iter_opt, formula_cost_radix2_iter, true, false, "Baseline Scalar", radix2scalarIndex, radix2scalar);
    addFunction(NTT_radix2_iterative_shifts, init_r2_iter_opt, formula_cost_radix2_iter, true, false, "Shifts", radix2scalarIndex, radix2scalar);
    addFunction(NTT_radix2_iterative_mod2, init_r2_iter_opt, formula_cost_radix2_iter, true, false, "Floored division", radix2scalarIndex, radix2scalar);
    addFunction(NTT_radix2_iterative_loop_unrolling, init_r2_iter_opt, formula_cost_radix2_iter, true, false, "Loop unrolling", radix2scalarIndex, radix2scalar);
    addFunction(NTT_radix2_iterative_max_scalar, init_r2_iter_opt, formula_cost_radix2_iter, true, false, "Combined", radix2scalarIndex, radix2scalar);

    // Plot 3: Radix2 AVX
    addFunction(fntt_wrapper, dummy_init, formula_cost_radix2_iter, false, false, "FTL", radix2avxIndex, radix2avx);
    addFunction(avx_radix2_iterative_base, init_r2_iter_opt, formula_cost_radix2_iter, true, false, "Baseline Vectorized", radix2avxIndex, radix2avx);
    addFunction(NTT_radix2_iterative_max_scalar, init_r2_iter_opt, formula_cost_radix2_iter, true, false, "Best Scalar", radix2avxIndex, radix2avx);
    addFunction(avx_radix2_iterative_mod2, init_r2_iter_opt, formula_cost_radix2_iter, true, false, "Floored division", radix2avxIndex, radix2avx);
    // addFunction(avx_radix2_iterative_loop_unrolling_mod, init_r2_iter_opt, formula_cost_radix2_iter, true, false, "All improvements", radix2avxIndex, radix2avx);*/

    // Plot 3: Final
    //addFunction(naive_ntt_baseline, dummy_init, formula_cost_radix2_iter, false, false, "Naive NTT", comparisonIndex, comparison);
    //addFunction(fntt_wrapper, dummy_init, formula_cost_radix2_iter, false, false, "FTL", comparisonIndex, comparison);
    //addFunction(NTT_radix2_iterative_max_scalar, init_r2_iter_opt, formula_cost_radix2_iter, true, false, "Best Radix 2 Scalar", comparisonIndex, comparison);
    //addFunction(scalar0_NTT_radix4_base, init_r4_NTT, formula_cost_radix2_iter, true, false, "Best Radix 4 Scalar", comparisonIndex, comparison);
    //addFunction(scalar0_NTT_radix4_base, init_r4_NTT, formula_cost_radix2_iter, true, false, "Best Radix 4 Scalar", comparisonIndex, comparison);
    //addFunction(avx_radix2_iterative_mod2, init_r2_iter_opt, formula_cost_radix2_iter, true, false, "Best Radix 2 Vectorized", comparisonIndex, comparison);
    //addFunction(avx4_NTT_radix4_base, init_r4_NTT, formula_cost_radix4_rec, true, true, "Best Radix 4 Vectorized", comparisonIndex, comparison);

    std::stringstream fmt;
    // fmt << "benchmark_results/benchmarks_" << args.str() << ".csv";
    fmt << "benchmark_results/benchmarks.csv";

    std::ofstream o(fmt.str());
    o << "function,flag,n,cycles,timing,intops,plot_index,graph_title" << std::endl;
    std::ofstream csvFile;
    csvFile.open(fmt.str(), ios::app);

    for (int i = 0; i < benchmarkedFunctions.size(); i++)
    {
        /*std::stringstream fmt;
        fmt << "benchmark_results/benchmarks_" << args.str() << "_function_" << functionsNames[i] << ".csv";
        std::ofstream o(fmt.str());
        o << "n,cycles,timing" << std::endl;*/

        std::ofstream csvFile;
        csvFile.open(fmt.str(), ios::app);
    }

    if (csvFile.is_open())
    {
        for (int i = 0; i <= UPPER_LIMIT; i++)
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

            double cyclesCount;
            for (int j = 0; j < benchmarkedFunctions.size(); j++)
            {
                cyclesCount = performanceTest(benchmarkedFunctions[j], vec, _n, _root, _mod);

                // print cycles count & print timing in ms based on frequency of processor
                double runtime = cyclesCount / (FREQ * 1000000);
                int operations = benchmarkedFunctions[j].cost_f(_n);
                double intops = operations / cyclesCount;

                cout << functionsNames[j] << " - Performance: " << intops << " intops/cycle - Cycles: " << cyclesCount << " - Time: " << runtime << " ms" << endl;
                csvFile << functionsNames[j] << "," << argv[1] << "," << _n << "," << cyclesCount << "," << runtime << "," << intops << "," << benchmarkedFunctions[j].plotIndex << "," << benchmarkedFunctions[j].graphTitle << endl;
                /*std::stringstream fmt;
                fmt << "benchmark_results/benchmarks_" << args.str() << "_function_" << functionsNames[j] << ".csv";

                std::ofstream csvFile_function;
                csvFile_function.open(fmt.str(), ios::app);
                csvFile_function << _n << "," << cyclesCount << "," << runtime << endl;
                csvFile_function.close();*/
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
