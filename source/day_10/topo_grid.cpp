#include "topo_grid.h"
#include <algorithm>
#include <random>
#include <set>

namespace {
constexpr size_t min_size_for_trail_mask{10};
}
//------------------------------------------------------------------------------

aoc24_10::Topo_grid::Topo_grid(const aoc24::Char_grid& reference_grid,
                               std::unique_ptr<aoc24::Dyn_bitset> dyn_bitset)
    : Bit_grid(reference_grid.size(), std::move(dyn_bitset))
    , init_desc_x_1_maps(false)
{
	add_initial_maps(reference_grid);
}
//------------------------------------------------------------------------------

void aoc24_10::Topo_grid::add_initial_maps(const aoc24::Char_grid& reference_grid)
{
	// Peaks and heads
	aoc24::Dyn_bitset& m_pks = add_map(peak_key());
	aoc24::Dyn_bitset& m_hds = add_map(head_key());
	add_map(peak_key() + temp_suffix());

	// Create descend-by-one maps
	add_map(up_key());
	add_map(ri_key());
	add_map(do_key());
	add_map(le_key());
	add_map(up_key() + temp_suffix());
	add_map(ri_key() + temp_suffix());
	add_map(do_key() + temp_suffix());
	add_map(le_key() + temp_suffix());

	size_t pos = 0;
	for (size_t row = 0; row < sz; ++row) {
		for (size_t col = 0; col < sz; ++col) {
			char c = reference_grid.char_at(col, row);
			if (c == '0') {
				m_hds.set(pos);
			} else {
				if (c == '9') {
					m_pks.set(pos);
				}
				set_desc_x_1(col, row, reference_grid);
			}
			++pos;
		}
	}
}

//------------------------------------------------------------------------------

aoc24::Dyn_bitset& aoc24_10::Topo_grid::desc_x_1_map(const Direction dir,
                                                     const bool temp)
{
	if (!init_desc_x_1_maps) {
		cache_desc_x_1_map_ptrs();
	}

	switch (dir) {
	case Direction::Up:
		return (!temp) ? *up_map : *up_map_temp;
	case Direction::Down:
		return (!temp) ? *do_map : *do_map_temp;
	case Direction::Right:
		return (!temp) ? *ri_map : *ri_map_temp;
	case Direction::Left:
		return (!temp) ? *le_map : *le_map_temp;
		break;
	}
	throw std::invalid_argument("Cannot convert direction of value "
	                            + std::to_string(static_cast<int>(dir)));
}

void aoc24_10::Topo_grid::cache_desc_x_1_map_ptrs()
{
	// Cache pointers to descend-by-one maps
	up_map = &get_map(up_key());
	ri_map = &get_map(ri_key());
	do_map = &get_map(do_key());
	le_map = &get_map(le_key());
	up_map_temp = &get_map(up_key() + temp_suffix());
	ri_map_temp = &get_map(ri_key() + temp_suffix());
	do_map_temp = &get_map(do_key() + temp_suffix());
	le_map_temp = &get_map(le_key() + temp_suffix());
	init_desc_x_1_maps = true;
}

void aoc24_10::Topo_grid::set_desc_x_1(const size_t x,
                                       const size_t y,
                                       const aoc24::Char_grid& reference_grid)
{
	int mid_alt = reference_grid.char_at(x, y) - '0';
	for (int i = 0; i < 4; ++i) {
		Direction dir = static_cast<Direction>(i);
		Offset offs = dir_to_offset(dir);
		if (valid_xy(x + offs.x, y + offs.y)) {
			int nbour_alt = reference_grid.char_at(x + offs.x, y + offs.y)
			                - '0';
			if (mid_alt - nbour_alt == 1) {
				desc_x_1_map(dir).set(
				    xy_to_idx(static_cast<int>(x), static_cast<int>(y), true));
			}
		}
	}
}
//------------------------------------------------------------------------------

