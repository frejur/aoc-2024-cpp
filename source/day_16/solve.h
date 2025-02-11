#ifndef SOLVE_H
#define SOLVE_H
#include "path.h"
#include <vector>

namespace aoc24_16 {

// Forward declare
class Maze;

std::vector<std::vector<int>> manhattan_distance_grid(const Maze& m);
std::vector<Path> get_shortest_paths(const Maze& maze);

// Needed for std::stack
bool operator<(const Turn& lhs, const Turn& rhs);
bool operator==(const Turn& lhs, const Turn& rhs);

} // namespace aoc24_16

#endif // SOLVE_H
