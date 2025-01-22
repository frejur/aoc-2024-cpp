#include "vec2d.h"

bool aoc24::Vec2d::operator==(const Vec2d& rhs) const
{
	return (x == rhs.x && y == rhs.y);
}

bool aoc24::Vec2d::operator!=(const Vec2d& rhs) const
{
	return !(*this == rhs);
}

bool aoc24::Vec2d_comparator::operator()(const Vec2d& lhs,
                                         const Vec2d& rhs) const
{
	if (lhs.x != rhs.x) {
		return lhs.x < rhs.x;
	}
	return lhs.y < rhs.y;
}
