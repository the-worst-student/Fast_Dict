import time
import tracemalloc
from fast_dict import FastDict


BIG_N = 2 ** 10
LOCALITY_REPEATS = 2000
EXTREME_REPEATS = 10000


def measure_insert(n):
    data = range(n)

    tracemalloc.start()
    start = time.perf_counter()
    fd = FastDict()
    for x in data:
        fd[x] = x
    fast_time = time.perf_counter() - start
    fast_mem = tracemalloc.get_traced_memory()[1]
    tracemalloc.stop()

    tracemalloc.start()
    start = time.perf_counter()
    d = {}
    for x in range(n):
        d[x] = x
    dict_time = time.perf_counter() - start
    dict_mem = tracemalloc.get_traced_memory()[1]
    tracemalloc.stop()

    return fast_time, dict_time, fast_mem, dict_mem


def prepare_dicts(n):
    fd = FastDict()
    d = {}
    for x in range(n):
        fd[x] = x
        d[x] = x
    return fd, d


def measure_search(n):
    fd, d = prepare_dicts(n)

    tracemalloc.start()
    start = time.perf_counter()
    for x in range(n):
        _ = fd[x]
    fast_time = time.perf_counter() - start
    fast_mem = tracemalloc.get_traced_memory()[1]
    tracemalloc.stop()

    tracemalloc.start()
    start = time.perf_counter()
    for x in range(n):
        _ = d[x]
    dict_time = time.perf_counter() - start
    dict_mem = tracemalloc.get_traced_memory()[1]
    tracemalloc.stop()

    return fast_time, dict_time, fast_mem, dict_mem


def measure_delete(n):
    fd, d = prepare_dicts(n)

    tracemalloc.start()
    start = time.perf_counter()
    for x in range(n):
        del fd[x]
    fast_time = time.perf_counter() - start
    fast_mem = tracemalloc.get_traced_memory()[1]
    tracemalloc.stop()

    tracemalloc.start()
    start = time.perf_counter()
    for x in range(n):
        del d[x]
    dict_time = time.perf_counter() - start
    dict_mem = tracemalloc.get_traced_memory()[1]
    tracemalloc.stop()

    return fast_time, dict_time, fast_mem, dict_mem


def measure_locality(n, repeats):
    fd, d = prepare_dicts(n)

    key = 0

    tracemalloc.start()
    start = time.perf_counter()
    for _ in range(repeats):
        _ = fd[key]
    fast_time = time.perf_counter() - start
    fast_mem = tracemalloc.get_traced_memory()[1]
    tracemalloc.stop()

    tracemalloc.start()
    start = time.perf_counter()
    for _ in range(repeats):
        _ = d[key]
    dict_time = time.perf_counter() - start
    dict_mem = tracemalloc.get_traced_memory()[1]
    tracemalloc.stop()

    return fast_time, dict_time, fast_mem, dict_mem


def measure_small_tree(repeats):
    fd = FastDict()
    d = {}

    for i in range(3):
        fd[i] = i
        d[i] = i

    key = 0

    tracemalloc.start()
    start = time.perf_counter()
    for _ in range(repeats):
        _ = fd[key]
    fast_time = time.perf_counter() - start
    fast_mem = tracemalloc.get_traced_memory()[1]
    tracemalloc.stop()

    tracemalloc.start()
    start = time.perf_counter()
    for _ in range(repeats):
        _ = d[key]
    dict_time = time.perf_counter() - start
    dict_mem = tracemalloc.get_traced_memory()[1]
    tracemalloc.stop()

    return fast_time, dict_time, fast_mem, dict_mem


if __name__ == "__main__":
    print(f"BIG_N = {BIG_N}")
    print()

    print("INSERT TEST")
    ft, dt, fm, dm = measure_insert(BIG_N)
    print(f"FastDict time: {ft}")
    print(f"dict time: {dt}")
    print(f"FastDict memory: {fm}")
    print(f"dict memory: {dm}")
    print()

    print("SEARCH TEST")
    ft, dt, fm, dm = measure_search(BIG_N)
    print(f"FastDict time: {ft}")
    print(f"dict time: {dt}")
    print(f"FastDict memory: {fm}")
    print(f"dict memory: {dm}")
    print()

    print("DELETE TEST")
    ft, dt, fm, dm = measure_delete(BIG_N)
    print(f"FastDict time: {ft}")
    print(f"dict time: {dt}")
    print(f"FastDict memory: {fm}")
    print(f"dict memory: {dm}")
    print()

    print("LOCALITY TEST")
    ft, dt, fm, dm = measure_locality(BIG_N, LOCALITY_REPEATS)
    print(f"FastDict time: {ft}")
    print(f"dict time: {dt}")
    print(f"FastDict memory: {fm}")
    print(f"dict memory: {dm}")
    print()

    print("EXTREME LOCALITY TEST")
    ft, dt, fm, dm = measure_small_tree(EXTREME_REPEATS)
    print(f"FastDict time: {ft}")
    print(f"dict time: {dt}")
    print(f"FastDict memory: {fm}")
    print(f"dict memory: {dm}")