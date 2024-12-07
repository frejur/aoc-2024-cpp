#include "day_04.h"

char aoc24_04::neighbour_ch(const aoc24::Grid& grid,
                            size_t x,
                            size_t y,
                            Direction dir)
{
	Offset offs{dir_to_xy(dir)};
	return grid.char_at(x + offs.x, y + offs.y);
}

int aoc24_04::count_x_mas(const aoc24::Grid& g)
{
	int n = 0;
	aoc24::XY xy{0, 0};
	while (xy = g.find_char('A', xy.x, xy.y), xy != aoc24::XY::oob) {
		char ne{neighbour_ch(g, xy.x, xy.y, Direction::North_east)};
		char se{neighbour_ch(g, xy.x, xy.y, Direction::South_east)};
		char sw{neighbour_ch(g, xy.x, xy.y, Direction::South_west)};
		char nw{neighbour_ch(g, xy.x, xy.y, Direction::North_west)};
		n += (((ne == 'M' && sw == 'S') || (ne == 'S' && sw == 'M'))
		      && ((nw == 'M' && se == 'S') || (nw == 'S' && se == 'M')));
		g.next(xy);
	}
	return n;
}
