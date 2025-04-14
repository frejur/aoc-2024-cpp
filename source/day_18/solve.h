#ifndef SOLVE_H
#define SOLVE_H
#include "point.h"
#include <queue>
#include <unordered_map>
#include <vector>

namespace aoc24_18 {

// Forward declare
class Byte_grid;

std::vector<std::vector<long>> manhattan_distance_grid(
    const Byte_grid& g, const aoc24::Vec2d& target_xy);
std::vector<std::vector<long>> manhattan_distance_grid(
    const Byte_grid& g, bool switch_start_end = false);
std::vector<Point> get_shortest_path(const Byte_grid& grid);
std::vector<Point> reconstruct_path(
    const std::unordered_map<aoc24::Vec2d, aoc24::Vec2d, aoc24::Vec2d_hash>&
        xy_map,
    const aoc24::Vec2d& start_pos,
    const aoc24::Vec2d& goal_pos,
    bool reverse_pts = false);
std::vector<Point> reconstruct_and_merge_paths(
    const std::unordered_map<aoc24::Vec2d, aoc24::Vec2d, aoc24::Vec2d_hash>&
        lhs_xy_map,
    const std::unordered_map<aoc24::Vec2d, aoc24::Vec2d, aoc24::Vec2d_hash>&
        rhs_xy_map,
    const aoc24::Vec2d& start_pos,
    const aoc24::Vec2d& xsect_pos,
    const aoc24::Vec2d& goal_pos);

enum class Path_result {
	No_result,
	Queue_empty,
	Illegal_turn,
	Dead_end,
	Loop,
	Corner_or_fork,
	Intersection,
	Goal
};
Path_result advance_path(
    const Byte_grid& grid,
    aoc24::Vec2d& xsect_pos,
    std::priority_queue<Point, std::vector<Point>, Point_comparator>& pt_queue,
    std::unordered_map<aoc24::Vec2d, aoc24::Vec2d, aoc24::Vec2d_hash>&
        pos_history,
    std::unordered_map<aoc24::Vec2d, long, aoc24::Vec2d_hash>& position_num_steps,
    bool ignore_xsect = false);
Path_result advance_path(
    const Byte_grid& grid,
    std::priority_queue<Point, std::vector<Point>, Point_comparator>& pt_queue,
    std::unordered_map<aoc24::Vec2d, aoc24::Vec2d, aoc24::Vec2d_hash>&
        pos_history,
    std::unordered_map<aoc24::Vec2d, long, aoc24::Vec2d_hash>&
        position_num_steps);

// Needed for std::stack
bool operator<(const Point& lhs, const Point& rhs);
bool operator==(const Point& lhs, const Point& rhs);

} // namespace aoc24_18

#endif // SOLVE_H
