#include "extract.h"
#include <cassert>
#include <fstream>

std::vector<std::string> aoc24::read_input_into_grid(const std::string& p,
                                                     size_t grid_size)
{
	return read_input_into_grid(p, grid_size, grid_size);
}

std::vector<std::string> aoc24::read_input_into_grid(
    const std::string& file_path, size_t width, size_t height)
{
	std::ifstream ifs{file_path};
	if (!ifs) {
		throw std::ios_base::failure("Could not open sample input file: "
		                             + file_path);
	}

	std::vector<std::string> str_v;
	str_v.reserve(height);

	std::string s;
	s.reserve(width);

	while (std::getline(ifs, s)) {
		assert(s.size() == width);
		str_v.emplace_back(std::move(s));
	}
	assert(str_v.size() == height);

	return str_v;
}
