#include "grid.h"
#include "extract.h"
#include <algorithm>
#include <stdexcept>
//------------------------------------------------------------------------------

namespace {
const std::vector<std::string>& valid_grid_text(
    const std::vector<std::string>& text, size_t grid_size)
{
	if (text.size() != grid_size) {
		throw std::invalid_argument("Invalid grid text: row count");
	}
	for (const auto& s : text) {
		if (s.size() != grid_size) {
			throw std::invalid_argument("Invalid grid text: row width");
		}
	}
	return text;
}
} // namespace

//------------------------------------------------------------------------------

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

aoc24::Grid::Grid(size_t grid_size)
    : sz_checked(false)
    , sz(grid_size)
    , sz_y(grid_size)
{}

aoc24::Grid::Grid(size_t grid_size_x, size_t grid_size_y)
    : sz_checked(false)
    , sz(grid_size_x)
    , sz_y(grid_size_y)
{}

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
		if (xy.y == sz_y - 1) {
			xy.x = XY::oob.x;
			xy.y = XY::oob.y;
		} else {
			++xy.y;
		}
	}
}

bool aoc24::Grid::valid_xy(size_t x, size_t y, bool skip_check) const
{
	if (!sz_checked) {
		check_size();
		sz_checked = true;
	}
	if (skip_check) {
		return true;
	}
	return ((0 <= x && x <= sz - 1) && (0 <= y && y <= sz_y - 1));
}
//------------------------------------------------------------------------------

aoc24::Char_grid::Char_grid(char c, size_t grid_size)
    : Grid(grid_size)
{
	g_.reserve(grid_size);
	std::string row = std::string(sz, c);
	for (int i = 0; i < sz; ++i) {
		g_.emplace_back(row);
	}
}

aoc24::Char_grid::Char_grid(char c, size_t grid_width, size_t grid_height)
    : Grid(grid_width, grid_height)
{
	g_.reserve(grid_height);
	std::string row = std::string(grid_width, c);
	for (int i = 0; i < sz; ++i) {
		g_.emplace_back(row);
	}
}

aoc24::Char_grid::Char_grid(const std::string& file_path, size_t grid_size)
    : Grid(grid_size)
    , g_(read_input_into_grid(file_path, grid_size))
{}

aoc24::Char_grid::Char_grid(
    const std::string& file_path, size_t width, size_t height)
    : Grid(width, height)
    , g_(read_input_into_grid(file_path, width, height))
{}

aoc24::Char_grid::Char_grid(const std::vector<std::string>& text,
                            size_t grid_size)
    : Grid(grid_size)
    , g_(valid_grid_text(text, grid_size))
{}

char aoc24::Char_grid::char_at(size_t x, size_t y) const
{
	return ch_at(x, y);
}

aoc24::XY aoc24::Char_grid::find_char(char c,
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

aoc24::XY aoc24::Char_grid::find_char(
    char c, size_t start_x, size_t start_y, int offs_x, int offs_y) const
{
	if (c == nochar) {
		throw std::invalid_argument("Cannot search for reserved char");
	}

	if (!valid_xy(start_x, start_y)) {
		return XY::oob;
	}

	XY cursor = {start_x, start_y};
	for (;;) {
		cursor.x += offs_x;
		cursor.y += offs_y;
		if (!valid_xy(cursor.x, cursor.y)) {
			break;
		}
		if (ch_at(cursor.x, cursor.y, true) == c) {
			return cursor;
		}
	}
	return XY::oob;
}

std::string aoc24::Char_grid::unique_chars() const
{
	std::string chars;
	for (const auto& s : g_) {
		chars += s;
	}
	std::sort(chars.begin(), chars.end());
	chars.erase(std::unique(chars.begin(), chars.end()), chars.end());
	return chars;
}

void aoc24::Char_grid::set_char(size_t x, size_t y, char c)
{
	if (!valid_xy(x, y)) {
		throw std::logic_error("Invalid position");
	}
	if (c == nochar) {
		throw std::invalid_argument("Cannot set to reserved char");
	}
	g_[y][x] = c;
}

void aoc24::Char_grid::check_size() const
{
	if (sz != g_.size()) {
		throw std::runtime_error("The size of the container is "
		                         + std::to_string(g_.size())
		                         + " but was expecting " + std::to_string(sz));
	}

	for (const auto& row : g_) {
		if (sz != row.size()) {
			throw std::runtime_error(
			    "The size of row " + std::to_string(row.size()) + " is "
			    + std::to_string(g_.size()) + " but was expecting "
			    + std::to_string(sz));
		}
	}
}

char aoc24::Char_grid::ch_at(size_t x, size_t y, bool skip_check) const
{
	if (!valid_xy(x, y, skip_check)) {
		return nochar;
	}
	return g_[y][x];
}
