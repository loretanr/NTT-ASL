import random
import math
import itertools
import numbers

# search for suitable primes
# they must be smaller than sqrt(2^52) so multiplication doesn't overflow
# p-1 must be divisible by a (as large as possible) power of 2. O/w the python ntt library doesn't work
# (this is then the max possible input vector size)

def sqrt(n):
    if n < 0:
        raise ValueError()
    i = 1
    while i * i <= n:
        i *= 2
    result = 0
    while i > 0:
        if (result + i)**2 <= n:
            result += i
        i //= 2
    return result


def is_prime(n):
    if n <= 1:
        raise ValueError()
    return all((n % i != 0) for i in range(2, sqrt(n) + 1))


curr = 2**23

while(curr < 2**26):
    if(is_prime(curr+1)):
        print(curr)
    curr += 2**23
