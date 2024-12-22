#include "grp_grid.h"
#include <stack>

aoc24_12::Group_grid::Group_grid(const aoc24::Char_grid& reference_grid,
                                 std::unique_ptr<aoc24::Dyn_bitset> dyn_bitset)
    : Bit_grid(reference_grid.size(), std::move(dyn_bitset))
{
	add_initial_maps(reference_grid);
}
//------------------------------------------------------------------------------

long long aoc24_12::Group_grid::calculate_fence_cost(bool apply_discount)
{
	long long total_cost = 0;
	for (const auto& grp_pair : grp_) {
		long long grp_cost = 0;
		const char map_ch = grp_pair.first;

		// Process sub-groups
		size_t num_sub_grp = 0;

		auto sub_grp_r = sub_grp_.equal_range(map_ch);
		for (auto sub_grp_it = sub_grp_r.first; sub_grp_it != sub_grp_r.second;
		     ++num_sub_grp, ++sub_grp_it) {
			aoc24::Dyn_bitset& map = *(sub_grp_it->second);
			grp_cost += map.count()
			            * (apply_discount ? sides(map) : perim(map));
		}

		// Process main group if there are no sub-groups
		if (num_sub_grp == 0) {
			aoc24::Dyn_bitset& map = *grp_pair.second;
			grp_cost += map.count()
			            * (apply_discount ? sides(map) : perim(map));
		}
		total_cost += grp_cost;
	}
	return total_cost;
}

//------------------------------------------------------------------------------

void aoc24_12::Group_grid::add_initial_maps(
    const aoc24::Char_grid& reference_grid)
{
	// Create fenced tile maps
	add_map(up_key());
	add_map(ri_key());
	add_map(do_key());
	add_map(le_key());
	add_map(up_key() + temp_suffix());
	add_map(ri_key() + temp_suffix());
	add_map(do_key() + temp_suffix());
	add_map(le_key() + temp_suffix());
	cache_fenced_tile_map_ptrs();

	size_t pos = 0;
	for (size_t row = 0; row < sz; ++row) {
		for (size_t col = 0; col < sz; ++col) {
			char c = reference_grid.char_at(col, row);

			// Get group map for char `c` or create a new one
			aoc24::Dyn_bitset* grp_map = nullptr;
			auto grp_it = grp_.find(c);
			if (grp_it == grp_.end()) {
				grp_map = &add_map(std::string{c});
				grp_.insert({c, grp_map});
			} else {
				grp_map = grp_it->second;
			}

			// Mark grouped tile
			grp_map->set(pos);

			// Mark fenced tiles
			set_fenced_tiles(col, row, reference_grid);

			++pos;
		}
	}

	// Create sub group maps
	std::stack<aoc24::XY> stk{}; // Stack used to 'flood-fill' sub groups

	// Temporary bitmaps
	auto temp_grp_map = map_.begin()->second->new_reset_copy();
	auto temp_sub_grp_map = temp_grp_map->new_copy();

	// Find all sub groups for the given group
	for (auto it = grp_.begin(); it != grp_.end(); ++it) {
		aoc24::XY grp_cursor{0, 0};
		const char map_ch = it->first;

		// Copy all group tiles to temporary group map
		aoc24::Dyn_bitset* grp_map = it->second;
		temp_grp_map->reset();
		*temp_grp_map |= *grp_map;

		// Find and add sub groups until map is empty
		size_t tiles_left = temp_grp_map->count();
		for (int num_sub_grp = 0; tiles_left > 0; ++num_sub_grp) {
			// Reset map used to keep track of the current sub group
			temp_sub_grp_map->reset();

			// Place cursor at the first tile in the temporary group map
			grp_cursor = find_bit(*temp_grp_map, 1, grp_cursor.x, grp_cursor.y);
			stk.push(grp_cursor);

			while (!stk.empty()) {
				const aoc24::XY this_xy = stk.top();
				if (b_at(*temp_sub_grp_map, this_xy.x, this_xy.y)) {
					stk.pop();
					continue;
				}

				temp_sub_grp_map->set(xy_to_idx(static_cast<int>(this_xy.x),
				                                static_cast<int>(this_xy.y),
				                                true));
				stk.pop();

				// UP
				if (this_xy.y > 0
				    && !b_at(fenced_tile_map(Direction::Up),
				             this_xy.x,
				             this_xy.y)) {
					stk.push({this_xy.x, this_xy.y - 1});
				}
				// RIGHT
				if (this_xy.x < sz
				    && !b_at(fenced_tile_map(Direction::Right),
				             this_xy.x,
				             this_xy.y)) {
					stk.push({this_xy.x + 1, this_xy.y});
				}
				// DOWN
				if (this_xy.y < sz
				    && !b_at(fenced_tile_map(Direction::Down),
				             this_xy.x,
				             this_xy.y)) {
					stk.push({this_xy.x, this_xy.y + 1});
				}
				// LEFT
				if (this_xy.x > 0
				    && !b_at(fenced_tile_map(Direction::Left),
				             this_xy.x,
				             this_xy.y)) {
					stk.push({this_xy.x - 1, this_xy.y});
				}
			}

			// Remove sub group tiles from group map
			temp_grp_map->subtract(*temp_sub_grp_map);
			tiles_left = temp_grp_map->count();

			// Only add map if there are more than one sub group
			if (tiles_left > 0 || num_sub_grp > 0) {
				std::string sub_grp_name = std::string{map_ch} + "_"
				                           + std::to_string(num_sub_grp);
				auto& new_m = add_map(sub_grp_name);
				new_m |= *temp_sub_grp_map;
				sub_grp_.insert({map_ch, &new_m});
			}

			// Advance cursor
			if (tiles_left > 0) {
				next(grp_cursor);
			}
		}
	}
}

