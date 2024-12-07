#include "grid.h"
#include "extract.h"
#include <stdexcept>

const aoc24::XY aoc24::XY::oob = {XY::noxy, XY::noxy};

bool aoc24::XY::operator==(const XY& a) const
{
	return (x == a.x && y == a.y);
}
bool aoc24::XY::operator!=(const XY& a) const
{
	return !(*this == a);
}

//------------------------------------------------------------------------------

aoc24::Grid::Grid(const std::string& file_path, size_t grid_size)
    : sz(grid_size)
    , g_(read_input_into_grid(file_path, grid_size))
{}

char aoc24::Grid::char_at(size_t x, size_t y) const
{
	return ch_at(x, y);
}

aoc24::XY aoc24::Grid::find_char(char c, size_t start_x, size_t start_y) const
{
	if (c == nochar) {
		throw std::invalid_argument("Cannot search for reserved char");
	}

	if (!valid_xy(start_x, start_y)) {
		return XY::oob;
	}

	for (size_t y = start_y; y < sz; ++y) {
		for (size_t x = (y == start_y) ? start_x : 0; x < sz; ++x) {
			if (ch_at(x, y, true) == c) {
				return {x, y};
			}
		}
	}

	return XY::oob;
}

void aoc24::Grid::previous(XY& xy) const
{
	if (xy == XY::oob || !valid_xy(xy.x, xy.y)) {
		return;
	}
	xy.x = (xy.x == 0) ? sz - 1 : xy.x - 1;

	if (xy.x == sz - 1) {
		if (xy.y == 0) {
			xy.x = XY::oob.x;
			xy.y = XY::oob.y;
		} else {
			--xy.y;
		}
	}
}

void aoc24::Grid::next(XY& xy) const
{
	if (xy == XY::oob || !valid_xy(xy.x, xy.y)) {
		return;
	}
	xy.x = (xy.x == sz - 1) ? 0 : xy.x + 1;

	if (xy.x == 0) {
		if (xy.y == sz - 1) {
			xy.x = XY::oob.x;
			xy.y = XY::oob.y;
		} else {
			++xy.y;
		}
	}
}

bool aoc24::Grid::valid_xy(size_t x, size_t y) const
{
	return ((0 <= x && x <= sz - 1) && (0 <= y && y <= sz - 1));
}

char aoc24::Grid::ch_at(size_t x, size_t y, bool skip_check) const
{
	if (!skip_check && !valid_xy(x, y)) {
		return nochar;
	}
	return g_[y][x];
}
