#include "bytegrid.h"
#include "solve.h"
#include <algorithm>

namespace {
using L_matrix = std::vector<std::vector<long>>;
using Bgrid = aoc24_18::Byte_grid;
using Vec = aoc24::Vec2d;
using Dir = aoc24::Direction;
using Queue = std::priority_queue<aoc24_18::Point,
                                  std::vector<aoc24_18::Point>,
                                  aoc24_18::Point_comparator>;
using Pos_map = std::unordered_map<aoc24::Vec2d, aoc24::Vec2d, aoc24::Vec2d_hash>;
using Step_map = std::unordered_map<aoc24::Vec2d, long, aoc24::Vec2d_hash>;
} // namespace

//------------------------------------------------------------------------------

bool aoc24_18::operator<(
    const Point& lhs, const Point& rhs)
{
	if (lhs.position().x != rhs.position().x) {
		return lhs.position().x < rhs.position().x;
	}
	return lhs.position().y < rhs.position().y;
}

bool aoc24_18::operator==(
    const Point& lhs, const Point& rhs)
{
	return lhs.position() == rhs.position();
}

//------------------------------------------------------------------------------

std::vector<aoc24_18::Point> aoc24_18::get_shortest_path(
    const Bgrid& bgrid)
{
	// (Type aliases definitions at the top)
	// clang-format off
	L_matrix	dist_to_end{manhattan_distance_grid(bgrid)};
	Queue		queue{Point_comparator(dist_to_end)};
	Pos_map		history;
	Step_map	num_steps;
	// clang-format on

	std::vector<Dir> start_dirs = bgrid.get_start_point_directions();
	std::vector<Dir> end_dirs = bgrid.get_end_point_directions();
	if (start_dirs.empty() || end_dirs.empty()) {
		return {};
	}

	Vec start_pos = bgrid.start_position();
	Vec end_pos = bgrid.end_position();

	for (const Dir dir : start_dirs) {
		queue.emplace(start_pos.x, start_pos.y, dir, 0);
	}

	while (!queue.empty()) {
		Path_result result = advance_path(bgrid, queue, history, num_steps);
		switch (result) {
		case Path_result::Goal: {
			return reconstruct_path(history, start_pos, end_pos);
		}
		case Path_result::Corner_or_fork:
		case Path_result::Queue_empty:
		case Path_result::Illegal_turn:
		case Path_result::Dead_end:
		case Path_result::Loop:
		default:
			break;
		}
	}

	return {};
}

aoc24_18::Path_result aoc24_18::advance_path(
    const Bgrid& grid,
    Queue& pt_queue,
    Pos_map& pos_history,
    Step_map& position_num_steps)
{
	if (pt_queue.empty()) {
		return Path_result::Queue_empty;
	}

	aoc24_18::Point this_pt = std::move(pt_queue.top());
	pt_queue.pop();

	auto prev_xy{this_pt.position()};
	auto next_xy{grid.get_next_point_in_dir(this_pt.position().x,
	                                        this_pt.position().y,
	                                        this_pt.direction())};

	auto it = pos_history.find(next_xy);
	if (it != pos_history.end()) {
		return Path_result::Loop;
	}

	if (next_xy == grid.end_position()) {
		if (pos_history.find(grid.start_position()) == pos_history.end()) {
			pos_history.insert(std::make_pair(grid.end_position(), prev_xy));
		}
		return Path_result::Goal;
	}

	if (next_xy == prev_xy) {
		return Path_result::Illegal_turn;
	}

	std::vector<Dir> available_dirs{
	    grid.get_available_directions_at_point(next_xy.x,
	                                           next_xy.y,
	                                           this_pt.direction())};

	int numdirs{static_cast<int>(available_dirs.size())};

	if (numdirs < 1) {
		return Path_result::Dead_end;
	}

	long next_num_steps{this_pt.number_of_steps()
	                    + std::abs(next_xy.x - prev_xy.x)
	                    + std::abs(next_xy.y - prev_xy.y)};

	if (pos_history.find(next_xy) == pos_history.end()) {
		pos_history.insert(std::make_pair(next_xy, prev_xy));
	}

	// Corner / Junction
	for (int i = 0; i < numdirs; ++i) {
		Dir next_dir{available_dirs[i]};
		auto it = position_num_steps.find(next_xy);
		if (it == position_num_steps.end() || it->second >= next_num_steps) {
			position_num_steps[next_xy] = next_num_steps;
			pt_queue.emplace(next_xy.x, next_xy.y, next_dir, next_num_steps);
		}
	}

	return Path_result::Corner_or_fork;
}

