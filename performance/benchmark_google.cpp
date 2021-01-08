//
// Created by rijad on 06.03.2020.
//
#include <benchmark/benchmark.h>
#include <random>
#include <limits>
#include "../implementation/ntt-naive.h"

std::random_device seed;
std::mt19937 gen(seed());

static void BM_transform(benchmark::State &state) {
    short unsigned int *invec;
    for (auto _ : state) {
//        state.PauseTiming();
        // TODO: set correct root and modulos for the tests. maybe move the root and mod to the library?
        
        // this is the best root that produces correct transforms (no overflow)
        // w/o adding a modmult function to transform.c.
        // it works for all input sizes <= 8192
        int root = 243;
        int mod = 40961;

        std::uniform_int_distribution<int> rand_int(0, mod-1);
        int vec_size = state.range(0);
        invec = new short unsigned int[vec_size];
        
//        while (state.KeepRunning()) {
        for (int i = 0; i < vec_size; i++) {
            invec[i] = rand_int(gen);
        }
//        state.ResumeTiming();
        naive_ntt_baseline(invec, vec_size, root, mod);
//        state.PauseTiming();
//        }
    }
    delete[] invec;
}

BENCHMARK(BM_transform)
->RangeMultiplier(2)->Range(1<<4, 1<<13);

BENCHMARK_MAIN();
