#include "guard.h"
#include <stdexcept>

aoc24_06::Direction aoc24_06::Guard::direction_to_the_left() const
{
	return static_cast<Direction>((static_cast<int>(dir) + 3) % 4);
}

aoc24_06::Pos2d aoc24_06::dir_to_xy(Direction dir)
{
	using D = Direction;
	int offs_x{}, offs_y{};
	offs_x = dir == D::East ? 1 : (dir == D::West ? -1 : 0);
	offs_y = dir == D::South ? 1 : (dir == D::North ? -1 : 0);
	return {offs_x, offs_y};
}

aoc24_06::Guard::Guard(Pos2d position)
    : is_future(false)
    , pos(position)
    , dir(Direction::North)
{}

aoc24_06::Pos2d aoc24_06::Guard::current_position() const
{
	return pos;
}

aoc24_06::Pos2d aoc24_06::Guard::simulated_position(int steps) const
{
	if (steps < 1) {
		throw std::invalid_argument("Parameter steps must > 0");
	}
	Pos2d offs{dir_to_xy(dir)};
	return {pos.x + offs.x * steps, pos.y + offs.y * steps};
}

aoc24_06::Direction aoc24_06::Guard::current_direction() const
{
	return dir;
}
void aoc24_06::Guard::move_forward(int steps)
{
	if (steps < 1) {
		throw std::invalid_argument("Parameter steps must > 0");
	}
	Pos2d offs{dir_to_xy(dir)};
	pos.x += offs.x * steps;
	pos.y += offs.y * steps;
}
void aoc24_06::Guard::turn_right()
{
	dir = static_cast<Direction>((static_cast<int>(dir) + 1) % 4);
}

//------------------------------------------------------------------------------
