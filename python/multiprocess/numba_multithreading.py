from __future__ import print_function, division, absolute_import

import math
import threading
from timeit import repeat

import numpy as np
from numba import jit

nthreads = 4
size = 10**6

def func_np(a, b):
    return np.exp(2.1 * a + 3.2 * b)


@jit('void(double[:], double[:], double[:])', nopython=True, nogil=True)
def inner_func_nb(result, a, b):
    for i in range(len(result)):
        result[i] = math.exp(2.1 * a[i] + 3.2 * b[i])

def timefunc(correct, s, func, *args, **kwargs):
    """
    benchmark *func* and print out its runtime.
    """
    print(s.ljust(20), end=" ")
    # make sure the function is compiled before we start the benchmark
    res = func(*args, **kwargs)
    if correct is not None:
        assert np.allclose(res, correct), (res,correct)
    #time it
    print("{:>5.0f} ms".format(min(repeat(lambda: func(*args, **kwargs), number=5, repeat=2)) * 1000))

    return res


def make_singlethread(inner_func):
    def func(*args):
        length = len(args[0])
        result = np.empty(length, dtype=np.float64)
        inner_func(result, *args)
        return result
    return func


def make_multithread(inner_func, numthreads):
    def func_mt(*args):
        length = len(args[0])
        result = np.empty(length, dtype=np.float64)
        args = (result,) + args
        chunklen = (length + numthreads - 1) // numthreads
        # create argument tuples for each input thunk
        chunks = [[arg[i * chunklen:(i+1) * chunklen] for arg in args]
                  for i in range(numthreads)]

        threads = [threading.Thread(target=inner_func, args=chunk) for chunk in chunks]

        for t in threads:
            t.start()

        for t in threads:
            t.join()

        return result

    return func_mt



func_nb = make_singlethread(inner_func_nb)
func_nb_mt = make_multithread(inner_func_nb, nthreads)


a = np.random.rand(size)
b = np.random.rand(size)

correct = timefunc(None, "numpy (1 thread)", func_np, a, b)
timefunc(correct, "numba (1 thread)", func_nb, a, b)
timefunc(correct, "numba (%d threads)" % nthreads, func_nb_mt, a, b)