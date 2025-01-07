#ifndef GRID_H
#define GRID_H
#include <string>
#include <vector>

namespace aoc24 {

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

//------------------------------------------------------------------------------

class Grid
{
public:
	void previous(XY& xy) const;
	void next(XY& xy) const;
	size_t size() const { return sz; }
	bool is_oob(XY xy) const { return !valid_xy(xy.x, xy.y); };
	bool is_oob(int x, int y) const { return !valid_xy(x, y); };

protected:
	Grid(size_t grid_size);
	Grid(size_t grid_size_x, size_t grid_size_y);

	mutable bool sz_checked;
	const size_t sz;
	const size_t sz_y;
	bool valid_xy(size_t x, size_t y, bool skip_check = false) const;
	virtual void check_size() const = 0;
};

//------------------------------------------------------------------------------

class Char_grid : public Grid
{
public:
	Char_grid(char c, size_t grid_size);
	Char_grid(const std::string& file_path, size_t grid_size);
	Char_grid(const std::vector<std::string>& text, size_t grid_size);
	char char_at(size_t x, size_t y) const;
	XY find_char(char c, size_t start_x, size_t start_y) const;
	XY find_char(
	    char c, size_t start_x, size_t start_y, int offs_x, int offs_y) const;
	static const char nochar = '\0';
	std::string unique_chars() const;

protected:
	std::vector<std::string> g_;
	char ch_at(size_t x, size_t y, bool skip_check = false) const;
	virtual void check_size() const override;
};


} // namespace aoc24

#endif // GRID_H
