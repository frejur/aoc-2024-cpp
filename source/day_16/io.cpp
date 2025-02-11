#include "io.h"
#include "keys.h"
#include <cassert>

void aoc24_16::extract_start_and_end_pos(
    aoc24::Char_grid& grid, aoc24::Vec2d& start, aoc24::Vec2d& end)
{
	for (int i = 0; i < 2; ++i) {
		char c = (i == 0) ? chars::start_char : chars::end_char;
		aoc24::XY match = grid.find_char(c, 0, 0);
		assert(match != aoc24::XY::oob);

		grid.set_char(match.x, match.y, chars::empty_char);

		aoc24::Vec2d& xy = (i == 0) ? start : end;
		xy.x = static_cast<int>(match.x);
		xy.y = static_cast<int>(match.y);
	}
}
