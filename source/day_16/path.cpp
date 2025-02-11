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
	int dir_diff = std::abs(static_cast<int>(direction_out)
		- static_cast<int>(direction_in));
	dir_diff = (dir_diff == 3) ? 1 : dir_diff;
	score_ += dir_diff * 1000 + std::abs(pos_x - last_pt.position().x)
			+ std::abs(pos_y - last_pt.position().y);
	
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

long aoc24_16::Path::score() const
{
	if (!has_end_pt_) {
		throw std::logic_error("Path has no end point");
	}

	long s{pending_score()};

	s += std::abs(end_pt_pos_.x - pts_.back().position().x)
	     + std::abs(end_pt_pos_.y - pts_.back().position().y);

	return s;
}

aoc24_16::Point::Point(
    int pos_x, int pos_y, aoc24::Direction dir_in, aoc24::Direction dir_out)
    : pos_(pos_x, pos_y)
    , dir_in_(dir_in)
    , dir_out_(dir_out)
{}