//------------------------------------------------------------------------------

aoc24::Dyn_bitset& aoc24_12::Group_grid::fenced_tile_map(const Direction dir,
                                                         const bool temp)
{
	switch (dir) {
	case Direction::Up:
		return (!temp) ? *up_map_ : *up_map_temp_;
	case Direction::Down:
		return (!temp) ? *do_map_ : *do_map_temp_;
	case Direction::Right:
		return (!temp) ? *ri_map_ : *ri_map_temp_;
	case Direction::Left:
		return (!temp) ? *le_map_ : *le_map_temp_;
		break;
	}
	throw std::invalid_argument("Cannot convert direction of value "
	                            + std::to_string(static_cast<int>(dir)));
}

void aoc24_12::Group_grid::cache_fenced_tile_map_ptrs()
{
	up_map_ = &get_map(up_key());
	ri_map_ = &get_map(ri_key());
	do_map_ = &get_map(do_key());
	le_map_ = &get_map(le_key());
	up_map_temp_ = &get_map(up_key() + temp_suffix());
	ri_map_temp_ = &get_map(ri_key() + temp_suffix());
	do_map_temp_ = &get_map(do_key() + temp_suffix());
	le_map_temp_ = &get_map(le_key() + temp_suffix());
}

void aoc24_12::Group_grid::set_fenced_tiles(
    const size_t x, const size_t y, const aoc24::Char_grid& reference_grid)
{
	char mid_ch = reference_grid.char_at(x, y);
	for (int i = 0; i < 4; ++i) {
		Direction dir = static_cast<Direction>(i);
		Vec2d offs = dir_to_offset(dir);
		bool not_oob = valid_xy(x + offs.x, y + offs.y);
		char nbour_ch = not_oob ? reference_grid.char_at(x + offs.x, y + offs.y)
		                        : '\0';
		if (!not_oob || nbour_ch != mid_ch) {
			fenced_tile_map(dir).set(
			    xy_to_idx(static_cast<int>(x), static_cast<int>(y), true));
		}
	}
}

