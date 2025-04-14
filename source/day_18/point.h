#ifndef POINT_H
#define POINT_H
#include "../lib/direction.h"
#include "../lib/vec2d.h"
#include <vector>

namespace aoc24_18 {

class Point
{
public:
	Point(int pos_x, int pos_y, aoc24::Direction dir, long num_steps);
	aoc24::Vec2d position() const { return pos_; }
	aoc24::Direction direction() const { return dir_; }
	void set_number_of_steps(long num_steps);
	long number_of_steps() const { return num_steps_; }
	long estimated_final_number_of_steps(
	    const std::vector<std::vector<long>>& dist_to_end) const;
	operator aoc24::Vec2d() const { return pos_; }

private:
	aoc24::Vec2d pos_;
	aoc24::Direction dir_;
	long num_steps_;
};

// Used for priority queue
struct Point_comparator
{
	const std::vector<std::vector<long>>& dist_;

	Point_comparator(
	    const std::vector<std::vector<long>>& dist)
	    : dist_(dist)
	{}
	bool operator()(
	    const Point& lhs, const Point& rhs) const
	{
		return lhs.estimated_final_number_of_steps(dist_)
		       > rhs.estimated_final_number_of_steps(dist_);
	}
};

} // namespace aoc24_18

#endif // POINT_H
