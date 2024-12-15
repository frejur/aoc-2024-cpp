#ifndef TRAIL_GRID_H
#define TRAIL_GRID_H
#include "../lib/bit_grid.h"
#include "keys.h"

namespace aoc24_08 {
struct Distance
{
	Distance(int xx, int yy)
	    : x(xx)
	    , y(yy)
	{}
	int x;
	int y;
};

class Node_grid : public aoc24::Bit_grid
{
public:
	Node_grid(const aoc24::Char_grid& reference_grid,
	          std::unique_ptr<aoc24::Dyn_bitset> dyn_bitset);
	void mark_tile(const aoc24::XY& pos,
	               const std::string& map_key,
	               const bool bit = true);

	void mark_antinodes();
	int number_of_antinode_tiles() const;

	// Keys
	using Con_str = const std::string;
	static Con_str& all_antinodes_key() { return keys::all_antinodes_key; }
	static Con_str& frequency_prefix() { return keys::frequency_prefix; }
	static Con_str& antenna_suffix() { return keys::antenna_suffix; }
	static Con_str& antinode_suffix() { return keys::antinode_suffix; }

private:
	std::string freq;

	// Keys
	inline Con_str& antenna_name(char f) const;
	inline Con_str& antinode_name(char f) const;
	inline std::string new_antenna_name(char f) const;
	inline std::string new_antinode_name(char f) const;
	std::unordered_map<char, std::string> antenna_cache;
	std::unordered_map<char, std::string> antinode_cache;

	void new_freq(char f, const aoc24::Char_grid& reference_grid);

	// Mark antinodes
	using XY_pair = std::pair<aoc24::XY, aoc24::XY>;
	std::vector<XY_pair> all_atna_pairs(char f) const;
	std::vector<aoc24::XY> all_atna_xy_v(char f) const;
	std::vector<XY_pair> atna_pairs(size_t start,
	                                const std::vector<aoc24::XY> xy_v) const;
	void mark_anod(char frequency,
	               const aoc24::XY& atna_a,
	               const aoc24::XY& atna_b);

	inline Distance distance(const aoc24::XY& a, const aoc24::XY& b) const;
	aoc24::XY to_xy(int x, int y, const bool skip_check = true);
};
} // namespace aoc24_08

#endif // TRAIL_GRID_H
