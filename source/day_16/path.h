#ifndef PATH_H
#define PATH_H
#include "../lib/direction.h"
#include "../lib/vec2d.h"
#include <vector>

namespace aoc24_16 {

struct Turn
{
	Turn(
	    int pos_x, int pos_y, aoc24::Direction dir)
	    : position(pos_x, pos_y)
	    , direction(dir)
	{}
	aoc24::Vec2d position;
	aoc24::Direction direction;
};

struct Turn_hash
{
	size_t operator()(
	    const Turn& turn) const
	{
		size_t h1 = std::hash<int>()(turn.position.x);
		size_t h2 = std::hash<int>()(turn.position.y);
		size_t h3 = std::hash<aoc24::Direction>()(turn.direction);

		size_t result = h1;
		result = result * 31 + h2;
		result = result * 31 + h3;
		return result;
	}
};

class Point
{
public:
	Point(int pos_x,
	      int pos_y,
	      aoc24::Direction dir_in,
	      aoc24::Direction dir_out);
	aoc24::Vec2d position() const { return pos_; }
	aoc24::Direction direction_in() const { return dir_in_; }
	aoc24::Direction direction_out() const { return dir_out_; }
	operator aoc24::Vec2d() const { return pos_; }

private:
	aoc24::Vec2d pos_;
	aoc24::Direction dir_in_;
	aoc24::Direction dir_out_;
};

//------------------------------------------------------------------------------

class Path
{
public:
	Path(int start_x, int start_y, aoc24::Direction direction_out);
	aoc24::Vec2d position() const { return pts_.back().position(); }
	aoc24::Direction direction_in() const { return pts_.back().direction_in(); }
	aoc24::Direction direction_out() const
	{
		return pts_.back().direction_out();
	}
	void add_point(int pos_x,
	               int pos_y,
	               aoc24::Direction direction_in,
	               aoc24::Direction direction_out);
	void add_end_point(int pos_x, int pos_y);
	bool has_point(int pos_x, int pos_y) const;
	long pending_score() const;
	long estimated_final_score(
	    const std::vector<std::vector<int>>& dist_to_end) const;
	long final_score() const;
	int number_of_points() const
	{
		return static_cast<int>(pts_.size() + (has_end_pt_ ? 1 : 0));
	}
	std::vector<aoc24::Vec2d> points() const
	{
		std::vector<aoc24::Vec2d> result(pts_.begin(), pts_.end());
		result.emplace_back(end_pt_pos_.x, end_pt_pos_.y);
		return result;
	}

private:
	bool has_end_pt_;
	long score_;
	aoc24::Vec2d end_pt_pos_;
	std::vector<Point> pts_;

	static constexpr int turn_cost{1000};

	// Score helpers
	int calc_turn_cost(aoc24::Direction in_dir, aoc24::Direction out_dir) const;
	int calc_move_cost(int start_x, int start_y, int end_x, int end_y) const;
};

// Used for priority queue
struct Path_comparator
{
	const std::vector<std::vector<int>>& dist_;

	Path_comparator(
	    const std::vector<std::vector<int>>& dist)
	    : dist_(dist)
	{}
	bool operator()(
	    const Path& lhs, const Path& rhs) const
	{
		return lhs.estimated_final_score(dist_)
		       > rhs.estimated_final_score(dist_);
	}
};

} // namespace aoc24_16

#endif // PATH_H
