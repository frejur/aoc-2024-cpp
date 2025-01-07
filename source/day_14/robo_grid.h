#ifndef ROBO_GRID_H
#define ROBO_GRID_H
#include "../lib/int_grid.h"
#include <iostream>
#include <limits>
#include <vector>

namespace aoc24_14 {
struct Vec2d
{
	Vec2d(int xx, int yy)
	    : x(xx)
	    , y(yy)
	{}
	int x;
	int y;
	static constexpr int nopos = std::numeric_limits<int>::min();
	static const Vec2d& dummy()
	{
		static const Vec2d v{nopos, nopos};
		return v;
	}
	bool is_dummy() const { return (x == nopos && y == nopos); }
};

struct Robot_position_and_velocity
{
	Robot_position_and_velocity(int pos_x, int pos_y, int vel_x, int vel_y)
	    : position(pos_x, pos_y)
	    , velocity(vel_x, vel_y)
	{}
	Vec2d position;
	Vec2d velocity;
};

std::istream& operator>>(std::istream& iss, Robot_position_and_velocity& rpv);

//------------------------------------------------------------------------------

class Robo_grid; // Fwd decl

class Robot
{
public:
	Robot(int pos_x, int pos_y)
	    : attached_(false)
	    , grid_(nullptr)
	    , init_pos_(pos_x, pos_y)
	    , pos_(pos_x, pos_y)
	    , v_(0, 0)
	{}
	void update_velocity(int x, int y);
	void move(int ticks = 1);
	Vec2d position() const { return pos_; }
	void reset() { pos_ = init_pos_; }

	friend Robo_grid;

private:
	bool attached_;
	Robo_grid* grid_;
	Vec2d init_pos_;
	Vec2d pos_;
	Vec2d v_;

	void warp();
};

struct Palindrome
{
	Palindrome(size_t p, int r)
	    : position(p)
	    , radius(r)
	{}
	size_t position;
	int radius;
	static constexpr size_t nopos = -1;
	static const Palindrome& dummy()
	{
		static const Palindrome p{nopos, 0};
		return p;
	}
	bool is_dummy() const { return (position == nopos && radius == 0); }
};

struct Row_result
{
	Row_result(int numr, int max_r)
	    : number_of_rows(numr)
	    , max_radius(max_r)
	{}
	int number_of_rows;
	int max_radius;
};

class Robo_grid : public aoc24::Int_grid
{
public:
	Robo_grid(const std::vector<Robot_position_and_velocity>& robot_pos_and_v,
	          size_t grid_size_x,
	          size_t grid_size_y);
	void fast_forward(int ticks);
	void print(std::ostream& ostr = std::cout) const;
	Vec2d find_symmetrical_pattern() const;
	int safety_factor() const;
	void reset_time();
	long long current_time() const { return ticks_; }
	void print_pattern_at_position(const Vec2d& pos,
	                               std::ostream& ostr = std::cout) const;

	static constexpr size_t nopos = -1;

private:
	enum class Direction { Up, Down };
	long long ticks_;
	std::vector<Robot> rob_;

	void attach_robot(Robot& r);
	std::vector<Palindrome> max_w_palindromes(const std::vector<int>& value_row,
	                                          const int min_w = 1) const;
	Row_result count_symmetrical_rows(const Vec2d& pos,
	                                  const Direction dir,
	                                  const int max_rows,
	                                  const int num_gaps_allowed) const;
};

} // namespace aoc24_14

#endif // ROBO_GRID_H
