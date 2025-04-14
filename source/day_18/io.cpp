#include "../lib/vec2d.h"
#include "io.h"
#include <cassert>
#include <fstream>
#include <sstream>

std::vector<aoc24::Vec2d> aoc24_18::read_coordinates_from_file(
    const std::string& filepath, const size_t number_of_entries)
{
	std::ifstream ifs{filepath};
	if (!ifs) {
		throw std::invalid_argument("Could not open input file at: \""
		                            + filepath + "\"");
	}

	std::istringstream isstr;

	std::vector<aoc24::Vec2d> coords;
	std::string ln;
	while (std::getline(ifs, ln)) {
		int x{}, y{};
		char comma{};
		isstr.str(ln);
		isstr.clear(std::ios_base::goodbit);
		while (isstr >> x >> comma >> y) {
			if (comma != ',') {
				throw std::runtime_error("Invalid character, expected comma");
			}
			coords.emplace_back(x, y);
		}
	}
	assert(coords.size() == number_of_entries);

	return coords;
}
