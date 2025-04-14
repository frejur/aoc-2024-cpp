#include "bytegrid.h"
#include "solve.h"

namespace {
aoc24::Vec2d noxy{-1, -1};

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
	L_matrix	dist_to_start{manhattan_distance_grid(bgrid, true)};
	Queue		pt_queue_fr_start{Point_comparator(dist_to_end)};
	Queue		pt_queue_fr_end{Point_comparator(dist_to_start)};
	Pos_map		pos_history_fr_start;
	Pos_map		pos_history_fr_end;
	Step_map	pos_num_steps_fr_start;
	Step_map	pos_num_steps_fr_end;
	// clang-format on

	std::vector<Dir> start_dirs = bgrid.get_start_point_directions();
	std::vector<Dir> end_dirs = bgrid.get_end_point_directions();
	if (start_dirs.empty() || end_dirs.empty()) {
		return {};
	}

	Vec start_pos = bgrid.start_position();
	Vec end_pos = bgrid.end_position();

	for (const Dir dir : start_dirs) {
		pt_queue_fr_start.emplace(start_pos.x, start_pos.y, dir, 0);
	}

	for (const Dir dir : end_dirs) {
		pt_queue_fr_end.emplace(end_pos.x, end_pos.y, dir, 0);
	}

	// Perform a bi-directional for the shortest path,
	// 1. From the starting point until the end point.
	// 2. From the end point until the starting point.
	// Continue until one of the paths reaches the target point,
	// or until both paths intersect.
	Vec xsect = noxy;

	bool fr_start{true};
	bool found_xsect{false};
	while (!pt_queue_fr_start.empty() || !pt_queue_fr_end.empty()) {
		Queue& queue = fr_start ? pt_queue_fr_start : pt_queue_fr_end;
		Pos_map& history = fr_start ? pos_history_fr_start : pos_history_fr_end;
		Step_map& num_steps = fr_start ? pos_num_steps_fr_start
		                               : pos_num_steps_fr_end;

		auto result = advance_path(bgrid, xsect, queue, history, num_steps);
		switch (result) {
		case Path_result::Goal: {
			const Vec& start = fr_start ? bgrid.start_position()
			                            : bgrid.end_position();
			const Vec& end = fr_start ? bgrid.end_position()
			                          : bgrid.start_position();
			return reconstruct_path(history, start, end);
		}
		case Path_result::Intersection:
			found_xsect = true;
			break;
		case Path_result::Queue_empty:
		case Path_result::Illegal_turn:
		case Path_result::Dead_end:
		case Path_result::Loop:
		case Path_result::Corner_or_fork:
		default:
			break;
		}

		fr_start = !fr_start;
	}

	if (!found_xsect) {
		return {};
	}

	// Find shortest path to intersection
	L_matrix dist_to_xsect{manhattan_distance_grid(bgrid, xsect)};
	Queue pt_queue_to_xsect_fr_start{Point_comparator(dist_to_xsect)};
	Queue pt_queue_to_xsect_fr_end{Point_comparator(dist_to_xsect)};
	pos_history_fr_start.clear();
	pos_history_fr_end.clear();
	pos_num_steps_fr_start.clear();
	pos_num_steps_fr_end.clear();

	for (const Dir dir : start_dirs) {
		pt_queue_fr_start.emplace(start_pos.x, start_pos.y, dir, 0);
	}

	for (const Dir dir : end_dirs) {
		pt_queue_fr_end.emplace(end_pos.x, end_pos.y, dir, 0);
	}

	for (Path_result result = Path_result::No_result;
	     result != Path_result::Goal;
	     result = advance_path(bgrid,
	                           pt_queue_to_xsect_fr_start,
	                           pos_history_fr_start,
	                           pos_num_steps_fr_start)) {
	}
	for (Path_result result = Path_result::No_result;
	     result != Path_result::Goal;
	     result = advance_path(bgrid,
	                           pt_queue_to_xsect_fr_end,
	                           pos_history_fr_end,
	                           pos_num_steps_fr_end)) {
	}

	return reconstruct_and_merge_paths(pos_history_fr_start,
	                                   pos_history_fr_end,
	                                   bgrid.start_position(),
	                                   xsect,
	                                   bgrid.end_position());
}

