#ifndef DAY_01_H
#define DAY_01_H
#include <string>
#include <vector>

namespace aoc24_01 {

void read_input_into_l_r(const std::string& path_to_input,
                         std::vector<int>& left_vector,
                         std::vector<int>& right_vector);

//------------------------------------------------------------------------------
// Part 1 (definitions in `part_01.cpp`)

int total_l_r_dist(const std::vector<int>& sorted_left_vector,
                   const std::vector<int>& sorted_right_vector);
int l_r_dist(const int left_value, const int right_value);

//------------------------------------------------------------------------------
// Part 2 (definitions in `part_02.cpp`)

int total_l_r_similarity_score(const std::vector<int>& sorted_left_vector,
                               const std::vector<int>& sorted_right_vector);
int count_val_at_idx(const std::vector<int>& sorted_vector, const int idx);

inline int l_r_similarity_score(const int left_value_count,
                                const int right_value_count,
                                const int left_value);

} // namespace aoc24_01

#endif // DAY_01_H
