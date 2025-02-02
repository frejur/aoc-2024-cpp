#include "direction.h"
#include "../lib/vec2d.h"
#include <string>

//------------------------------------------------------------------------------

std::istream& aoc24::operator>>(std::istream& istr, Direction& dir)
{
	char c{};
	istr >> c;
	switch (c) {
	case '^':
		dir = Direction::Up;
		break;
	case '>':
		dir = Direction::Right;
		break;
	case 'v':
		dir = Direction::Down;
		break;
	case '<':
		dir = Direction::Left;
		break;
	default:
		istr.clear(std::ios_base::failbit);
	}
	return istr;
}

std::ostream& aoc24::operator<<(std::ostream& ostr, const Direction& dir)
{
	switch (dir) {
	case Direction::Up:
		ostr << '^';
		break;
	case Direction::Right:
		ostr << '>';
		break;
	case Direction::Down:
		ostr << 'v';
		break;
	case Direction::Left:
		ostr << '<';
		break;
	}
	return ostr;
}

//------------------------------------------------------------------------------

aoc24::Vec2d aoc24::dir_to_offset(Direction dir, bool invert)
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

aoc24::Direction aoc24::dir_turn_left(const aoc24::Direction dir)
{
	return static_cast<aoc24::Direction>((static_cast<int>(dir) + 3) % 4);
}

aoc24::Direction aoc24::dir_turn_right(const aoc24::Direction dir)
{
	return static_cast<aoc24::Direction>((static_cast<int>(dir) + 1) % 4);
}

aoc24::Direction aoc24::dir_inverted(const Direction dir)
{
	return static_cast<aoc24::Direction>((static_cast<int>(dir) + 2) % 4);
}
