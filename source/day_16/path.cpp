#include "path.h"

namespace {
constexpr aoc24::Direction start_direction{aoc24::Direction::Right};
}

aoc24_16::Path::Path(
    int start_x, int start_y, aoc24::Direction direction_out)
    : has_end_pt_(false)
	, score_(0)
    , pts_{Point{start_x, start_y, start_direction, direction_out}}
{
	pts_.reserve(200);
}

void aoc24_16::Path::add_point(
    int pos_x,
    int pos_y,
    aoc24::Direction direction_in,
    aoc24::Direction direction_out)
{
	if (has_end_pt_) {
		throw std::logic_error("Path locked");
	}

	// Update pending score
	const Point& last_pt = pts_.back();
	score_ += calc_turn_cost(direction_in, direction_out)
	          + calc_move_cost(last_pt.position().x,
	                           last_pt.position().y,
	                           pos_x,
	                           pos_y);

	// If this is the first point to be added, add the score for turning around
	// at the start position
	if (pts_.size() == 1) {
		score_ += calc_turn_cost(pts_[0].direction_in(),
		                         pts_[0].direction_out());
	}

	// Add new point
	pts_.emplace_back(pos_x, pos_y, direction_in, direction_out);
}

void aoc24_16::Path::add_end_point(
    int pos_x, int pos_y)
{
	if (has_end_pt_) {
		throw std::logic_error("Path already has an end point");
	}
	has_end_pt_ = true;
	end_pt_pos_ = {pos_x, pos_y};
}

bool aoc24_16::Path::has_point(
    int pos_x, int pos_y) const
{
	for (const auto& p : pts_) {
		if (p.position().x == pos_x && p.position().y == pos_y) {
			return true;
		}
	}
	return false;
}

long aoc24_16::Path::pending_score() const
{
	return score_;
}

long aoc24_16::Path::estimated_final_score(
    const std::vector<std::vector<int> >& dist_to_end) const
{
	return score_
	       + dist_to_end[pts_.back().position().x][pts_.back().position().y];
}

long aoc24_16::Path::final_score() const
{
	if (!has_end_pt_) {
		throw std::logic_error("Path has no end point");
	}

	return score_
	       + calc_move_cost(pts_.back().position().x,
	                        pts_.back().position().y,
	                        end_pt_pos_.x,
	                        end_pt_pos_.y);
}

//------------------------------------------------------------------------------

int aoc24_16::Path::calc_turn_cost(
    aoc24::Direction in_dir, aoc24::Direction out_dir) const
{
	if (in_dir == out_dir) {
		return 0;
	}

	int delta = std::abs(static_cast<int>(in_dir) - static_cast<int>(out_dir));
	return (delta == 3 ? 1 : delta) * turn_cost;
}

int aoc24_16::Path::calc_move_cost(
    int start_x, int start_y, int end_x, int end_y) const
{
	return std::abs(end_x - start_x) + std::abs(end_y - start_y);
}

//------------------------------------------------------------------------------

aoc24_16::Point::Point(
    int pos_x, int pos_y, aoc24::Direction dir_in, aoc24::Direction dir_out)
    : pos_(pos_x, pos_y)
    , dir_in_(dir_in)
    , dir_out_(dir_out)
{}
