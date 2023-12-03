#include <parlay/parallel.h>
#include <parlay/sequence.h>
#include <parlay/random.h>
#include <chrono>
#include <cstdint>
#include <cmath>

template <typename T>
void par_quicksort(parlay::sequence<T>& arr, int64_t low, int64_t high) {
  if (low < high) {
    T pivot = arr[high];
    int64_t i = low - 1;
    for (int64_t j = low; j < high; j++) {
      if (arr[j] < pivot) {
        i++;
        std::swap(arr[i], arr[j]);
      }
    }
    std::swap(arr[i + 1], arr[high]);
    int64_t partition_idx = i + 1;

    parlay::par_do(
      [&]() { par_quicksort(arr, low, partition_idx - 1); },
      [&]() { par_quicksort(arr, partition_idx + 1, high); }
    );
  }
}