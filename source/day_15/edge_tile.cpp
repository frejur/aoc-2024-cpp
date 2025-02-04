#include "edge_tile.h"

bool aoc24_15::Edge_tile_comparator::operator()(
    const Edge_tile& lhs, const Edge_tile& rhs) const
{
	if (lhs.position.x != rhs.position.x) {
		return lhs.position.x < rhs.position.x;
	}
	if (lhs.position.y != rhs.position.y) {
		return lhs.position.y < rhs.position.y;
	}
	return static_cast<int>(lhs.direction) < static_cast<int>(rhs.direction);
}

aoc24_15::Edge_tile_instruction::Edge_tile_instruction(
    const Edge_tile& t, Box_new* b)
    : tile(t)
    , box(b)
{}
