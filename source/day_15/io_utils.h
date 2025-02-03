#ifndef IO_UTILS_H
#define IO_UTILS_H
#include "../lib/direction.h"
#include <queue>
#include <string>

namespace aoc24_15 {
void read_grid_and_moves_from_file(const std::string& file_path,
                                   std::vector<std::string>& output_text,
                                   std::queue<aoc24::Direction>& output_moves,
                                   size_t grid_size,
                                   size_t moves_count);

std::ostream& operator<<(std::ostream& ostr, aoc24::Vec2d& vec);
} // namespace aoc24_15

#endif // IO_UTILS_H
