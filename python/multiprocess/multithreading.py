"""Sample code for python threading"""

from __future__ import print_function, division, absolute_import

import threading
import time
import numpy as np
from numba import jit

nthreads = 4
size = 10**6


@jit(nopython=True, nogil=True)
def worker_func(*args):
    print("worker_func ", args[0], " started")
    a = np.random.rand(size)
    b = np.random.rand(size)
    ret = np.exp(2.1 * a + 3.2 * b)
    print("worker_func ", args[0], " ended")
    return ret


def make_singlethread(inner_func):
    def func(*args):
        args = args + (0,)
        return inner_func(args)
    return func


def make_multithread(inner_func, numthreads):
    def func_mt(*args):
        newargs = [args + (i,) for i in range(numthreads)]
        threads = [threading.Thread(target=inner_func, args=na)
                   for na in newargs]

        for t in threads:
            t.start()

        for t in threads:
            t.join()

    return func_mt


func_single = make_singlethread(worker_func)
func_multi = make_multithread(worker_func, nthreads)

func_single()
func_multi()
