#include "day_06.h"
#include "../lib/dynbs.h"
#include "../lib/puzzle.h"

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
try {
	using namespace aoc24_06;
	using aoc24::Char_grid;

	aoc24::Puzzle pz{6, "Guard Gallivant", argc, argv};

	constexpr size_t sz_inp{130};
	constexpr size_t sz_test{10};
	const size_t sz = (pz.is_testing() ? sz_test : sz_inp);
	auto bitset = (pz.is_testing())
	                  ? aoc24::Bitset_factory::create<sz_test * sz_test>()
	                  : aoc24::Bitset_factory::create<sz_inp * sz_inp>();

	Char_grid chars(pz.input_file_path(), sz);
	Trail_grid grid{chars, std::move(bitset)};
	Guard gal{spawn_guard(chars)};
	Pos2d start_pos{gal.current_position()};
	Direction start_dir{Direction::North};

	// Part 1
	Move p1_mv = Move(start_pos.x, start_pos.y);
	move_and_mark_until_condition(p1_mv,
	                              gal,
	                              grid,
	                              find_obstruction,
	                              mark_start,
	                              find_obstruction,
	                              no_obstruction_ahead,
	                              move_to_obstruction_and_turn,
	                              move_to_end,
	                              mark_trail);
	pz.file_answer(1, "Unique tiles covered", grid.number_of_marked_tiles());

	// Reset guard / grid
	gal.set_position(start_pos);
	gal.set_direction(start_dir);
	grid.reset_trail();

	// Part 2
	Move p2_mv = Move(start_pos.x, start_pos.y);
	move_and_mark_until_condition(p2_mv,
	                              gal,
	                              grid,
	                              find_obstruction,
	                              mark_start_dir,
	                              find_obstruction,
	                              no_obstruction_ahead,
	                              step_and_check_for_loops,
	                              step_and_check_for_loops,
	                              mark_dir_trail);
	pz.file_answer(2,
	               "Loop-inducing obstructions",
	               grid.number_of_new_obstructions());
	pz.print_answers();

	return 0;
} catch (const std::exception& e) {
	std::cerr << "Error: " << e.what() << '\n';
	return 1;
} catch (...) {
	std::cerr << "Unknown error" << '\n';
	return 2;
}
