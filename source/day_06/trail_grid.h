#ifndef TRAIL_GRID_H
#define TRAIL_GRID_H
#include "../lib/bit_grid.h"
#include "keys.h"

namespace aoc24_06 {
class Trail_grid : public aoc24::Bit_grid
{
public:
	Trail_grid(const aoc24::Char_grid& reference_grid,
	           std::unique_ptr<aoc24::Dyn_bitset> dyn_bitset);
	void mark_tile(const aoc24::XY& pos,
	               const std::string& map_key = trail_key(),
	               const bool bit = true);
	void mark_tiles(const aoc24::XY& a,
	                const aoc24::XY& b,
	                const std::string& map_key = trail_key(),
	                const bool bit = true);
	void reset_trail();

	void print_trail(std::ostream& ostr = std::cout);
	int number_of_marked_tiles() const;
	int number_of_new_obstructions() const;

	// Keys
	using Con_str = const std::string;
	static constexpr char obstruction{'#'};
	static Con_str& obstruction_key() { return keys::obstruction; }
	static Con_str& trail_key() { return keys::trail; }
	static Con_str& new_obstruction_key() { return keys::new_obstruction; }
	static Con_str& up_key() { return keys::up; }
	static Con_str& right_key() { return keys::right; }
	static Con_str& down_key() { return keys::down; }
	static Con_str& left_key() { return keys::left; }
	static Con_str& future_key_suffix() { return keys::future_suffix; }
};
} // namespace aoc24_06

#endif // TRAIL_GRID_H
