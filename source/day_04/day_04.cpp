#include "day_04.h"
#include "../lib/puzzle.h"

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
try {
	using namespace aoc24_04;
	using aoc24::Grid;

	aoc24::Puzzle pz{4, "Ceres Search", argc, argv};

	constexpr size_t sz_inp{140};
	constexpr size_t sz_test{10};
	const size_t sz = (pz.is_testing() ? sz_test : sz_inp);
	aoc24::Char_grid g{pz.input_file_path(), sz};

	int p1_num_xmas{count_xmas(g)};
	int p2_num_x_mas{count_x_mas(g)};

	pz.file_answer(1, "Number of \"XMAS\"", p1_num_xmas);
	pz.file_answer(2, "Number of \"X_MAS\"", p2_num_x_mas);
	pz.print_answers();

	return 0;
} catch (const std::exception& e) {
	std::cerr << "Error: " << e.what() << '\n';
	return 1;
} catch (...) {
	std::cerr << "Unknown error" << '\n';
	return 2;
}
