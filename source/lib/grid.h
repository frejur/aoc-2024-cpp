#ifndef GRID_H
#define GRID_H
#include <string>
#include <vector>

namespace aoc24 {
class Grid; //Fwd. declare

struct XY
{
	XY(size_t xx, size_t yy)
	    : x(xx)
	    , y(yy)
	{}
	size_t x;
	size_t y;

	static const XY oob;
	bool operator==(const XY& a) const;
	bool operator!=(const XY& a) const;

private:
	static constexpr size_t noxy = -1;
};

class Grid
{
public:
	Grid(const std::string& file_path, size_t grid_size);
	char char_at(size_t x, size_t y) const;
	XY find_char(char c, size_t start_x, size_t start_y) const;
	void previous(XY& xy) const;
	void next(XY& xy) const;
	static const char nochar = '\0';

private:
	size_t sz;
	const std::vector<std::string> g_;
	bool valid_xy(size_t x, size_t y) const;
	char ch_at(size_t x, size_t y, bool skip_check = false) const;
};
} // namespace aoc24

#endif // GRID_H