int aoc24_12::Group_grid::perim(aoc24::Dyn_bitset& map)
{
	prep_temp_maps(map);
	int p = 0;
	for (int i = 0; i < 4; ++i) {
		Direction dir = static_cast<Direction>(i);
		p += fenced_tile_map(dir, true).count();
	}
	return p;
}

int aoc24_12::Group_grid::sides(aoc24::Dyn_bitset& map)
{
	if (map.count() == 1 || map.count() == 2) {
		return 4;
	}

	prep_temp_maps(map);

	// Scan horizontally and vertically for edges
	int s = 0;
	for (size_t i = 0; i < sz; ++i) {
		bool found_up = false;
		bool found_down = false;
		bool found_left = false;
		bool found_right = false;
		for (size_t j = 0; j < sz; ++j) {
			// Horizontal edges
			if (fenced_tile_map(Direction::Up, true).test(i * sz + j)
			    != found_up) {
				if (found_up) {
					++s;
				}
				found_up = !found_up;
			}
			if (fenced_tile_map(Direction::Down, true).test(i * sz + j)
			    != found_down) {
				if (found_down) {
					++s;
				}
				found_down = !found_down;
			}
			// Vertical edges
			if (fenced_tile_map(Direction::Left, true).test(j * sz + i)
			    != found_left) {
				if (found_left) {
					++s;
				}
				found_left = !found_left;
			}
			if (fenced_tile_map(Direction::Right, true).test(j * sz + i)
			    != found_right) {
				if (found_right) {
					++s;
				}
				found_right = !found_right;
			}

			// Last tile in column / row, finish and count any 'open' edges
			if (j == sz - 1) {
				s += (found_up) ? 1 : 0;
				s += (found_down) ? 1 : 0;
				s += (found_left) ? 1 : 0;
				s += (found_right) ? 1 : 0;
				break;
			}
		}
	}

	return s;
}
//------------------------------------------------------------------------------

void aoc24_12::Group_grid::prep_temp_maps(const aoc24::Dyn_bitset& group_map)
{
	// Reset temp maps
	up_map_temp_->reset();
	ri_map_temp_->reset();
	do_map_temp_->reset();
	le_map_temp_->reset();

	// Copy fenced tiles to temp maps
	*up_map_temp_ |= *up_map_;
	*ri_map_temp_ |= *ri_map_;
	*do_map_temp_ |= *do_map_;
	*le_map_temp_ |= *le_map_;

	// Isolate group tiles
	std::unique_ptr<aoc24::Dyn_bitset> m = group_map.new_copy();
	*m |= group_map;
	mask_->set();
	mask_->subtract(*m);
	up_map_temp_->subtract(*mask_);
	ri_map_temp_->subtract(*mask_);
	do_map_temp_->subtract(*mask_);
	le_map_temp_->subtract(*mask_);
}

//------------------------------------------------------------------------------

aoc24::XY aoc24_12::Group_grid::to_xy(int x, int y, const bool skip_check)
{
	if (!skip_check && is_oob(x, y)) {
		throw std::invalid_argument("Cannot convert x=" + std::to_string(x)
		                            + " and y=" + std::to_string(y)
		                            + " to a valid XY of map with size "
		                            + std::to_string(sz));
	}
	return {static_cast<size_t>(x), static_cast<size_t>(y)};
}

aoc24_12::Vec2d aoc24_12::Group_grid::dir_to_offset(const Direction dir)
{
	switch (dir) {
	case Direction::Up:
		return {0, -1};
	case Direction::Down:
		return {0, 1};
	case Direction::Right:
		return {1, 0};
	case Direction::Left:
		return {-1, 0};
		break;
	}
	throw std::invalid_argument("Cannot convert direction of value "
	                            + std::to_string(static_cast<int>(dir)));
	return {0, 0};
}
