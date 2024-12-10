#ifndef BIT_GRID_H
#define BIT_GRID_H

#include "dynbs.h"
#include "grid.h"
#include <iostream>
#include <unordered_map>

namespace aoc24 {

class Bit_grid : public Grid
{
public:
	Bit_grid(size_t grid_size,
	         const std::string& name,
	         std::unique_ptr<Dyn_bitset> dyn_bitset);

	XY find_bit(bool bit, size_t x, size_t y) const;
	XY find_bit(const std::string& map_key, bool bit, size_t x, size_t y) const;
	XY find_bit(bool bit, size_t x, size_t y, int offs_z, int offs_y) const;
	XY find_bit(const std::string& map_key,
	            bool bit,
	            size_t x,
	            size_t y,
	            int offs_z,
	            int offs_y) const;
	int bit_at(size_t x, size_t y) const;
	int bit_at(const std::string& map_key, size_t x, size_t y) const;
	void set();
	void set(const std::string& map_key);
	void set(aoc24::XY pos);
	void set(const std::string& map_key, aoc24::XY pos);
	void reset();
	void reset(const std::string& map_key);
	void reset(aoc24::XY pos);
	void reset(const std::string& map_key, aoc24::XY pos);
	void set(aoc24::XY a, aoc24::XY b){};
	void set(const std::string& map_key, aoc24::XY a, aoc24::XY b){};

	void add_map(const std::string& name);
	void add_map(const std::string& name, std::unique_ptr<Dyn_bitset>);

	void mask_row(int row, int leading_0, int trailing_0);
	void mask_col(int col, int leading_0, int trailing_0);
	void apply_mask(bool subtract = false);
	void apply_mask(const std::string& key, bool subtract = false);
	void apply_mask_to_all(bool subtract);

	int count() const;
	int count(const std::string& map_key) const;
	void print(std::ostream& ostr = std::cout);
	void print(const std::string& map_key, std::ostream& ostr = std::cout);

	static constexpr int nobit = -1;

protected:
	std::unordered_map<std::string, std::unique_ptr<Dyn_bitset>> map_;
	std::unique_ptr<Dyn_bitset> mask_;

	int b_at(const Dyn_bitset& map,
	         size_t x,
	         size_t y,
	         bool skip_check = false) const;

	void check_size() const;
	XY idx_to_xy(size_t idx, bool skip_check = false) const;
	size_t xy_to_idx(int x, int y, bool skip_check = false) const;
	std::string valid_key(const std::string& n);
	Dyn_bitset& get_only_map() const;
	Dyn_bitset& get_map(const std::string& key) const;

	void check_mask_op(int row_or_col, int leading_0, int trailing_0) const;

	XY find_bit(const Dyn_bitset& map, bool bit, size_t x, size_t y) const;
	XY find_bit(const Dyn_bitset& map,
	            bool bit,
	            size_t x,
	            size_t y,
	            int offs_z,
	            int offs_y) const;
};

} // namespace aoc24

#endif // BIT_GRID_H
