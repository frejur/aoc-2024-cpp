#include "int_grid.h"
#include <stdexcept>

aoc24::Int_grid::Int_grid(size_t grid_size_x, size_t grid_size_y)
    : Grid(grid_size_x, grid_size_y)
    , g_(grid_size_y, std::vector<int>(grid_size_x, 0))
{}

int aoc24::Int_grid::value_at(size_t x, size_t y) const
{
	return int_at(x, y);
}

void aoc24::Int_grid::reset()
{
	for (auto& i : g_) {
		i.assign(sz, 0);
	}
}

void aoc24::Int_grid::set_value(size_t x, size_t y, int val)
{
	set(x, y, val);
}

aoc24::XY aoc24::Int_grid::find_value(int val,
                                      size_t start_x,
                                      size_t start_y) const
{
	if (!valid_xy(start_x, start_y)) {
		return XY::oob;
	}

	for (size_t y = start_y; y < sz_y; ++y) {
		for (size_t x = (y == start_y) ? start_x : 0; x < sz; ++x) {
			if (int_at(x, y, true) == val) {
				return {x, y};
			}
		}
	}

	return XY::oob;
}

aoc24::XY aoc24::Int_grid::find_value(
    int val, size_t start_x, size_t start_y, int offs_x, int offs_y) const
{
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
		if (int_at(cursor.x, cursor.y, true) == val) {
			return cursor;
		}
	}
	return XY::oob;
}

int aoc24::Int_grid::int_at(size_t x, size_t y, bool skip_check) const
{
	if (!valid_xy(x, y, skip_check)) {
		throw std::logic_error(
		    "Invalid coordinate pair passed to Int_grid::int_at()");
	}
	return g_[y][x];
}

void aoc24::Int_grid::set(size_t x, size_t y, int val, bool skip_check)
{
	if (!valid_xy(x, y, skip_check)) {
		throw std::logic_error(
		    "Invalid coordinate pair passed to Int_grid::set()");
	}
	g_[y][x] = val;
}

void aoc24::Int_grid::check_size() const
{
	if (sz_y != g_.size()) {
		throw std::runtime_error(
		    "The size of the container is " + std::to_string(g_.size())
		    + " but was expecting " + std::to_string(sz_y));
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