std::pair<size_t, size_t> aoc24_10::Topo_grid::prep_temp_maps(size_t head_pos)
{
	// Generate range mask
	aoc24::XY c = idx_to_xy(head_pos, true);
	auto mask_se = gen_trail_mask(c.x, c.y, true);

	// Reset temp masks
	auto& m = get_map(peak_key() + temp_suffix());
	m.reset();
	up_map_temp->reset();
	ri_map_temp->reset();
	do_map_temp->reset();
	le_map_temp->reset();

	// Copy peaks
	m |= get_map(peak_key());
	*up_map_temp |= *up_map;
	*ri_map_temp |= *ri_map;
	*do_map_temp |= *do_map;
	*le_map_temp |= *le_map;

	// Cut away out-of-range peaks
	m.subtract(*mask_);
	up_map_temp->subtract(*mask_);
	ri_map_temp->subtract(*mask_);
	do_map_temp->subtract(*mask_);
	le_map_temp->subtract(*mask_);

	return {mask_se.first, mask_se.second};
}

void aoc24_10::Topo_grid::find_and_store_trails(
    std::multimap<size_t, size_t>& trails)
{
	if (trails.size() > 0) {
		trails.clear();
	}

	// Get all trail heads and shuffle
	std::vector<size_t> hd_v{all_heads()};
	if (hd_v.empty()) {
		return;
	}
	auto rng = std::default_random_engine{};
	std::shuffle(std::begin(hd_v), std::end(hd_v), rng);

	// Find peaks
	for (size_t hd : hd_v) {
		// Get peaks already found for current head
		auto excl_r = trails.equal_range(hd);
		std::vector<int> exclude;
		for (auto it = excl_r.first; it != excl_r.second; ++it) {
			exclude.push_back(static_cast<int>(it->second));
		}
		std::sort(exclude.begin(), exclude.end());

		auto se_pair = prep_temp_maps(hd);
		size_t start = se_pair.first;
		size_t end = se_pair.second;

		// Find peaks in range of current head
		std::vector<size_t> pk_v{peaks_in_range(hd, start, end, exclude)};

		// Recursively try to connect trails
		for (size_t pk : pk_v) {
			if (connect_trail(pk, hd)) {
				trails.insert({hd, pk});
			}
		}
	}
}

std::vector<size_t> aoc24_10::Topo_grid::all_heads()
{
	const auto& m = get_map(head_key());

	if (m.count() == 0) {
		return {};
	}

	std::vector<size_t> pos_v;
	for (size_t pos = 0; pos < m.size(); ++pos) {
		if (m.test(pos)) {
			pos_v.push_back(pos);
		}
	}
	return pos_v;
}

std::vector<size_t> aoc24_10::Topo_grid::peaks_in_range(
    size_t pos, size_t start, size_t end, const std::vector<int>& exclude_v)
{
	auto& m = get_map(peak_key() + temp_suffix());

	if (m.count() == 0) {
		return {};
	}

	// Store peaks in vector
	std::vector<size_t> pk_v;
	auto excl = exclude_v.begin();
	for (int pos = static_cast<int>(start); pos <= end; ++pos) {
		// Check if peak should be excluded
		if (!exclude_v.empty()) {
			while (*excl < pos) {
				++excl;
			}
			if (*excl == pos) {
				++excl;
				continue;
			}
		}

		// Add peak
		if (m.test(pos)) {
			pk_v.push_back(pos);
		}
	}

	return pk_v;
}

