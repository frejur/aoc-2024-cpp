#include "solve.h"
#include "maze.h"
#include <stack>
#include <unordered_map>

bool aoc24_16::operator<(
    const Turn& lhs, const Turn& rhs)
{
	if (lhs.position.x != rhs.position.x) {
		return lhs.position.x < rhs.position.x;
	}
	if (lhs.position.y != rhs.position.y) {
		return lhs.position.y < rhs.position.y;
	}
	return lhs.direction < rhs.direction;
}

bool aoc24_16::operator==(
    const Turn& lhs, const Turn& rhs)
{
	return lhs.position == rhs.position && lhs.direction == rhs.direction;
}

//------------------------------------------------------------------------------

std::vector<aoc24_16::Path> aoc24_16::get_shortest_paths(
    const Maze& maze)
{
    using Vec = aoc24::Vec2d;
    using Dir = aoc24::Direction;

    std::vector<std::vector<int>> dist_to_end{ manhattan_distance_grid(maze) };

    std::stack<Path> path_stack;
    std::unordered_map<Turn, long, Turn_hash> turn_scores;

    std::vector<Dir> start_dirs = maze.get_start_point_directions();
    if (start_dirs.empty()) {
        return {};
    }

    std::vector<Path> shortest_paths;
    Vec start_pos = maze.start_position();

    for (const Dir dir : start_dirs) {
        path_stack.emplace(start_pos.x, start_pos.y, dir);
    }

    long min_score = 0;
    bool found_path = false;
    while (!path_stack.empty()) {

        Path& this_path = path_stack.top();

        long this_p_score = this_path.pending_score();
		if (found_path
		    && ((this_p_score > min_score)
		        || dist_to_end[this_path.position().x][this_path.position().y])
		           > (min_score - this_p_score)) {
			path_stack.pop();
			continue;
		}

		Vec next_xy = maze.get_next_point_in_dir(this_path.position().x,
		                                         this_path.position().y,
		                                         this_path.direction_out());

		if (next_xy == maze.end_position()) {
			this_path.add_end_point(maze.end_position().x,
                maze.end_position().y);
            long this_score = this_path.score();

            if (!found_path || this_score < min_score) {
                shortest_paths.clear();
                min_score = this_score;
                found_path = true;
            }

            if (this_score <= min_score) {
                shortest_paths.push_back(this_path);
            }
            path_stack.pop();
            continue;
		}

		if (next_xy == this_path.position()
            || this_path.has_point(next_xy.x, next_xy.y)) {
            path_stack.pop();
            continue;
        }

        std::vector<Dir> available_dirs{
            maze.get_available_directions_at_point(next_xy.x,
                                               next_xy.y,
                                               this_path.direction_out()) };

        int numdirs{ static_cast<int>(available_dirs.size()) };

        if (numdirs < 1) {
            path_stack.pop();
            continue;
        }

        if (numdirs == 1) {
            this_path.add_point(next_xy.x,
                next_xy.y,
                this_path.direction_out(),
                available_dirs.front());
            continue;
        }

        Path p_copy = path_stack.top();
        Dir dir_out = this_path.direction_out();
        for (int i = 0; i < numdirs; ++i) {
            Path p_new_copy = p_copy;
            Dir new_out_dir = available_dirs[i];
            p_new_copy.add_point(next_xy.x,
                next_xy.y,
                dir_out,
                available_dirs[i]);

            Turn new_turn = { next_xy.x, next_xy.y, new_out_dir };
            long new_score = p_new_copy.pending_score();
            auto it = turn_scores.find(new_turn);
            if (it == turn_scores.end()) {
                turn_scores.try_emplace(new_turn, new_score);
            }
            else if (it->second >= new_score) {
                turn_scores[new_turn] = new_score;
            }
            else {
                if (i == 0) {
                    path_stack.pop();
                }
                continue;
            }

            if (i == 0) {
                this_path.add_point(next_xy.x,
                    next_xy.y,
                    dir_out,
                    available_dirs[i]);
            }
            else {
                path_stack.emplace(std::move(p_new_copy));
            }
        }
    }

    return shortest_paths;
}

std::vector<std::vector<int>> aoc24_16::manhattan_distance_grid(
    const Maze& m)
{
	aoc24::Vec2d end_pt = m.end_position();
	std::vector<std::vector<int>> dist_grid;
	dist_grid.resize(m.height(), std::vector<int>(m.width()));
	for (int row = 0; row < m.height(); ++row) {
		for (int col = 0; col < m.width(); ++col) {
			dist_grid[row][col] = (std::abs)(end_pt.x - col)
			                      + (std::abs)(end_pt.y - row);
		}
	}
	return dist_grid;
}
