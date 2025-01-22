#ifndef VEC2D_H
#define VEC2D_H

namespace aoc24 {

struct Vec2d
{
	Vec2d(int xx, int yy)
	    : x(xx)
	    , y(yy)
	{}
	int x;
	int y;
	bool operator==(const Vec2d& rhs) const;
	bool operator!=(const Vec2d& rhs) const;
};

struct Vec2d_comparator
{
	bool operator()(const aoc24::Vec2d& lhs, const aoc24::Vec2d& rhs) const;
};

} // namespace aoc24
//------------------------------------------------------------------------------

#endif // VEC2D_H
