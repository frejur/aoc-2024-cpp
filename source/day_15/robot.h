#ifndef ROBOT_H
#define ROBOT_H
#include "../lib/direction.h"
#include "../lib/vec2d.h"

namespace aoc24_15 {

class Robot
{
public:
	Robot()
	    : has_spawned_(false)
	    , init_pos_(0, 0)
	    , pos_(0, 0)
	{}
	void spawn(int pos_x, int pos_y);
	void move(aoc24::Direction dir);
	aoc24::Vec2d position() const { return pos_; }
	void reset() { pos_ = init_pos_; }

private:
	bool has_spawned_;
	aoc24::Vec2d init_pos_;
	aoc24::Vec2d pos_;
};

} // namespace aoc24_15

#endif // ROBOT_H
