#ifndef TOPO_GRID_H
#define TOPO_GRID_H
#include "../lib/bit_grid.h"
#include "keys.h"
#include <map>

namespace aoc24_10 {
struct Distance
{
	Distance(int xx, int yy)
	    : x(xx)
	    , y(yy)
	{}
	int x;
	int y;
};

struct Offset
{
	Offset(int xx, int yy)
	    : x(xx)
	    , y(yy)
	{}
	int x;
	int y;
};

class Topo_grid : public aoc24::Bit_grid
{
public:
	Topo_grid(const aoc24::Char_grid& reference_grid,
	          std::unique_ptr<aoc24::Dyn_bitset> dyn_bitset);

	void mark_tile(const aoc24::XY& pos,
	               const std::string& map_key,
	               const bool bit = true);

	enum class Direction { No_direction = -1, Up = 0, Down, Right, Left };

	void find_trails();
	int combined_trailhead_score() const { return head_peak_pairs.size(); };

	// Keys
	using Con_str = const std::string;
	static Con_str& peak_key() { return keys::peak_key; }
	static Con_str& head_key() { return keys::head_key; }
	static Con_str& up_key() { return keys::descending_up_key; }
	static Con_str& ri_key() { return keys::descending_right_key; }
	static Con_str& do_key() { return keys::descending_down_key; }
	static Con_str& le_key() { return keys::descending_left_key; }
	static Con_str& temp_suffix() { return keys::descending_temp_suffix; }

	static constexpr size_t nopos = -1;

private:
	bool init_desc_x_1_maps;
	std::multimap<size_t, size_t> head_peak_pairs;

	void add_initial_maps(const aoc24::Char_grid& reference_grid);

	// Pointers to descend-by-one maps
	aoc24::Dyn_bitset* up_map;
	aoc24::Dyn_bitset* ri_map;
	aoc24::Dyn_bitset* do_map;
	aoc24::Dyn_bitset* le_map;
	aoc24::Dyn_bitset* up_map_temp;
	aoc24::Dyn_bitset* ri_map_temp;
	aoc24::Dyn_bitset* do_map_temp;
	aoc24::Dyn_bitset* le_map_temp;

	// Descend-by-one helpers
	inline aoc24::Dyn_bitset& desc_x_1_map(const Direction dir,
	                                       const bool temp = false);
	void cache_desc_x_1_map_ptrs();
	void set_desc_x_1(const size_t x,
	                  const size_t y,
	                  const aoc24::Char_grid& reference_grid);

	// Generate range mask and prep temporary maps
	std::pair<size_t, size_t> prep_temp_maps(size_t head_pos);

	// Find trails
	void find_and_store_trails(std::multimap<size_t, size_t>& trails);
	std::vector<size_t> all_heads();
	std::vector<size_t> peaks_in_range(size_t pos,
	                                   size_t start,
	                                   size_t end,
	                                   const std::vector<int>& exclude_v);
	bool connect_trail(size_t current_pos,
	                   size_t head_pos,
	                   Direction dir = Direction::No_direction,
	                   size_t step = 0);

	// Draw diamond mask eminating from the center, representing the area
	// where a valid trail peak may be located
	std::pair<size_t, size_t> gen_trail_mask(const size_t x,
	                                         const size_t y,
	                                         const bool invert = false);

	// Various helpers
	inline Distance distance(const aoc24::XY& a, const aoc24::XY& b) const;
	aoc24::XY to_xy(int x, int y, const bool skip_check = true);
	Offset dir_to_offset(const Direction dir);
};
} // namespace aoc24_10

#endif // TOPO_GRID_H
