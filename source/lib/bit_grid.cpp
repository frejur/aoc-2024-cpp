#include "bit_grid.h"
#include <sstream>

aoc24::Bit_grid::Bit_grid(size_t grid_size,
                          const std::string& name,
                          std::unique_ptr<Dyn_bitset> dyn_bitset)
    : Grid(grid_size)
{
	add_map(valid_key(name), std::move(dyn_bitset));
	auto& m = get_only_map();
	mask_ = m.new_copy();
}

aoc24::XY aoc24::Bit_grid::find_bit(bool bit,
                                    size_t start_x,
                                    size_t start_y) const
{
	auto& m = get_only_map();

	size_t total_sz{sz * sz};
	if ((bit && m.count() == 0) || (!bit && m.count() == total_sz)
	    || !valid_xy(start_x, start_y)) {
		return XY::oob;
	}

	size_t start_i = total_sz - start_y * sz - start_x;
	for (size_t i = start_i; i > 0; --i) {
		if (m.test(i - 1) == bit) {
			return idx_to_xy(i, true);
		}
	}
	return XY::oob;
}

aoc24::XY aoc24::Bit_grid::find_bit(
    bool bit, size_t start_x, size_t start_y, int offs_x, int offs_y) const
{
	return find_bit(get_only_map(), bit, start_x, start_y, offs_x, offs_y);
}

aoc24::XY aoc24::Bit_grid::find_bit(const std::string& map_key,
                                    bool bit,
                                    size_t start_x,
                                    size_t start_y,
                                    int offs_x,
                                    int offs_y) const
{
	return find_bit(get_map(map_key), bit, start_x, start_y, offs_x, offs_y);
}

int aoc24::Bit_grid::bit_at(size_t x, size_t y) const
{
	return b_at(get_only_map(), x, y);
}

int aoc24::Bit_grid::bit_at(const std::string& map_key, size_t x, size_t y) const
{
	return b_at(get_map(map_key), x, y);
}
void aoc24::Bit_grid::set()
{
	auto& m = get_only_map();
	m.set();
}

void aoc24::Bit_grid::set(const std::string& map_key)
{
	auto& m = get_map(map_key);
	m.set();
}

void aoc24::Bit_grid::reset()
{
	auto& m = get_only_map();
	m.reset();
}

void aoc24::Bit_grid::reset(const std::string& map_key)
{
	auto& m = get_map(map_key);
	m.reset();
}

void aoc24::Bit_grid::set(XY pos)
{
	get_only_map().set(xy_to_idx(pos.x, pos.y));
}

void aoc24::Bit_grid::set(const std::string& map_key, XY pos)
{
	get_map(map_key).set(xy_to_idx(pos.x, pos.y));
}

void aoc24::Bit_grid::reset(XY pos)
{
	get_only_map().reset(xy_to_idx(pos.x, pos.y));
}

void aoc24::Bit_grid::reset(const std::string& map_key, XY pos)
{
	get_map(map_key).reset(xy_to_idx(pos.x, pos.y));
}

void aoc24::Bit_grid::add_map(const std::string& name)
{
	if (map_.empty()) {
		throw std::logic_error(
		    "Cannot add map, expected at least one existing map to copy");
	}
	if (map_.find(valid_key(name)) == map_.end()) {
		map_.insert({name, map_.begin()->second->new_copy()});
		map_[name]->reset();
		sz_checked = true;
	} else {
		throw std::logic_error("Cannot add map, duplicate key: " + name);
	}
}

void aoc24::Bit_grid::add_map(const std::string& name,
                              std::unique_ptr<Dyn_bitset> dyn_bitset)
{
	if (map_.find(valid_key(name)) == map_.end()) {
		sz_checked = false;
		if (sz * sz != dyn_bitset->size()) {
			throw std::logic_error(
			    "Cannot add map of size "
			    + std::to_string((int) sqrt(dyn_bitset->size())) + ", expected "
			    + std::to_string(sz));
		}
		sz_checked = true;
		map_.insert({name, std::move(dyn_bitset)});
		map_[name]->reset();
	} else {
		throw std::logic_error("Cannot add map, duplicate key: " + name);
	}
}

