#ifndef DAY_15_H
#define DAY_15_H
#include "box_grid_single.h"
#include "box_grid_wide.h"
#include <queue>

namespace aoc24_15 {
long long move_robot_and_get_sum_of_coordinates(
    Box_grid& grid,
    std::queue<Direction> moves,
    bool enable_debug = false,
    const std::string debug_output_filename = "");

void read_grid_and_moves_from_file(const std::string& file_path,
                                   std::vector<std::string>& output_text,
                                   std::queue<Direction>& output_moves,
                                   size_t grid_size,
                                   size_t moves_count);
} // namespace aoc24_15

#endif // DAY_15_H
