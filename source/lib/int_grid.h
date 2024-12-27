#ifndef INT_GRID_H
#define INT_GRID_H
#include "grid.h"

namespace aoc24 {

class Int_grid : public Grid
{
public:
	Int_grid(size_t grid_size_x, size_t grid_size_y);
	int value_at(size_t x, size_t y) const;
	void reset();
	void set_value(size_t x, size_t y, int val);
	XY find_value(int val, size_t start_x, size_t start_y) const;
	XY find_value(
	    int val, size_t start_x, size_t start_y, int offs_x, int offs_y) const;
	size_t width() const { return sz; }
	size_t height() const { return sz_y; }

protected:
	std::vector<std::vector<int>> g_;
	int int_at(size_t x, size_t y, bool skip_check = false) const;
	void set(size_t x, size_t y, int val, bool skip_check = false);
	virtual void check_size() const override;

private:
	size_t size() const = delete; // Hide
};

} // namespace aoc24

#endif // INT_GRID_H
