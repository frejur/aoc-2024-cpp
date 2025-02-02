#include "vec2d.h"

bool aoc24::Vec2d::operator==(const Vec2d& rhs) const
{
	return (x == rhs.x && y == rhs.y);
}

bool aoc24::Vec2d::operator!=(const Vec2d& rhs) const
{
	return !(*this == rhs);
}

aoc24::Vec2d aoc24::Vec2d::operator+(const Vec2d& rhs) const
{
	return {x + rhs.x, y + rhs.y};
}

aoc24::Vec2d aoc24::Vec2d::operator-(const Vec2d& rhs) const
{
	return {x - rhs.x, y - rhs.y};
}

bool aoc24::Vec2d_comparator::operator()(const Vec2d& lhs,
                                         const Vec2d& rhs) const
{
	if (lhs.x != rhs.x) {
		return lhs.x < rhs.x;
	}
	return lhs.y < rhs.y;
}

std::string aoc24::xytoa(int pos_x, int pos_y)
{
	return std::string("(" + std::to_string(pos_x) + ", "
	                   + std::to_string(pos_y) + ")");
}
