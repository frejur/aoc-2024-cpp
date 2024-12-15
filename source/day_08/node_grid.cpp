#include "node_grid.h"

aoc24_08::Node_grid::Node_grid(const aoc24::Char_grid& reference_grid,
                               std::unique_ptr<aoc24::Dyn_bitset> dyn_bitset)
    : Bit_grid(reference_grid.size(), std::move(dyn_bitset))
{
	for (char f : reference_grid.unique_chars()) {
		if (f == '.') {
			continue;
		}
		freq += f;
		new_freq(f, reference_grid);
	}
	add_map(all_antinodes_key());
}

void aoc24_08::Node_grid::new_freq(char f,
                                   const aoc24::Char_grid& reference_grid)
{
	const std::string n_antna = new_antenna_name(f);
	const std::string n_anode = new_antinode_name(f);
	antenna_cache[f] = n_antna;
	antinode_cache[f] = n_anode;
	add_map(n_antna);
	add_map(n_anode);

	aoc24::XY xy{0, 0};
	while (xy = reference_grid.find_char(f, xy.x, xy.y), xy != aoc24::XY::oob) {
		set(n_antna, xy);
		reference_grid.next(xy);
	}
}

//------------------------------------------------------------------------------

void aoc24_08::Node_grid::mark_tile(const aoc24::XY& pos,
                                    const std::string& map_key,
                                    const bool bit)
{
	if (bit) {
		set(map_key, pos);
	} else {
		reset(map_key, pos);
	}
}

void aoc24_08::Node_grid::mark_antinodes(bool use_resonant_harmonics)
{
	reset(all_antinodes_key());

	for (char f : freq) {
		bool has_pairs = false;
		for (auto& p : all_atna_pairs(f)) {
			has_pairs = true;
			mark_anod(f, p.first, p.second, use_resonant_harmonics);
		}
		if (has_pairs) {
			combine_maps(all_antinodes_key(), antinode_name(f));
			if (use_resonant_harmonics) {
				combine_maps(all_antinodes_key(), antenna_name(f));
			}
		}
	}
}

//------------------------------------------------------------------------------

int aoc24_08::Node_grid::number_of_antinode_tiles() const
{
	return count(all_antinodes_key());
}

//------------------------------------------------------------------------------

std::vector<aoc24::XY> aoc24_08::Node_grid::all_atna_xy_v(char f) const
{
	std::vector<aoc24::XY> xy_v;
	aoc24::XY xy{0, 0};
	while (xy = find_bit(antenna_name(f), 1, xy.x, xy.y), xy != aoc24::XY::oob) {
		xy_v.emplace_back(xy);
		next(xy);
	}
	return xy_v;
}

void aoc24_08::Node_grid::mark_anod(char frequency,
                                    const aoc24::XY& atna_a,
                                    const aoc24::XY& atna_b,
                                    bool use_resonant_harmonics)
{
	Distance dist = distance(atna_a, atna_b);
	if (dist.x > sz / 2 || dist.y > sz / 2) {
		return;
	}
	bool a_x_is_gr = (atna_a.x > atna_b.x);
	bool a_y_is_gr = (atna_a.y > atna_b.y);
	int offs_x = (a_x_is_gr) ? dist.x : -dist.x;
	int offs_y = (a_y_is_gr) ? dist.y : -dist.y;
	int new_x = static_cast<int>(atna_a.x) + offs_x;
	int new_y = static_cast<int>(atna_a.y) + offs_y;
	while (valid_xy(new_x, new_y)) {
		set(antinode_name(frequency), to_xy(new_x, new_y));
		if (!use_resonant_harmonics) {
			break;
		}
		new_x += offs_x;
		new_y += offs_y;
	}

	offs_x *= -1;
	offs_y *= -1;
	new_x = atna_b.x + offs_x;
	new_y = atna_b.y + offs_y;
	while (valid_xy(new_x, new_y)) {
		set(antinode_name(frequency), to_xy(new_x, new_y));
		if (!use_resonant_harmonics) {
			break;
		}
		new_x += offs_x;
		new_y += offs_y;
	}
}

std::vector<aoc24_08::Node_grid::XY_pair> aoc24_08::Node_grid::all_atna_pairs(
    char f) const
{
	std::vector<aoc24::XY> xy_v = all_atna_xy_v(f);
	if (xy_v.size() < 2) {
		return {};
	}

	std::vector<XY_pair> pairs;
	for (size_t l = 0; l < xy_v.size(); ++l) {
		for (size_t r = l + 1; r < xy_v.size(); ++r) {
			pairs.emplace_back(XY_pair{xy_v[l], xy_v[r]});
		}
	}
	return pairs;
}

//------------------------------------------------------------------------------

aoc24_08::Distance aoc24_08::Node_grid::distance(const aoc24::XY& a,
                                                 const aoc24::XY& b) const
{
	return {std::abs(static_cast<int>(a.x) - static_cast<int>(b.x)),
	        std::abs(static_cast<int>(a.y) - static_cast<int>(b.y))};
}

aoc24::XY aoc24_08::Node_grid::to_xy(int x, int y, const bool skip_check)
{
	if (!skip_check && is_oob(x, y)) {
		throw std::invalid_argument("Cannot convert x=" + std::to_string(x)
		                            + " and y=" + std::to_string(y)
		                            + " to a valid XY of map with size "
		                            + std::to_string(sz));
	}
	return {static_cast<size_t>(x), static_cast<size_t>(y)};
}

//------------------------------------------------------------------------------

aoc24_08::Node_grid::Con_str& aoc24_08::Node_grid::antenna_name(char f) const
{
	return antenna_cache.at(f);
}

aoc24_08::Node_grid::Con_str& aoc24_08::Node_grid::antinode_name(char f) const
{
	return antinode_cache.at(f);
}

std::string aoc24_08::Node_grid::new_antenna_name(char f) const
{
	return frequency_prefix() + std::string{f} + antenna_suffix();
}

std::string aoc24_08::Node_grid::new_antinode_name(char f) const
{
	return frequency_prefix() + std::string{f} + antinode_suffix();
}
