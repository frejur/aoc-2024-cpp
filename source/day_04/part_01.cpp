#include "day_04.h"
#include <stdexcept>

int aoc24_04::count_xmas(const Grid& g)
{
	int n = 0;
	XY xy{0, 0};
	while (xy = g.find_char('X', xy.x, xy.y), xy != XY::oob) {
		for (int i = 0; i < 8; ++i) {
			Direction d{static_cast<Direction>(i)};
			bool match = has_neighbour_str(g, xy.x, xy.y, "MAS", d);
			n += match ? 1 : 0;
		}
		g.next(xy);
	}
	return n;
}

//------------------------------------------------------------------------------

const aoc24_04::XY aoc24_04::XY::oob = {XY::noxy, XY::noxy};

bool aoc24_04::XY::operator==(const XY& a) const
{
	return (x == a.x && y == a.y);
}
bool aoc24_04::XY::operator!=(const XY& a) const
{
	return !(*this == a);
}

//------------------------------------------------------------------------------

aoc24_04::Grid::Grid(const std::string& file_path, size_t grid_size)
    : sz(grid_size)
    , g_(read_input_into_strings(file_path, grid_size))
{}

char aoc24_04::Grid::char_at(size_t x, size_t y) const
{
	return ch_at(x, y);
}

aoc24_04::XY aoc24_04::Grid::find_char(char c,
                                       size_t start_x,
                                       size_t start_y) const
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

void aoc24_04::Grid::previous(XY& xy) const
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

void aoc24_04::Grid::next(XY& xy) const
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

bool aoc24_04::Grid::valid_xy(size_t x, size_t y) const
{
	return ((0 <= x && x <= sz - 1) && (0 <= y && y <= sz - 1));
}

char aoc24_04::Grid::ch_at(size_t x, size_t y, bool skip_check) const
{
	if (!skip_check && !valid_xy(x, y)) {
		return nochar;
	}
	return g_[y][x];
}

//------------------------------------------------------------------------------
aoc24_04::Offset aoc24_04::dir_to_xy(Direction dir)
{
	using D = Direction;
	int offs_x{}, offs_y{};
	if (dir == D::North || dir == D::North_east || dir == D::North_west) {
		offs_y = -1;
	} else if (dir == D::South || dir == D::South_east || dir == D::South_west) {
		offs_y = 1;
	}
	if (dir == D::East || dir == D::North_east || dir == D::South_east) {
		offs_x = 1;
	} else if (dir == D::West || dir == D::North_west || dir == D::South_west) {
		offs_x = -1;
	}
	return {offs_x, offs_y};
}

bool aoc24_04::has_neighbour_str(
    const Grid& grid, size_t x, size_t y, const std::string& s, Direction dir)
{
	Offset xy{dir_to_xy(dir)};
	for (size_t i = 0; i < s.size(); ++i) {
		if (s[i] != grid.char_at(x + xy.y * (i + 1), y + xy.y * (i + 1))) {
			return false;
		}
	}
	return true;
}
