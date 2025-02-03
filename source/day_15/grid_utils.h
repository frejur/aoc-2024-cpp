#ifndef GRID_UTILS_H
#define GRID_UTILS_H
#include "../lib/grid.h"
#include "../lib/vec2d.h"

namespace aoc24_15 {
aoc24::Char_grid create_new_wider_grid(const aoc24::Char_grid& grid);
void extract_spawn_and_box_positions(aoc24::Char_grid& grid,
                                     aoc24::Vec2d& spawn_pos,
                                     std::vector<aoc24::Vec2d>& box_pos_v);
} // namespace aoc24_15

#endif // GRID_UTILS_H
