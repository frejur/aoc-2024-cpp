#ifndef EDGE_TILE_H
#define EDGE_TILE_H
#include "../lib/direction.h"
#include "../lib/vec2d.h"

namespace aoc24_15 {

class Box_new;

struct Edge_tile
{
	constexpr Edge_tile()
	    : position(0, 0)
	    , direction(aoc24::Direction::Up)
	{}
	constexpr Edge_tile(
	    int pos_x, int pos_y, aoc24::Direction dir)
	    : position(pos_x, pos_y)
	    , direction(dir)
	{}
	constexpr Edge_tile(
	    const aoc24::Vec2d& pos_xy, aoc24::Direction dir)
	    : position(pos_xy.x, pos_xy.y)
	    , direction(dir)
	{}
	const aoc24::Vec2d position;
	const aoc24::Direction direction;

	bool operator==(
	    const Edge_tile& rhs)
	{
		return (position == rhs.position) && (direction == rhs.direction);
	}
};

struct Edge_tile_comparator
{
	Edge_tile_comparator() = default;
	Edge_tile_comparator(const Edge_tile_comparator& rhs) = default;
	bool operator()(const Edge_tile& lhs, const Edge_tile& rhs) const;
};

struct Edge_tile_instruction
{
	Edge_tile_instruction(const Edge_tile& t, Box_new* b);
	const Edge_tile tile;
	Box_new* box;
};
} // namespace aoc24_15

#endif // EDGE_TILE_H
