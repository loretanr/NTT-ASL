from sympy.ntheory.residue_ntheory import primitive_root
import random
import math
import itertools
import numbers
from sympy import ntt
from sympy import intt

# select choices
#prime = 3 * 2**5 + 1
prime = 4253024257  # prime must be of form m * 2^k + 1
maxlength = 2**23  # input array length must be power of 2, and <= 2^k
max_value = 100  # can be anything but oviously < prime


# Find smallest primitive root

pr = primitive_root(prime)


# ---------------------------------------------------
#  FUNCTIONS TO FIND n-TH ROOT   (nayuki code)
# ---------------------------------------------------

def find_modulus(veclen, minimum):
    check_int(veclen)
    check_int(minimum)
    if veclen < 1 or minimum < 1:
        raise ValueError()
    start = (minimum - 1 + veclen - 1) // veclen
    for i in itertools.count(max(start, 1)):
        n = i * veclen + 1
        assert n >= minimum
        if is_prime(n):
            return n


def find_generator(totient, mod):
    check_int(totient)
    check_int(mod)
    if not (1 <= totient < mod):
        raise ValueError()
    for i in range(1, mod):
        if is_generator(i, totient, mod):
            return i
    raise ValueError("No generator exists")


def find_root(degree, totient, mod):
    check_int(degree)
    check_int(totient)
    check_int(mod)
    if not (1 <= degree <= totient < mod):
        raise ValueError()
    if totient % degree != 0:
        raise ValueError()
    gen = find_generator(totient, mod)
    root = pow(gen, totient // degree, mod)
    assert 0 <= root < mod
    return root


def is_generator(val, totient, mod):
    check_int(val)
    check_int(totient)
    check_int(mod)
    if not (0 <= val < mod):
        raise ValueError()
    if not (1 <= totient < mod):
        raise ValueError()
    pf = unique_prime_factors(totient)
    return pow(val, totient, mod) == 1 and all((pow(val, totient // p, mod) != 1) for p in pf)


def is_primitive_root(val, degree, mod):
    check_int(val)
    check_int(degree)
    check_int(mod)
    if not (0 <= val < mod):
        raise ValueError()
    if not (1 <= degree < mod):
        raise ValueError()
    pf = unique_prime_factors(degree)
    return pow(val, degree, mod) == 1 and all((pow(val, degree // p, mod) != 1) for p in pf)


def reciprocal(n, mod):
    check_int(n)
    check_int(mod)
    if not (0 <= n < mod):
        raise ValueError()
    x, y = mod, n
    a, b = 0, 1
    while y != 0:
        a, b = b, a - x // y * b
        x, y = y, x % y
    if x == 1:
        return a % mod
    else:
        raise ValueError("Reciprocal does not exist")


def unique_prime_factors(n):
    check_int(n)
    if n < 1:
        raise ValueError()
    result = []
    i = 2
    end = sqrt(n)
    while i <= end:
        if n % i == 0:
            n //= i
            result.append(i)
            while n % i == 0:
                n //= i
            end = sqrt(n)
        i += 1
    if n > 1:
        result.append(n)
    return result


def is_prime(n):
    check_int(n)
    if n <= 1:
        raise ValueError()
    return all((n % i != 0) for i in range(2, sqrt(n) + 1))


def sqrt(n):
    check_int(n)
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


def check_int(n):
    if not isinstance(n, numbers.Integral):
        raise TypeError()

# ---------------------------------------------------
# ---------------------------------------------------


lenn = 2**7

while(lenn <= maxlength):

    for i in range(100):  # try multiple times if necessary

        seq = [random.randint(0, max_value) for i in range(lenn)]
        #seq = [1 for i in range(length)]
        #seq = [60,31,89,12]

        transformed_seq = ntt(seq, prime)
        transformed_back = intt(transformed_seq, prime)[:lenn]

        if seq == transformed_back:

            n_th = find_root(lenn, prime-1, prime)

            print("prime: {}".format(prime))
            print("smallest primitive root {}".format(pr))
            print("n-th root {}".format(n_th))
            print("lengths: {}".format(lenn))

            name = '{}_{}_{}_{}'.format(prime, pr, n_th, lenn)
            infile = open(name + '.in', "w+")
            infile.write(','.join([str(x) for x in seq]) + ',')
            infile.close()
            outfile = open(name + '.out', "w+")
        outfile.write(','.join([str(x) for x in transformed_seq]) + ',')
        outfile.close()
        break
    lenn *= 2
