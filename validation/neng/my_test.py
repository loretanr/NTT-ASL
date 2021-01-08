import numpy as np
from NTT import NTT

ntt = NTT()
M = 11
poly = [2, 7, 10, 0, 6]     # 4x^3+3x^2+2x+1
print("Modulus : %d" % M)
print("Polynomial : ", poly)
N = len(poly)
w = ntt.NthRootOfUnity(M, N)
w = 3
ntt_poly = ntt.ntt(poly, M, N, w)
print("ntt_poly " + str(ntt_poly))
intt_poly = ntt.intt(ntt_poly, M, N, w)
print("Polynomial degree : %d" % (N - 1))
print("Primitive %dth root of unity : %d" % (N, w))
print("NTT(poly) = ", ntt_poly)
#print("Inverse NTT : ", intt_poly)
