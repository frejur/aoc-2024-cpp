#ifndef DAY_02_H
#define DAY_02_H
#include <string>
#include <vector>

namespace aoc24_02 {

void read_input_into_reports(const std::string& path_to_input,
                             std::vector<std::vector<int>>& reports);

//------------------------------------------------------------------------------
// Part 1 (definitions in `part_01.cpp`)

std::vector<int> unsafe_indices(const std::vector<std::vector<int>>& reports);
bool delta_is_safe(const int value_a, const int value_b, const bool increase);
bool is_safe(const std::vector<int>& report, const size_t skip_idx = -1);

//------------------------------------------------------------------------------
// Part 2 (definitions in `part_02.cpp`)

int count_safe_dampened(const std::vector<std::vector<int>>& reports,
                        const std::vector<int> indices);
std::vector<int> unsafe_dampened_indices(
    const std::vector<std::vector<int>>& reports,
    const std::vector<int> indices);
std::vector<int> unsafe_dampened_indices_brute(
    const std::vector<std::vector<int>>& reports,
    const std::vector<int> indices);

} // namespace aoc24_02

#endif // DAY_02_H
