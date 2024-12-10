#ifndef DAY_06_H
#define DAY_06_H
#include "../lib/bit_grid.h"
#include "../lib/grid.h"
#include <iostream>

namespace aoc24_06 {

//------------------------------------------------------------------------------
// Part 1 (definitions in `part_01.cpp`)

enum class Direction {
	North = 0,
	East,
	South,
	West,
};
struct Pos2d
{
	int x;
	int y;
};

Pos2d dir_to_xy(Direction dir);

class Guard
{
public:
	Guard(Pos2d position);
	Pos2d current_position() const;
	Direction current_direction() const;
	void move_forward(int steps = 1);
	void turn_right();

private:
	Pos2d pos;
	Direction dir;
};

//------------------------------------------------------------------------------

class Trail_grid : public aoc24::Bit_grid
{
public:
	Trail_grid(const aoc24::Char_grid& reference_grid,
	           std::unique_ptr<aoc24::Dyn_bitset> dyn_bitset);
	void mark_tile(aoc24::XY pos);
	void mark_trail(aoc24::XY a, aoc24::XY b);
	void print_trail(std::ostream& ostr = std::cout);
	int number_of_marked_tiles() const;

private:
};

//------------------------------------------------------------------------------

Guard spawn_guard(const aoc24::Char_grid& grid);
int steps_between(const aoc24::XY& a, const aoc24::XY& b);
int move_to_limit(Guard& guard, const Trail_grid& grid);
void move_and_mark_until_oob(Guard& guard, Trail_grid& grid);

//------------------------------------------------------------------------------
// Part 2 (definitions in `part_02.cpp`)

} // namespace aoc24_06

#endif // DAY_06_H
