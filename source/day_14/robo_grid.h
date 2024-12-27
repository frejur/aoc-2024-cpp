#ifndef ROBO_GRID_H
#define ROBO_GRID_H
#include "../lib/int_grid.h"
#include <iostream>
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
	    , pos_(pos_x, pos_y)
	    , v_(0, 0)
	{}
	void update_velocity(int x, int y);
	void move(int ticks = 1);
	Vec2d position() const { return pos_; }
	friend Robo_grid;

private:
	bool attached_;
	Robo_grid* grid_;
	Vec2d pos_;
	Vec2d v_;

	void warp();
};

class Robo_grid : public aoc24::Int_grid
{
public:
	Robo_grid(const std::vector<Robot_position_and_velocity>& robot_pos_and_v,
	          size_t grid_size_x,
	          size_t grid_size_y);
	void fast_forward(int ticks);
	void print(std::ostream& ostr = std::cout) const;
	int safety_factor() const;

	static constexpr size_t nopos = -1;

private:
	std::vector<Robot> rob_;

	void attach_robot(Robot& r);
};

} // namespace aoc24_14

#endif // ROBO_GRID_H