aoc24_18::Path_result aoc24_18::advance_path(
    const Bgrid& grid,
    Vec& xsect_pos,
    Queue& pt_queue,
    Pos_map& pos_history,
    Step_map& position_num_steps,
    bool ignore_xsect)
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
		pos_history.try_emplace(grid.end_position(), prev_xy);
		return Path_result::Goal;
	}

	if (!ignore_xsect && next_xy == xsect_pos) {
		pos_history.try_emplace(next_xy, prev_xy);
		return Path_result::Intersection;
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

	pos_history.try_emplace(next_xy, prev_xy);

	// Corner / Junction
	for (int i = 0; i < numdirs; ++i) {
		Dir next_dir{available_dirs[i]};
		auto it = position_num_steps.find(next_xy);
		if (it == position_num_steps.end() || it->second >= next_num_steps) {
			position_num_steps[next_xy] = next_num_steps;
			pt_queue.emplace(next_xy.x, next_xy.y, next_dir, next_num_steps);
		}
	}

	if (!ignore_xsect) {
		xsect_pos = next_xy; // Update point of intersection
	}

	return Path_result::Corner_or_fork;
}

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

std::vector<aoc24_18::Point> aoc24_18::reconstruct_path(
    const std::unordered_map<aoc24::Vec2d, aoc24::Vec2d, aoc24::Vec2d_hash>&
        xy_map,
    const aoc24::Vec2d& begin_pos,
    const aoc24::Vec2d& break_pos,
    bool reverse_pts)
{
	std::vector<aoc24_18::Point> final_path;
	std::vector<aoc24::Vec2d> path_reversed; // Store Vec2d first

	aoc24::Vec2d local_pos = break_pos;

	if (break_pos == begin_pos) {
		final_path.emplace_back(begin_pos.x, begin_pos.y, Dir::Up, 0);
		return final_path;
	}

	// Backtrack from the end position using parent XY map
	while (local_pos != begin_pos) {
		path_reversed.push_back(local_pos);

		auto it = xy_map.find(local_pos);
		if (it == xy_map.end()) {
			throw std::runtime_error("Path is broken");
		}
		local_pos = it->second;
	}
	path_reversed.push_back(begin_pos); // Add start position back in

	if (!reverse_pts) {
		// Reverse to ensure correct order
		std::reverse(path_reversed.begin(), path_reversed.end());
	}

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
		Dir dir = !reverse_pts ? Dir::Up : Dir::Down;
		if (local_xy.x < later_xy.x) {
			dir = !reverse_pts ? Dir::Right : Dir::Left;
		} else if (local_xy.x > later_xy.x) {
			dir = !reverse_pts ? Dir::Left : Dir::Right;
		} else if (local_xy.y < later_xy.y) {
			dir = !reverse_pts ? Dir::Down : Dir::Up;
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

std::vector<aoc24_18::Point> aoc24_18::reconstruct_and_merge_paths(
    const std::unordered_map<aoc24::Vec2d, aoc24::Vec2d, aoc24::Vec2d_hash>&
        lhs_xy_map,
    const std::unordered_map<aoc24::Vec2d, aoc24::Vec2d, aoc24::Vec2d_hash>&
        rhs_xy_map,
    const aoc24::Vec2d& start_pos,
    const aoc24::Vec2d& xsect_pos,
    const aoc24::Vec2d& goal_pos)
{
	std::vector<Point> lhs_pts{
	    reconstruct_path(lhs_xy_map, start_pos, xsect_pos)};
	std::vector<Point> rhs_pts{
	    reconstruct_path(rhs_xy_map, goal_pos, xsect_pos, true)};
	if (lhs_pts.size() < 2 || rhs_pts.size() < 2) {
		throw std::logic_error("Cannot merge incomplete paths");
	}

	std::vector<Point> pts{};
	pts.reserve(lhs_pts.size() + rhs_pts.size() - 1);
	pts.insert(pts.end(), lhs_pts.begin(), lhs_pts.end());
	pts.insert(pts.end(), rhs_pts.begin() + 1, rhs_pts.end());

	long num_steps{0};
	for (int i = 0; i < pts.size(); ++i) {
		Point& local_pt{pts[i]};
		const aoc24::Vec2d& local_xy{local_pt.position()};
		if (i > 0) {
			const aoc24::Vec2d& prior_xy{pts[i - 1].position()};
			num_steps += std::abs(local_xy.x - prior_xy.x)
			             + std::abs(local_xy.y - prior_xy.y);
		}
		local_pt.set_number_of_steps(num_steps);
	}
	return pts;
}

aoc24_18::Path_result aoc24_18::advance_path(
    const Byte_grid& grid,
    std::priority_queue<Point, std::vector<Point>, Point_comparator>& pt_queue,
    std::unordered_map<aoc24::Vec2d, aoc24::Vec2d, aoc24::Vec2d_hash>&
        pos_history,
    std::unordered_map<aoc24::Vec2d, long, aoc24::Vec2d_hash>& position_num_steps)
{
	return advance_path(grid,
	                    noxy,
	                    pt_queue,
	                    pos_history,
	                    position_num_steps,
	                    false);
}
