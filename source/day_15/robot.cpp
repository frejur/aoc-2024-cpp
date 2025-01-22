#include "robot.h"

void aoc24_15::Robot::spawn(int pos_x, int pos_y)
{
	init_pos_ = {pos_x, pos_y};
	pos_ = init_pos_;
	has_spawned_ = true;
}

void aoc24_15::Robot::move(aoc24::Direction dir)
{
	aoc24::Vec2d offs = dir_to_offset(dir);
	pos_.x += offs.x;
	pos_.y += offs.y;
}
