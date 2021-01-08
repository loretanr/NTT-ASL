#ifndef PROJECT_TRANSFORM_H
#define PROJECT_TRANSFORM_H

void NTT_radix2_iterative(unsigned *outvec, unsigned *invec, ull n, ull root, ull mod);
void CT_radix2_recursive(unsigned *outvec, unsigned *invec, ull n, ull root, ull mod);
void init_r2_iter_opt(ull n, ull root, ull mod);
int formula_cost_radix2_iter(int n);
int real_cost_radix2_iter(int n);

#endif //PROJECT_TRANSFORM_H
