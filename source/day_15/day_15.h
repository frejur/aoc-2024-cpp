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


} // namespace aoc24_15

#endif // DAY_15_H
