#include "day_06.h"
#include "../lib/dynbs.h"
#include "../lib/puzzle.h"

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
try {
	using namespace aoc24_06;
	using aoc24::Grid;

	aoc24::Puzzle pz{6, "Guard Gallivant", argc, argv};

	constexpr size_t sz_inp{130};
	constexpr size_t sz_test{10};
	const size_t sz = (pz.is_testing() ? sz_test : sz_inp);
	auto bitset = (pz.is_testing())
	                  ? aoc24::Bitset_factory::create<sz_test * sz_test>()
	                  : aoc24::Bitset_factory::create<sz_inp * sz_inp>();

	aoc24::Char_grid chars(pz.input_file_path(), sz);
	Trail_grid grid{chars, std::move(bitset)};
	Guard gal{spawn_guard(chars)};
	move_and_mark_until_oob(gal, grid);
	pz.file_answer(1, "Unique tiles covered", grid.number_of_marked_tiles());
	pz.print_answers();

	return 0;
} catch (const std::exception& e) {
	std::cerr << "Error: " << e.what() << '\n';
	return 1;
} catch (...) {
	std::cerr << "Unknown error" << '\n';
	return 2;
}
