#include <iostream>
#include <parlay/parallel.h>
#include <parlay/sequence.h>
#include <parlay/random.h>
#include <chrono>
#include <cstdint>
#include <cmath>
#include <string>

#include "seq.cpp"
#include "par.cpp"

int main(int argc, char* argv[]) {

  if (argc < 2) {
    std::cout << "Please provide a 'method' parameter. Values expected: ['seq', 'par']" << std::endl;
    return 1;
  }
  std::string method = argv[1];

  srand(42);
  size_t workers_cnt = parlay::num_workers();
  if (workers_cnt != 4) {
    std::cout << "Number of parlaylib workers set: "+std::to_string(workers_cnt)+". Expected: 4" << std::endl;
    return 1;
  }

  const long max_rand = 1000000000L;
 
  double lower_bound = 0;
  double upper_bound = 100;

  // Generate random doubles
  auto gen_start_time = std::chrono::high_resolution_clock::now();
  size_t n = size_t(1e8);
  parlay::sequence<double> arr(n);
  for (size_t i = 0; i < n; i++) {
    arr[i] = lower_bound + (upper_bound - lower_bound) * (random() % max_rand) / max_rand;
  }
  auto gen_end_time = std::chrono::high_resolution_clock::now();
  auto gen_duration = std::chrono::duration_cast<std::chrono::microseconds>(gen_end_time - gen_start_time);

  // Measure execution time of quicksort
  auto start_time = std::chrono::high_resolution_clock::now();
  if (method == "seq") {
    seq_quicksort(arr, 0, n - 1);
  } else if (method == "par") {
    par_quicksort(arr, 0, n - 1);
  } else {
    std::cout << "Unknown 'method' parameter: '"+method+"'. Values expected: ['seq', 'par']" << std::endl;
    return 1;
  }
  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

  // Print the sorted array
  std::cout << "Sorted:" << std::endl;
  int step = 100000;
  for (size_t i = 0; i < n; i+=step) {
    std::cout << arr[i] << " ";
  }
  std::cout << std::endl;

  // Print the execution time
  std::cout << "Execution Time: " << std::round(duration.count()/1e3)/1e3 << " s " 
            << "Generating time: " << std::round(gen_duration.count()/1e3)/1e3 << " s" << std::endl;

  return 0;
}