bool aoc24_10::Topo_grid::connect_trail(size_t current_pos,
                                        size_t head_pos,
                                        Direction dir,
                                        size_t step)
{
	// DEBUG
	if (current_pos == 6) {
		int test = 0;
	}
	if (step >= 9 || current_pos == head_pos) {
		return step == 9;
	}

	Distance dist = distance(idx_to_xy(current_pos), idx_to_xy(head_pos));
	if (dist.x + dist.y > (9 - step)) {
		return false;
	}

	if (dir != Direction::Down && current_pos >= sz
	    && up_map_temp->test(current_pos)) {
		if (connect_trail(current_pos - sz, head_pos, Direction::Up, step + 1)) {
			return true;
		}
	}
	if (dir != Direction::Left && current_pos < sz * sz
	    && ri_map_temp->test(current_pos)) {
		if (connect_trail(current_pos + 1,
		                  head_pos,
		                  Direction::Right,
		                  step + 1)) {
			return true;
		}
	}
	if (dir != Direction::Up && current_pos < sz * (sz - 1)
	    && do_map_temp->test(current_pos)) {
		if (connect_trail(current_pos + sz,
		                  head_pos,
		                  Direction::Down,
		                  step + 1)) {
			return true;
		}
	}
	if (dir != Direction::Right && current_pos > 0
	    && le_map_temp->test(current_pos)) {
		if (connect_trail(current_pos - 1, head_pos, Direction::Left, step + 1)) {
			return true;
		}
	}
	return false;
}

//------------------------------------------------------------------------------

std::pair<size_t, size_t> aoc24_10::Topo_grid::gen_trail_mask(const size_t x,
                                                              const size_t y,
                                                              const bool invert)
{
	if (!valid_xy(x, y)) {
		throw std::invalid_argument(
		    "Cannot generate trail mask, position invalid: ("
		    + std::to_string(x) + ", " + std::to_string(y) + ")");
	}

	if (sz < min_size_for_trail_mask) {
		//Not worth it
		if (!invert) {
			mask_->set();
		} else {
			mask_->reset();
		}
		return {0, sz * sz - 1};
	}

	if (!invert) {
		mask_->reset();
	} else {
		mask_->set();
	}
	int start_x = static_cast<int>(x) - 9;
	int start_y = static_cast<int>(y) - 9;

	size_t min_pos = nopos;
	size_t max_pos = nopos;

	for (int row = 0; row < 19; ++row) {
		const int this_y = start_y + row;
		if (this_y < 0 || this_y >= sz) {
			continue;
		}
		const int half_w = 1 + (row < 10 ? (row % 10) : (8 - (row % 10)));
		for (int col = 0; col < half_w * 2 - 1; ++col) {
			const int this_x = start_x + (10 - half_w) + col;
			if (this_x >= 0 && this_x < static_cast<int>(sz)) {
				size_t pos = this_y * sz + this_x;
				mask_->set(pos, !invert);
				if (min_pos == nopos) {
					min_pos = pos;
				}
				max_pos = pos;
			}
		}
	}

	if (min_pos == nopos || max_pos == nopos) {
		throw std::runtime_error(
		    "Could not generate mask: The registered min "
		    "and/or max positional values are out of bounds");
	}

	return {min_pos, max_pos};
}

void aoc24_10::Topo_grid::find_trails()
{
	find_and_store_trails(head_peak_pairs);
}

//------------------------------------------------------------------------------

aoc24_10::Distance aoc24_10::Topo_grid::distance(const aoc24::XY& a,
                                                 const aoc24::XY& b) const
{
	return {std::abs(static_cast<int>(a.x) - static_cast<int>(b.x)),
	        std::abs(static_cast<int>(a.y) - static_cast<int>(b.y))};
}

aoc24::XY aoc24_10::Topo_grid::to_xy(int x, int y, const bool skip_check)
{
	if (!skip_check && is_oob(x, y)) {
		throw std::invalid_argument("Cannot convert x=" + std::to_string(x)
		                            + " and y=" + std::to_string(y)
		                            + " to a valid XY of map with size "
		                            + std::to_string(sz));
	}
	return {static_cast<size_t>(x), static_cast<size_t>(y)};
}

aoc24_10::Offset aoc24_10::Topo_grid::dir_to_offset(const Direction dir)
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
