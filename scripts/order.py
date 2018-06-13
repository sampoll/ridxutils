import sys

""" Test substring-to-offset and offset-to-substring 

Generate all substrings for arbitrary dimensionality so they
can be checked against extraction code.

Example:
     D = [3, 5, 2, 4]    
     SR = gen_all_subscripts_rmajor(D)
     print_subscripts(SR)
     SC = gen_all_subscripts_cmajor(D)
     print_subscripts(SC)

"""

def iterate(A, D, nd, S):
  if (nd == len(A)-1):     # print a line
    S.append(list(A))
    if (A[nd] == D[nd]-1):
      A[nd] = 0
      nd = nd - 1
      iterate(A, D, nd, S)
    else:
      A[nd] = A[nd]+1
      iterate(A, D, nd, S)
  else:
    if (A[nd] == D[nd]-1):
      A[nd] = -1 
      nd = nd - 1
      if (nd < 0):
        return
      iterate(A, D, nd, S)
    else:
      A[nd] = A[nd]+1
      nd = nd + 1
      iterate(A, D, nd, S)

""" Generate all substrings in last-fastest order """
def gen_all_subscripts_rmajor(D):
     ndim = len(D)
     A = [0]*ndim
     S = []
     iterate(A, D, ndim-1, S)
     return S

""" Generate all substrings in first-fastest order """
def gen_all_subscripts_cmajor(D):
     E = list(reversed(D))
     ndim = len(E)
     A = [0]*ndim
     S = []
     iterate(A, E, ndim-1, S)

     T = []
     for s in S:
       T.append(list(reversed(s)))
     return T

def print_subscripts(S):
     k = 0
     for s in S:
       print("{:4d}  {}".format(k,s))
       k = k + 1
     return

def subscripts_for_offset_rmajor(D, off):
   m = len(D) 
   X = [-1]*m
   for j in range(m):
     if j == 0:
       s = off
       for i in range(1,m,1):
         s = s // D[i]
     elif j == m-1:
       s = off % D[m-1]
     else: 
       s = off
       for i in range(j+1,m,1):
         s = s // D[i]
       s = s % D[j]
     X[j] = s
   return X
        
def subscripts_for_offset_cmajor(D, off):
   m = len(D) 
   X = [-1]*m
   for j in range(m):
     if j == 0:
       s = off % D[0]
     elif j == m-1:
       s = off
       for i in range(0,m-1):
         s = s // D[i]
     else:
       s = off
       for i in range(0,j,1):
         s = s // D[i]
       s = s % D[j]
     X[j] = s
   return X

def offset_for_subscripts_rmajor(D, s):
  m = len(D)
  off = 0
  for j in range(m):
    p = 1
    for i in range(j+1,m,1):
      p = p * D[i]
    off = off + p*s[j]
  return off

def offset_for_subscripts_cmajor(D, s):
  m = len(D)
  off = 0
  for j in range(m):
    p = 1
    for i in range(0,j,1):
      p = p * D[i]
    off = off + p*s[j]
  return off

if __name__ == "__main__":
     D = [3, 5, 2, 4]
     # D = [2,2,2]
     SR = gen_all_subscripts_rmajor(D)
     SC = gen_all_subscripts_cmajor(D)

     # check the offset-to-subscript routines
     # and the subscript-to-offset routines
     for k in range(len(SR)):
       R = subscripts_for_offset_rmajor(D, k)
       l = offset_for_subscripts_rmajor(D, R)
       if SR[k] != R or l != k:
         print("R: {}  {}  {}  {}".format(k, R, SR[k], l))

     for k in range(len(SC)):
       C = subscripts_for_offset_cmajor(D, k)
       l = offset_for_subscripts_cmajor(D, C)
       if SC[k] != C or l != k:
         print("C: {}  {}  {}  {}".format(k, C, SC[k], l))



