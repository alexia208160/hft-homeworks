HW 2 — pointers, references & the cost of a copy
elements N = 1048576   sizeof(Big) = 648 B   sizeof(Node) = 16 B

TABLE 1 — swap correctness
  function           a before   b before    a after    b after   result
  ---------------------------------------------------------------------
  swap_ref                  3          9          9          3   OK
  swap_ptr                  3          9          9          3   OK
  swap_ptr(&a,&a)           5          5          5          5   OK

TABLE 2 — pass a 648-byte struct: by value vs by const reference
  variant                             p50          p99        p99.9         mean
                                  ns/call      ns/call      ns/call      ns/call
  ---------------------------------------------------------------------------------
  sum_by_value(Big)                33.416       55.541       67.084       35.633
  sum_by_cref(const Big&)          21.605       24.041       26.625       21.488
  p50 ratio value/cref = 1.55x     (checksum 30202200000.0, 1000 samples x 2000 calls)
  correctness: sum_by_value=7191.0  sum_by_cref=7191.0  OK (equal, non-zero)

TABLE 3 — traverse 1,048,576 ints: contiguous vector vs linked list
  variant                             p50          p99        p99.9         mean
                                  ns/elem      ns/elem      ns/elem      ns/elem
  ---------------------------------------------------------------------------------
  sum_vector (contiguous)           0.032        0.043        0.047        0.033
  sum_list   (pointer chase)       14.297       65.916       81.825       18.731
  p50 ratio list/vector = 445.29x    (200 samples x 1 full traversal)
  correctness: sum_vector=549755289600  sum_list=549755289600  expected=549755289600  OK
  bytes touched: vector 4.0 MB, list 16.0 MB

TABLE 4 — build & method (state your machine in the README)
  compiler               clang++ 15.0.0 (clang-1500.3.9.4)
  optimisation           -O2/-O3 (release)  OK
  language               __cplusplus = 201703L
  arch                   arm64
  clock                  std::chrono::steady_clock (monotonic)
  warm-up                yes — whole batches before the first sample
  reported               p50 / p99 / p99.9 over samples, plus mean
  dead-code guard        doNotOptimize() on every result
  CPU                    Apple M4 Pro (12 cores)
  RAM                    24GB 
  OS                     macOS 26.6.2
  Noise reduction        all other apps closed, not charging, set to "auto"     
                         performance


both get passed the addresses of a & b, since a reference is really just a pointer the compiler dereferences for us, so both cost the same, one address each.

swap_ptr needs the * because we get passed addresses, not the ints, so we have to dereference to reach the values. swap_ref does not because the compiler dereferences the reference for us.

swapping the pointers inside swap_ptr does nothing to the caller because the pointers are copies, so we only swap our own local copies of the addresses and a & b in the caller never change.

a reference has to be bound to a real variable, so the caller cannot pass nothing. a pointer can be nullptr, so the caller can pass nothing, and if we dereference it we get undefined behavior, usually a crash. I did not check for null because non-null is a precondition of the function, it is the caller's job to pass valid addresses.

I would use swap_ref in an HFT hot path because it compiles to the same instructions as swap_ptr, can never be null so there is no check to "pay" for.

In a 64-byte cache line you could fit 16 ints, with the 16 bytes from a node you only use a quarter every time, since most of it is the pointer and padding. It helps the vector because the memory is contiguous, so one cache line brings in many elements at once and the CPU can load the next lines ahead of time. In the node case each node can be on a different cache line anywhere in memory, so every node can be its own cache miss and the CPU cannot predict where to go next. Also you cannot load the next node until you have read the current node's pointer, so the misses happen one after another instead of overlapping, which is the why the list is so much slower.


By value copies 648 bytes per call, by reference passes an 8-byte address. From Table 3, the vector reads about 125 GB/s (4 bytes per 0.032 ns), so copying 648 bytes in and out should cost about 10 ns, which matches the 12 ns gap at p50 (33.4 vs 21.6 ns). At p99.9 the copy adds about 40 ns (67.1 vs 26.6 ns), while by reference barely changes, so by const reference would be the choice for an HFT hot path.
