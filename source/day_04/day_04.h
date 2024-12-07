#ifndef DAY_04_H
#define DAY_04_H
#include "../lib/grid.h"
#include <string>

namespace aoc24_04 {

//------------------------------------------------------------------------------
// Part 1 (definitions in `part_01.cpp`)

enum class Direction {
	North = 0,
	North_east,
	East,
	South_east,
	South,
	South_west,
	West,
	North_west
};
struct Offset
{
	int x;
	int y;
};

Offset dir_to_xy(Direction dir);

bool has_neighbour_str(const aoc24::Grid& grid,
                       size_t x,
                       size_t y,
                       const std::string& s,
                       Direction dir);

int count_xmas(const aoc24::Grid& g);

//------------------------------------------------------------------------------
// Part 2 (definitions in `part_02.cpp`)

int count_x_mas(const aoc24::Grid& g);

char neighbour_ch(const aoc24::Grid& grid, size_t x, size_t y, Direction dir);

} // namespace aoc24_04

#endif // DAY_04_H