int aoc24::Bit_grid::count() const
{
	return get_only_map().count();
}

int aoc24::Bit_grid::count(const std::string& map_key) const
{
	return get_map(map_key).count();
}

void aoc24::Bit_grid::print(std::ostream& ostr)
{
	auto& m = get_only_map();
	size_t i = 0;
	for (size_t y = 0; y < sz; ++y) {
		for (size_t x = 0; x < sz; ++x) {
			ostr << (x == 0 ? "" : " ") << (m.test(i) ? '1' : '0');
			++i;
		}
		ostr << '\n';
	}
}
void aoc24::Bit_grid::print(const std::string& map_key, std::ostream& ostr)
{
	auto& m = get_map(map_key);
	size_t i = 0;
	for (size_t y = 0; y < sz; ++y) {
		for (size_t x = 0; x < sz; ++x) {
			ostr << (x == 0 ? "" : " ") << (m.test(i) ? '1' : '0');
			++i;
		}
		ostr << '\n';
	}
}

int aoc24::Bit_grid::b_at(const Dyn_bitset& map,
                          size_t x,
                          size_t y,
                          bool skip_check) const
{
	if (!valid_xy(x, y, skip_check)) {
		return nobit;
	}
	return map.test(xy_to_idx(x, y, true));
}

void aoc24::Bit_grid::mask_row(int row, int leading_0, int trailing_0)
{
	check_mask_op(row, leading_0, trailing_0);
	mask_->reset();
	for (size_t i = 0; i < sz; ++i) {
		mask_->set(i);
	}
	*mask_ >>= (leading_0 + trailing_0);
	*mask_ <<= (row * sz + leading_0);
}

void aoc24::Bit_grid::mask_col(int col, int leading_0, int trailing_0)
{
	check_mask_op(col, leading_0, trailing_0);
	mask_->reset();
	for (int i = 0; i < sz - (leading_0 + trailing_0); ++i) {
		mask_->set((leading_0 + i) * sz + col);
	}
}

void aoc24::Bit_grid::apply_mask(bool subtract)
{
	auto& m = get_only_map();
	if (subtract) {
		m.subtract(*mask_);
	} else {
		m |= *mask_;
	}
}

void aoc24::Bit_grid::apply_mask(const std::string& key, bool subtract)
{
	auto& m = get_map(key);
	if (subtract) {
		m.subtract(*mask_);
	} else {
		m |= *mask_;
	}
}

void aoc24::Bit_grid::apply_mask_to_all(bool subtract)
{
	if (map_.empty()) {
		throw std::runtime_error(
		    "Can't apply mask to all maps, no maps were found");
	}
	for (auto& m_ptr : map_) {
		if (subtract) {
			m_ptr.second->subtract(*mask_);
		} else {
			*m_ptr.second |= *mask_;
		}
	}
}

void aoc24::Bit_grid::check_size() const
{
	for (const auto& map : map_) {
		if (map.second->size() != sz) {
			throw std::runtime_error("The size of the container is "
			                         + std::to_string(map.second->size())
			                         + " but was expecting "
			                         + std::to_string(sz));
		}
	}
}

aoc24::XY aoc24::Bit_grid::idx_to_xy(size_t idx, bool skip_check) const
{
	if (!skip_check && idx >= sz * sz) {
		throw std::runtime_error("Invalid index: " + std::to_string(idx)
		                         + ", size of grid is " + std::to_string(sz)
		                         + "x" + std::to_string(sz));
	}
	return {idx % sz, idx / sz};
}

