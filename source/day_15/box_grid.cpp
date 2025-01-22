#include "box_grid.h"

aoc24_15::Box_grid::Box_grid(std::unique_ptr<aoc24::Dyn_bitset> dyn_bitset)
    : Bit_grid(static_cast<size_t>(sqrt(dyn_bitset->size())),
               std::move(dyn_bitset))
    , map_ptr_init_(false)
    , num_moves_(0)
    , box_dummy_(Box::dummy())
{}

aoc24_15::Box_grid::Box_grid(size_t width,
                             size_t height,
                             std::unique_ptr<aoc24::Dyn_bitset> dyn_bitset)
    : Bit_grid(width, height, std::move(dyn_bitset))
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

//------------------------------------------------------------------------------

bool aoc24_15::Box_grid::has_cached_l(
    const aoc24::Vec2d& pos,
    const std::set<aoc24::Vec2d, aoc24::Vec2d_comparator> cache)
{
	return (cache.find({pos.x - 1, pos.y}) != cache.end());
};

bool aoc24_15::Box_grid::has_cached_u(
    const aoc24::Vec2d& pos,
    const std::set<aoc24::Vec2d, aoc24::Vec2d_comparator> cache)
{
	return (cache.find({pos.x, pos.y - 1}) != cache.end());
};

void aoc24_15::Box_grid::throw_if_no_map() const
{
	if (!map_ptr_init_) {
		throw std::runtime_error(
		    "Cannot access map: Maps have not been initialized");
	}
}
