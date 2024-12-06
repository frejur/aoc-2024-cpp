#ifndef DAY_04_H
#define DAY_04_H
#include <string>
#include <vector>

namespace aoc24_04 {

std::vector<std::string> read_input_into_strings(const std::string& file_path,
                                                 size_t string_size);

//------------------------------------------------------------------------------
// Part 1 (definitions in `part_01.cpp`)

class Grid; //Fwd. declare

int count_xmas(const Grid& g);

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

enum class Direction {
	North = 0,
	North_east,
	East,
	South_east,
	South,
	South_west,
	West,
	North_west
};
struct Offset
{
	int x;
	int y;
};

inline Offset dir_to_xy(Direction dir);

bool has_neighbour_str(
    const Grid& grid, size_t x, size_t y, const std::string& s, Direction dir);

//------------------------------------------------------------------------------
// Part 2 (definitions in `part_02.cpp`)

} // namespace aoc24_04

#endif // DAY_04_H
