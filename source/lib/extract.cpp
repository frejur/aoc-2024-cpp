#include "extract.h"
#include <cassert>
#include <fstream>

std::vector<std::string> aoc24::read_input_into_grid(const std::string& p,
                                                     size_t grid_size)
{
	std::ifstream ifs{p};
	if (!ifs) {
		throw std::ios_base::failure("Could not open sample input file: " + p);
	}

	std::vector<std::string> str_v;
	str_v.reserve(grid_size);

	std::string s;
	s.reserve(grid_size);

	while (std::getline(ifs, s)) {
		assert(s.size() == grid_size);
		str_v.emplace_back(std::move(s));
	}
	assert(str_v.size() == grid_size);

	return str_v;
}
