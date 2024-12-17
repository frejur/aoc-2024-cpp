#include "../lib/dynbs.h"
#include "../lib/grid.h"
#include "../lib/puzzle.h"
#include "topo_grid.h"

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
try {
	using namespace aoc24_10;
	using aoc24::Char_grid;

	aoc24::Puzzle pz{10, "Hoof It", argc, argv};

	constexpr size_t sz_inp{40};
	constexpr size_t sz_test{8};
	const size_t sz = (pz.is_testing() ? sz_test : sz_inp);
	auto bitset = (pz.is_testing())
	                  ? aoc24::Bitset_factory::create<sz_test * sz_test>()
	                  : aoc24::Bitset_factory::create<sz_inp * sz_inp>();

	Char_grid chars(pz.input_file_path(), sz);
	Topo_grid grid{chars, std::move(bitset)};

	grid.find_trails();
	int p1_score{grid.combined_trailhead_score()};
	int p2_rating{grid.combined_trailhead_rating()};

	pz.file_answer(1, "Combined trailhead score", p1_score);
	pz.file_answer(2, "Combined trailhead rating", p2_rating);
	pz.print_answers();

	return 0;
} catch (const std::exception& e) {
	std::cerr << "Error: " << e.what() << '\n';
	return 1;
} catch (...) {
	std::cerr << "Unknown error" << '\n';
	return 2;
}
