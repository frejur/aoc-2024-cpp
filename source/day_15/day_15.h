#ifndef DAY_15_H
#define DAY_15_H
#include "warehouse.h"
#include <queue>

namespace aoc24_15 {
long long move_robot_and_get_sum_of_coordinates(
    Warehouse& grid,
    std::queue<aoc24::Direction> moves,
    bool enable_debug = false,
    const std::string debug_output_filename = "");

void read_grid_and_moves_from_file(const std::string& file_path,
                                   std::vector<std::string>& output_text,
                                   std::queue<aoc24::Direction>& output_moves,
                                   size_t grid_size,
                                   size_t moves_count);



std::ostream& operator<<(std::ostream& ostr, aoc24::Vec2d& vec);

} // namespace aoc24_15

#endif // DAY_15_H