size_t aoc24::Bit_grid::xy_to_idx(int x, int y, bool skip_check) const
{
	if (!skip_check && !valid_xy(x, y)) {
		throw std::runtime_error("Invalid position: (" + std::to_string(x)
		                         + ", " + std::to_string(y)
		                         + "), size of grid is " + std::to_string(sz)
		                         + "x" + std::to_string(sz));
	}
	return y * sz + x;
}

std::string aoc24::Bit_grid::valid_key(const std::string& n)
{
	if (n.empty()) {
		throw std::invalid_argument("Key cannot be empty");
	}

	static const int max{24};
	if (n.size() > max) {
		throw std::invalid_argument("Key can only contain a max. of "
		                            + std::to_string(max) + " chars, got "
		                            + std::to_string(n.size()));
	}

	if (!isalpha(n.front())) {
		throw std::invalid_argument("Key must start with a letter");
	}

	for (size_t i = 1; i < n.size(); ++i) {
		char c = n[i];
		if (!isdigit(c) && !islower(c) && c != '_') {
			throw std::invalid_argument("Key must start with a letter, and the "
			                            "remaining characters may only be "
			                            "lowercase characters, "
			                            "digits or underscores");
		}
	}

	return n;
}

aoc24::Dyn_bitset& aoc24::Bit_grid::get_only_map() const
{
	if (map_.size() != 1) {
		throw std::logic_error(
		    "Cannot get map, expected exactly one map but found "
		    + std::to_string(map_.size()));
	}
	return *(map_.begin()->second);
}

aoc24::Dyn_bitset& aoc24::Bit_grid::get_map(const std::string& key) const
{
	auto m = map_.find(key);
	if (m == map_.end()) {
		throw std::logic_error("Cannot find map with key: " + key);
	}
	return *(m->second);
}

void aoc24::Bit_grid::check_mask_op(int row_or_col,
                                    int leading_0,
                                    int trailing_0) const
{
	if (map_.empty() || mask_ == nullptr) {
		throw std::logic_error(
		    "Cannot perform mask operations before at least one "
		    "map has been added");
	}
	if (row_or_col < 0 || row_or_col >= sz || leading_0 < 0 || trailing_0 < 0
	    || ((leading_0 + trailing_0) >= sz)) {
		throw std::invalid_argument(
		    "Cannot perform mask operation on row/col "
		    + std::to_string(row_or_col)
		    + " with leading_0=" + std::to_string(leading_0)
		    + " and trailing_0=" + std::to_string(trailing_0)
		    + " for the given mask of size " + std::to_string(sz));
	}
}

aoc24::XY aoc24::Bit_grid::find_bit(const Dyn_bitset& m,
                                    bool bit,
                                    size_t start_x,
                                    size_t start_y) const
{
	size_t total_sz{sz * sz};
	if ((bit && m.count() == 0) || (!bit && m.count() == total_sz)
	    || !valid_xy(start_x, start_y)) {
		return XY::oob;
	}

	size_t start_i = total_sz - start_y * sz - start_x;
	for (size_t i = start_i; i > 0; --i) {
		if (m.test(i - 1) == bit) {
			return idx_to_xy(i, true);
		}
	}
	return XY::oob;
}

aoc24::XY aoc24::Bit_grid::find_bit(const Dyn_bitset& m,
                                    bool bit,
                                    size_t start_x,
                                    size_t start_y,
                                    int offs_x,
                                    int offs_y) const
{
	size_t total_sz{sz * sz};
	if ((bit && m.count() == 0) || (!bit && m.count() == total_sz)
	    || !valid_xy(start_x, start_y)) {
		return XY::oob;
	}

	XY cursor = {start_x, start_y};
	for (;;) {
		cursor.x += offs_x;
		cursor.y += offs_y;
		if (!valid_xy(cursor.x, cursor.y)) {
			break;
		}
		if (b_at(m, cursor.x, cursor.y, true) == bit) {
			return cursor;
		}
	}
	return XY::oob;
}
