#include "trail_grid.h"

aoc24_06::Trail_grid::Trail_grid(const aoc24::Char_grid& reference_grid,
                                 std::unique_ptr<aoc24::Dyn_bitset> dyn_bitset)
    : Bit_grid(reference_grid.size(), obstruction_key(), std::move(dyn_bitset))
{
	aoc24::XY xy{0, 0};
	while (xy = reference_grid.find_char(obstruction, xy.x, xy.y),
	       xy != aoc24::XY::oob) {
		set(xy);
		reference_grid.next(xy);
	}
	add_map(trail_key());
	add_map(new_obstruction_key());
	add_map(up_key());
	add_map(right_key());
	add_map(down_key());
	add_map(left_key());
	add_map(up_key() + future_key_suffix());
	add_map(right_key() + future_key_suffix());
	add_map(down_key() + future_key_suffix());
	add_map(left_key() + future_key_suffix());
}

void aoc24_06::Trail_grid::mark_tile(const aoc24::XY& pos,
                                     const std::string& map_key,
                                     const bool bit)
{
	if (bit) {
		set(map_key, pos);
	} else {
		reset(map_key, pos);
	}
}

void aoc24_06::Trail_grid::mark_tiles(const aoc24::XY& a,
                                      const aoc24::XY& b,
                                      const std::string& map_key,
                                      const bool bit)
{
	bool horizontal = false;
	int start = 0;
	int end = 0;
	if (a.y == b.y) {
		// Horizontal
		horizontal = true;
		start = static_cast<int>((std::min)(a.x, b.x));
		end = static_cast<int>((std::max)(a.x, b.x));
	} else if (a.x == b.x) {
		// Vertical
		horizontal = false;
		start = static_cast<int>((std::min)(a.y, b.y));
		end = static_cast<int>((std::max)(a.y, b.y));
	} else {
		throw std::invalid_argument("Only horizontal / vertical paths allowed");
	}

	if (end - start == 0) {
		mark_tile(a, map_key, bit);
		return;
	}

	if (horizontal) {
		mask_row(a.y, start, static_cast<int>(sz - end - 1));
	} else {
		mask_col(a.x, start, static_cast<int>(sz - end - 1));
	}
	apply_mask(map_key, !bit);
}

void aoc24_06::Trail_grid::reset_trail()
{
	reset(trail_key());
}

void aoc24_06::Trail_grid::print_trail(std::ostream& ostr)
{
	print(trail_key());
}

int aoc24_06::Trail_grid::number_of_marked_tiles() const
{
	return count(trail_key());
}

int aoc24_06::Trail_grid::number_of_new_obstructions() const
{
	return count(new_obstruction_key());
}
