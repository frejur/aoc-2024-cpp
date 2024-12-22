#ifndef GRP_GRID_H
#define GRP_GRID_H
#include "../lib/bit_grid.h"
#include "keys.h"

namespace aoc24_12 {
struct Vec2d
{
	Vec2d(int xx, int yy)
	    : x(xx)
	    , y(yy)
	{}
	int x;
	int y;
};

class Group_grid : public aoc24::Bit_grid
{
public:
	Group_grid(const aoc24::Char_grid& reference_grid,
	           std::unique_ptr<aoc24::Dyn_bitset> dyn_bitset);

	void mark_tile(const aoc24::XY& pos,
	               const std::string& map_key,
	               const bool bit = true);
	long long calculate_fence_cost(bool apply_discount = false);

	enum class Direction { Up = 0, Right, Down, Left };

	// Keys
	using Con_str = const std::string;
	static Con_str& up_key() { return keys::fenced_up_key; }
	static Con_str& ri_key() { return keys::fenced_right_key; }
	static Con_str& do_key() { return keys::fenced_down_key; }
	static Con_str& le_key() { return keys::fenced_left_key; }
	static Con_str& temp_suffix() { return keys::fenced_temp_suffix; }

	static constexpr size_t nopos = -1;

private:
	void add_initial_maps(const aoc24::Char_grid& reference_grid);

	// Pointers to group maps
	std::unordered_map<char, aoc24::Dyn_bitset*> grp_;
	std::unordered_multimap<char, aoc24::Dyn_bitset*> sub_grp_;

	// Pointers to fenced tile maps
	aoc24::Dyn_bitset* up_map_;
	aoc24::Dyn_bitset* ri_map_;
	aoc24::Dyn_bitset* do_map_;
	aoc24::Dyn_bitset* le_map_;
	aoc24::Dyn_bitset* up_map_temp_;
	aoc24::Dyn_bitset* ri_map_temp_;
	aoc24::Dyn_bitset* do_map_temp_;
	aoc24::Dyn_bitset* le_map_temp_;

	// Fenced tile helpers
	inline aoc24::Dyn_bitset& fenced_tile_map(const Direction dir,
	                                          const bool temp = false);
	void cache_fenced_tile_map_ptrs();
	void set_fenced_tiles(const size_t x,
	                      const size_t y,
	                      const aoc24::Char_grid& reference_grid);
	int perim(aoc24::Dyn_bitset& map);
	int sides(aoc24::Dyn_bitset& map);

	// Prepare temporary maps
	void prep_temp_maps(const aoc24::Dyn_bitset& group_map);

	// Various helpers
	aoc24::XY to_xy(int x, int y, const bool skip_check = true);
	Vec2d dir_to_offset(const Direction dir);
};
} // namespace aoc24_12

#endif // GRP_GRID_H
