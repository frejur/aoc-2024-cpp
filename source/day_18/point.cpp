#include "point.h"

aoc24_18::Point::Point(
    int pos_x, int pos_y, aoc24::Direction dir, long num_steps)
    : pos_(pos_x, pos_y)
    , dir_(dir)
    , num_steps_(num_steps)
{}

void aoc24_18::Point::set_number_of_steps(
    long num_steps)
{
	num_steps_ = num_steps;
}

long aoc24_18::Point::estimated_final_number_of_steps(
    const std::vector<std::vector<long>>& dist_to_end) const
{
	return num_steps_ + dist_to_end[position().y][position().x];
}
