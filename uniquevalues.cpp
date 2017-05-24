//
// Original:
//   http://lemire.me/blog/2017/05/23/counting-exactly-the-number-of-distinct-elements-sorted-arrays-vs-hash-sets/
//   https://github.com/lemire/Code-used-on-Daniel-Lemire-s-blog/tree/master/2017/05/23
//
// g++ -O3 -o uniquevalues uniquevalues.cpp  -std=c++11 -Wall -Wextra -lJudy
//

#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <vector>
#include <stdint.h>
#include <stdio.h>
#include "benchmark.h"
#include <Judy.h>
#include <random>
#include <algorithm>

size_t distinct_count_hash(const uint64_t * values, size_t howmany) {
  std::unordered_set<uint64_t> hash(values, values + howmany);
  return hash.size();
}

size_t distinct_count_sort(const uint64_t * values, size_t howmany) {
  std::vector<uint64_t> array(values, values + howmany);
  std::sort(array.begin(), array.end());
  return std::unique(array.begin(), array.end()) - array.begin();
}

size_t distinct_count_judy(const uint64_t * values, size_t howmany) {
  void *j_array = nullptr;

  for (; howmany; values++, howmany--) {
    int ret;
    J1S(ret, j_array, *values);
  }

  size_t count, bytes_freed;

  J1C(count, j_array, 0, -1);
  J1FA(bytes_freed, j_array);

  return count;
}

void demo(size_t N, std::string &mode) {
    std::cout << "N = " << N <<std::endl;
    uint64_t * values = new uint64_t[N];
    for(size_t i = 0; i < N; ++i) {
      if (mode == "original")
        values[i] = ((uint16_t)rand()) | ((uint32_t)rand() << 16) | ((uint64_t)rand()<<32) |((uint64_t)rand()<<48);
      else if (mode == "dense_random")
        values[i] = (((uint16_t)rand()) | ((uint32_t)rand() << 16) | ((uint64_t)rand()<<32) |((uint64_t)rand()<<48)) % N;
      else if (mode == "really_dense_random")
        values[i] = (((uint16_t)rand()) | ((uint32_t)rand() << 16) | ((uint64_t)rand()<<32) |((uint64_t)rand()<<48)) % (N / 10);
      else if (mode == "sequential" || mode == "shuffle")
        values[i] = i;
      else if (mode == "rev_sequential")
        values[i] = N - i;
      else {
        std::cerr << "unrecognized mode: " << mode << std::endl;
        ::exit(1);
      }
    }
    if (mode == "shuffle") {
      std::random_device rd;
      std::mt19937 g(rd());
      std::shuffle(values, values + N, g);
    }
    values[N/2] = values[0];
    values[N-1] = values[1];
    size_t expected = distinct_count_hash(values,N);
    std::cout <<" expected = "<<expected << std::endl;
    const int repeat = 1;
    BEST_TIME(distinct_count_hash(values,N), expected, , repeat, N, true);
    BEST_TIME(distinct_count_sort(values,N), expected, , repeat, N, true);
    BEST_TIME(distinct_count_judy(values,N), expected, , repeat, N, true);
    delete[] values;
}
int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Need a mode argument" << std::endl;
    ::exit(1);
  }

  std::string mode(argv[1]);

  for(size_t N = 10; N < 100000000; N*=10) demo(N, mode);
}
