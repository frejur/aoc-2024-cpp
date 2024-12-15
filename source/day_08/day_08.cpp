#include "../lib/dynbs.h"
#include "../lib/grid.h"
#include "../lib/puzzle.h"
#include "node_grid.h"

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
try {
	using namespace aoc24_08;
	using aoc24::Char_grid;
	constexpr bool Use_resonant_harmonics{true};

	aoc24::Puzzle pz{8, "Resonant Collinearity", argc, argv};

	constexpr size_t sz_inp{50};
	constexpr size_t sz_test{12};
	const size_t sz = (pz.is_testing() ? sz_test : sz_inp);
	auto bitset = (pz.is_testing())
	                  ? aoc24::Bitset_factory::create<sz_test * sz_test>()
	                  : aoc24::Bitset_factory::create<sz_inp * sz_inp>();

	Char_grid chars(pz.input_file_path(), sz);
	Node_grid grid{chars, std::move(bitset)};

	// Part one
	grid.mark_antinodes();
	int p1_num_antinode_tiles{grid.number_of_antinode_tiles()};

	// Part two
	grid.mark_antinodes(Use_resonant_harmonics);
	int p2_num_antinode_tiles_w_res_harmonics{grid.number_of_antinode_tiles()};

	pz.file_answer(1, "Unique tiles with antinodes", p1_num_antinode_tiles);
	pz.file_answer(2,
	               "Tiles (Reson. harmonics)",
	               p2_num_antinode_tiles_w_res_harmonics);
	pz.print_answers();

	return 0;
} catch (const std::exception& e) {
	std::cerr << "Error: " << e.what() << '\n';
	return 1;
} catch (...) {
	std::cerr << "Unknown error" << '\n';
	return 2;
}
