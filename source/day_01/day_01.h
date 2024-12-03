#ifndef DAY_01_H
#define DAY_01_H
#include <string>
#include <vector>

namespace aoc24_01 {

const std::string& input_file();

void read_input_into_l_r(const std::string& path_to_input,
                         std::vector<int>& left_vector,
                         std::vector<int>& right_vector);

//------------------------------------------------------------------------------
// Part 1 (definitions in `part_01.cpp`)

int total_l_r_dist(const std::vector<int>& sorted_left_vector,
                   const std::vector<int>& sorted_right_vector);
int l_r_dist(const int left_value, const int right_value);

} // namespace aoc24_01

#endif // DAY_01_H
