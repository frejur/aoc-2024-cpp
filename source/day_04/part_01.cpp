#include "day_04.h"

int aoc24_04::count_xmas(const aoc24::Char_grid& g)
{
	int n = 0;
	aoc24::XY xy{0, 0};
	while (xy = g.find_char('X', xy.x, xy.y), xy != aoc24::XY::oob) {
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

bool aoc24_04::has_neighbour_str(const aoc24::Char_grid& grid,
                                 size_t x,
                                 size_t y,
                                 const std::string& s,
                                 Direction dir)
{
	Offset xy{dir_to_xy(dir)};
	for (size_t i = 0; i < s.size(); ++i) {
		if (s[i] != grid.char_at(x + xy.x * (i + 1), y + xy.y * (i + 1))) {
			return false;
		}
	}
	return true;
}
