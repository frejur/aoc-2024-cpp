#ifndef IO_H
#define IO_H
#include "../lib/grid.h"
#include "../lib/vec2d.h"

namespace aoc24_16 {

void extract_start_and_end_pos(aoc24::Char_grid& grid,
                               aoc24::Vec2d& start,
                               aoc24::Vec2d& end);

}

#endif // IO_H
