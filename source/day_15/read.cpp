#include "day_15.h"
#include <cassert>
#include <fstream>

void aoc24_15::read_grid_and_moves_from_file(
    const std::string& file_path,
    std::vector<std::string>& output_text,
    std::queue<aoc24::Direction>& output_moves,
    size_t grid_size,
    size_t moves_count)
{
	std::ifstream ifs{file_path};
	if (!ifs) {
		throw std::ios_base::failure("Could not open sample input file: "
		                             + file_path);
	}

	// Grid
	std::string s;
	s.reserve(grid_size);

	while (std::getline(ifs, s)) {
		if (s.empty()) {
			break;
		}
		assert(s.size() == grid_size);
		output_text.emplace_back(std::move(s));
	}
	assert(output_text.size() == grid_size);

	// Moves
	aoc24::Direction dir;
	while (ifs >> dir) {
		output_moves.push(dir);
	}
	assert(output_moves.size() == moves_count);
}
