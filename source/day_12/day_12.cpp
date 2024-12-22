#include "../lib/dynbs.h"
#include "../lib/grid.h"
#include "../lib/puzzle.h"
#include "grp_grid.h"

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
try {
	using namespace aoc24_12;
	using aoc24::Char_grid;

	aoc24::Puzzle pz{12, "Garden Groups", argc, argv};

	constexpr size_t sz_inp{140};
	constexpr size_t sz_test{10};
	const size_t sz = (pz.is_testing() ? sz_test : sz_inp);
	auto bitset = (pz.is_testing())
	                  ? aoc24::Bitset_factory::create<sz_test * sz_test>()
	                  : aoc24::Bitset_factory::create<sz_inp * sz_inp>();

	Char_grid chars(pz.input_file_path(), sz);
	Group_grid grid{chars, std::move(bitset)};

	long long p1_fence_cost{grid.calculate_fence_cost()};
	long long p2_fence_cost{grid.calculate_fence_cost(true)};

	pz.file_answer(1, "Total fence cost", p1_fence_cost);
	pz.file_answer(2, "Discounted fence cost", p2_fence_cost);
	pz.print_answers();

	return 0;
} catch (const std::exception& e) {
	std::cerr << "Error: " << e.what() << '\n';
	return 1;
} catch (...) {
	std::cerr << "Unknown error" << '\n';
	return 2;
}
