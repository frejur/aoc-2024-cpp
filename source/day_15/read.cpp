#include "day_15.h"
#include <cassert>
#include <fstream>

std::istream& aoc24_15::operator>>(std::istream& istr, Direction& dir)
{
	char c{};
	istr >> c;
	switch (c) {
	case '^':
		dir = Direction::Up;
		break;
	case '>':
		dir = Direction::Right;
		break;
	case 'v':
		dir = Direction::Down;
		break;
	case '<':
		dir = Direction::Left;
		break;
	default:
		istr.clear(std::ios_base::failbit);
	}
	return istr;
}

std::ostream& aoc24_15::operator<<(std::ostream& ostr, const Direction& dir)
{
	switch (dir) {
	case Direction::Up:
		ostr << '^';
		break;
	case Direction::Right:
		ostr << '>';
		break;
	case Direction::Down:
		ostr << 'v';
		break;
	case Direction::Left:
		ostr << '<';
		break;
	}
	return ostr;
}

void aoc24_15::read_grid_and_moves_from_file(
    const std::string& file_path,
    std::vector<std::string>& output_text,
    std::queue<Direction>& output_moves,
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
	Direction dir;
	while (ifs >> dir) {
		output_moves.push(dir);
	}
	assert(output_moves.size() == moves_count);
}
