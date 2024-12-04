#ifndef DAY_01_H
#define DAY_01_H
#include <string>
#include <vector>

namespace aoc24_02 {

void read_input_into_reports(const std::string& path_to_input,
                             std::vector<std::vector<int>>& reports);

//------------------------------------------------------------------------------
// Part 1 (definitions in `part_01.cpp`)

int count_safe(const std::vector<std::vector<int>>& reports);
bool is_safe(const std::vector<int>& report);

//------------------------------------------------------------------------------
// Part 2 (definitions in `part_02.cpp`)

} // namespace aoc24_02

#endif // DAY_01_H