//------------------------------------------------------------------------------

std::vector<std::vector<long>> aoc24_18::manhattan_distance_grid(
    const Bgrid& m, const aoc24::Vec2d& target_xy)
{
	std::vector<std::vector<long>> dist_grid;
	dist_grid.resize(m.height(), std::vector<long>(m.width()));
	for (int row = 0; row < m.height(); ++row) {
		for (int col = 0; col < m.width(); ++col) {
			dist_grid[row][col] = (std::abs)(target_xy.x - col)
			                      + (std::abs)(target_xy.y - row);
		}
	}
	return dist_grid;
}

std::vector<std::vector<long>> aoc24_18::manhattan_distance_grid(
    const Bgrid& m, bool switch_start_end)
{
	const aoc24::Vec2d& end_pt = (!switch_start_end) ? m.end_position()
	                                                 : m.start_position();
	return manhattan_distance_grid(m, end_pt);
}

//------------------------------------------------------------------------------

std::vector<aoc24_18::Point> aoc24_18::reconstruct_path(
    const std::unordered_map<aoc24::Vec2d, aoc24::Vec2d, aoc24::Vec2d_hash>&
        xy_map,
    const Vec& start_pos,
    const Vec& end_pos)
{
	std::vector<aoc24_18::Point> final_path;
	std::vector<aoc24::Vec2d> path_reversed; // Store Vec2d first

	aoc24::Vec2d local_pos = end_pos;

	if (end_pos == start_pos) {
		final_path.emplace_back(start_pos.x, start_pos.y, Dir::Up, 0);
		return final_path;
	}

	// Backtrack from the end position using parent XY map
	while (local_pos != start_pos) {
		path_reversed.push_back(local_pos);

		auto it = xy_map.find(local_pos);
		if (it == xy_map.end()) {
			throw std::runtime_error("Path is broken");
		}
		local_pos = it->second;
	}
	path_reversed.push_back(start_pos); // Add start position back in

	// Reverse to ensure correct order
	std::reverse(path_reversed.begin(), path_reversed.end());

	// Convert coordinates to Points (Reverse directions if needed)
	long num_steps{0};
	final_path.reserve(path_reversed.size());
	for (int i = 0; i < path_reversed.size() - 1; ++i) {
		const aoc24::Vec2d& local_xy{path_reversed[i]};
		const aoc24::Vec2d& later_xy{path_reversed[i + 1]};
		if (i > 0) {
			const aoc24::Vec2d& prior_xy{path_reversed[i - 1]};
			num_steps += std::abs(local_xy.x - prior_xy.x)
			             + std::abs(local_xy.y - prior_xy.y);
		}
		Dir dir = Dir::Up;
		if (local_xy.x < later_xy.x) {
			dir = Dir::Right;
		} else if (local_xy.x > later_xy.x) {
			dir = Dir::Left;
		} else if (local_xy.y < later_xy.y) {
			dir = Dir::Down;
		}
		final_path.emplace_back(local_xy.x, local_xy.y, dir, num_steps);
	}
	num_steps += std::abs(path_reversed.back().x
	                      - final_path.back().position().x)
	             + std::abs(path_reversed.back().y
	                        - final_path.back().position().y);
	final_path.emplace_back(path_reversed.back().x,
	                        path_reversed.back().y,
	                        final_path.back().direction(),
	                        num_steps);

	return final_path;
}
