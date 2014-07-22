#include <iostream>
#include <vector>
#include "tools/ProbSchedule.h"
#include "tools/Random.h"

int main() {
  const int val_count = 100;
  const int test_count = 10000000;

  emk::ProbSchedule sched(val_count);
  emk::Random random;

  std::vector<double> merits(val_count);

  double total_val = 0.0;
  for (int i = 0; i < val_count; i++) {
    const double new_val = random.GetDouble();
    total_val += new_val;
    merits[i] = new_val;
    sched.Adjust(i, new_val);
  }

  std::vector<int> found_counts(val_count);
  for (int i = 0; i < val_count; i++) found_counts[i] = 0;

  const int step = test_count / 10;
  for (int i = 0; i < test_count; i++) {
    if ((i+1) % step == 0) {
      std::cout << 10 * (i+1) / step << "% done..." << std::endl;
    }
    int found_id = sched.NextID();
    found_counts[found_id]++;
  }

  for (int i = 0; i < val_count; i++) {
    std::cout << i << " weight=" << merits[i]
              << " found=" << found_counts[i]
              << " (" << (merits[i] / total_val) * test_count << " expected)"
              << std::endl;
  }

  std::cout << "Total = " << total_val << std::endl;

}
