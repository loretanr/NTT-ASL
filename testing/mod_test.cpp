#include "tsc_x86.h"
#include <stdlib.h>
#include <stdio.h>
#include <random>

int mod = 97;
int y;
int *x;

void rands(int *m, size_t row, size_t col)
{
    std::random_device rd;
    std::mt19937 gen{rd()};
    std::uniform_real_distribution<double> dist(1, 200);
    for (size_t i = 0; i < row * col; ++i)
        m[i] = (int)dist(gen);
}

inline int mod_straightforward(int a)
{
    return a % mod;
}

inline int mod_conditional(int a)
{
    return a >= mod ? a % mod : a;
}

inline int mod_div(int a)
{
    return a - mod * (a / mod);
}

inline int mod_div_conditional(int a)
{
    return a >= mod ? (a - mod * (a / mod)) : a;
}

int main()
{
    x = static_cast<int *>(malloc(100 * sizeof(int)));
    rands(x, 1, 100);
    unsigned long long start, end;

    double cycles;
    int a = rand() % 300;
    /* Start bench */
    start = start_tsc();
    for (int i = 0; i < 100; ++i)
    {
        y = mod_straightforward(x[i]);
    }
    end = stop_tsc(start);
    cycles = ((double)end) / 100;
    printf("mod_straightforward: Cycles = % f\n", cycles);

    start = start_tsc();
    for (int i = 0; i < 100; ++i)
    {
        y = mod_conditional(x[i]);
    }
    end = stop_tsc(start);
    cycles = ((double)end) / 100;
    printf("mod_conditional: Cycles = % f\n", cycles);

    start = start_tsc();
    for (int i = 0; i < 100; ++i)
    {
        y = mod_div(x[i]);
    }
    end = stop_tsc(start);
    cycles = ((double)end) / 100;
    printf("mod_div: Cycles = % f\n", cycles);

    start = start_tsc();
    for (int i = 0; i < 100; ++i)
    {
        y = mod_div_conditional(x[i]);
    }
    end = stop_tsc(start);
    cycles = ((double)end) / 100;
    printf("mod_div_conditional: Cycles = % f\n", cycles);
}