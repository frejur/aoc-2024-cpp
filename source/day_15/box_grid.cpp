#include "box_grid.h"

bool aoc24_15::Vec2d::operator<(const Vec2d& rhs) const
{
	if (y != rhs.y) {
		return y < rhs.y;
	} else {
		return x < rhs.x;
	}
}

//------------------------------------------------------------------------------

aoc24_15::Vec2d aoc24_15::dir_to_offset(Direction dir, bool invert)
{
	if (invert) {
		dir = static_cast<Direction>((static_cast<int>(dir) + 2) % 4);
	}
	switch (dir) {
	case Direction::Up:
		return {0, -1};
	case Direction::Down:
		return {0, 1};
	case Direction::Right:
		return {1, 0};
	case Direction::Left:
		return {-1, 0};
		break;
	}
	throw std::invalid_argument("Cannot convert direction of value "
	                            + std::to_string(static_cast<int>(dir)));
	return {0, 0};
}

//------------------------------------------------------------------------------

aoc24_15::Box_grid::Box_grid(std::unique_ptr<aoc24::Dyn_bitset> dyn_bitset)
    : Bit_grid(sqrt(dyn_bitset->size()), std::move(dyn_bitset))
    , map_ptr_init_(false)
    , num_moves_(0)
    , box_dummy_(Box::dummy())
{}

//------------------------------------------------------------------------------

aoc24::XY aoc24_15::Box_grid::to_xy(int x, int y, const bool skip_check)
{
	if (!skip_check && is_oob(x, y)) {
		throw std::invalid_argument("Cannot convert x=" + std::to_string(x)
		                            + " and y=" + std::to_string(y)
		                            + " to a valid XY of map with size "
		                            + std::to_string(sz));
	}
	return {static_cast<size_t>(x), static_cast<size_t>(y)};
}

aoc24_15::Direction aoc24_15::Box_grid::dir_turn_left(const Direction dir) const
{
	return static_cast<Direction>((static_cast<int>(dir) + 3) % 4);
}

aoc24_15::Direction aoc24_15::Box_grid::dir_turn_right(const Direction dir) const
{
	return static_cast<Direction>((static_cast<int>(dir) + 1) % 4);
}

//------------------------------------------------------------------------------

void aoc24_15::Robot::spawn(int pos_x, int pos_y)
{
	init_pos_ = {pos_x, pos_y};
	pos_ = init_pos_;
	has_spawned_ = true;
}

void aoc24_15::Robot::move(Direction dir)
{
	Vec2d offs = dir_to_offset(dir);
	pos_.x += offs.x;
	pos_.y += offs.y;
}
