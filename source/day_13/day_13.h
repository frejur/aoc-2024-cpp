#ifndef DAY_13_H
#define DAY_13_H
#include <iostream>
#include <vector>
namespace aoc24_13 {

struct Vec2d
{
	Vec2d()
	    : x(0)
	    , y(0)
	{}
	Vec2d(long long xx, long long yy)
	    : x(xx)
	    , y(yy)
	{}
	long long x;
	long long y;
};

struct Machine_buttons_and_prize
{
	Machine_buttons_and_prize()
	    : button_a{0, 0}
	    , button_b{0, 0}
	    , prize{0, 0}
	{}
	Vec2d button_a;
	Vec2d button_b;
	Vec2d prize;
};

std::istream& operator>>(std::istream& iss, Vec2d& vec);
std::istream& operator>>(std::istream& iss, Machine_buttons_and_prize& m);

std::vector<Machine_buttons_and_prize> read_machine_buttons_and_prize_from_file(
    const std::string& file_path, size_t number_of_entries);

//------------------------------------------------------------------------------

struct Results
{
	Results(long long aa, long long bb, bool fail = false)
	    : success(!fail)
	    , a(aa)
	    , b(bb)
	{}
	bool success;
	long long a; // Coefficients
	long long b;
	static const Results& fail()
	{
		static const Results r(0, 0, true);
		return r;
	}
};

Results solve(const Vec2d& vx, const Vec2d& vy, const Vec2d& vz);

} // namespace aoc24_13

#endif // DAY_13_H
