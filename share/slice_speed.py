#!/usr/bin/env python3

import sys

nitems = int(sys.argv[1])
niter = int(sys.argv[2])
nsliceitems = int(sys.argv[3])

range_base = nitems - nsliceitems


if True:
    v = list(range(0, 10000000))
else:
    v = []
    for i in range(0, 10000000):
        v.append(i)

print(len(v))

total = 0
for i in range(0, niter):
    sl = v[range_base:range_base + nsliceitems]
    total += sl[min(i, nsliceitems - 1)]

print(total)

