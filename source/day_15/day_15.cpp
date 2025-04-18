#include "../lib/dynbs.h"
#include "../lib/grid.h"
#include "../lib/puzzle.h"
#include "grid_utils.h"
#include "io_utils.h"
#include "move.h"
#include "s_box.h"
#include "warehouse.h"
#include "wd_box.h"
#include <memory>

namespace {
constexpr bool enable_debug{false};
constexpr bool use_smaller_test_sample{false};

const std::string& debug_output_filename_p1()
{
	static const std::string p{"aoc24_15_p_1_debug.txt"};
	return p;
}
const std::string& debug_output_filename_p2()
{
	static const std::string p{"aoc24_15_p_2_debug.txt"};
	return p;
}
} // namespace

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
try {
	using namespace aoc24_15;
	using aoc24::Char_grid;
	using aoc24::Vec2d;

	aoc24::Puzzle pz{15, "Warehouse Woes", argc, argv};

	// Determine size of grid input
	constexpr size_t sz_test = use_smaller_test_sample ? 8 : 10;
	constexpr size_t sz_inp{50};
	const size_t sz = (pz.is_testing() ? sz_test : sz_inp);
	auto bitset_p1 = (pz.is_testing())
	                     ? aoc24::Bitset_factory::create<sz_test * sz_test>()
	                     : aoc24::Bitset_factory::create<sz_inp * sz_inp>();
	auto bitset_p2 = (pz.is_testing())
	                     ? aoc24::Bitset_factory::create<sz_test * sz_test * 2>()
	                     : aoc24::Bitset_factory::create<sz_inp * sz_inp * 2>();

	// Determine number of input moves
	constexpr size_t num_moves_inp{20000};
	constexpr size_t num_moves_test = use_smaller_test_sample ? 15 : 700;
	const size_t num_moves = (pz.is_testing() ? num_moves_test : num_moves_inp);

	// Read input from disk
	std::vector<std::string> chars;
	std::queue<aoc24::Direction> moves;
	read_grid_and_moves_from_file(pz.input_file_path(),
	                              chars,
	                              moves,
	                              sz,
	                              num_moves);

	// Setup reference grid, extract Box and Robot positions
	Char_grid char_grid(chars, sz);
	Vec2d spawn_xy;
	std::vector<Vec2d> box_xy_v;
	extract_spawn_and_box_positions(char_grid, spawn_xy, box_xy_v);

	// Part one
	Simple_box_factory sf;
	Warehouse p1_wh{sz, sz, std::move(bitset_p1), char_grid};

	for (const auto& xy : box_xy_v) {
		p1_wh.add_box(sf, xy.x, xy.y);
	}

	p1_wh.spawn_robot(spawn_xy.x, spawn_xy.y);

	long long p1_sum{
	    move_robot_and_get_sum_of_coordinates(p1_wh,
	                                          moves,
	                                          enable_debug,
	                                          debug_output_filename_p1())};

	pz.file_answer(1, "Sum of coordinates", p1_sum);

	// Part two
	Char_grid char_grid_wide{create_new_wider_grid(char_grid)};

	Wide_box_factory wd_f;
	Warehouse p2_wh{sz * 2, sz, std::move(bitset_p2), char_grid_wide};

	for (const auto& xy : box_xy_v) {
		p2_wh.add_box(wd_f, xy.x * 2, xy.y);
	}

	p2_wh.spawn_robot(spawn_xy.x * 2, spawn_xy.y);

	long long p2_sum{
	    move_robot_and_get_sum_of_coordinates(p2_wh,
	                                          moves,
	                                          enable_debug,
	                                          debug_output_filename_p2())};

	pz.file_answer(2, "Sum of wide Warehouse", p2_sum);

	pz.print_answers();

	return 0;
} catch (const std::exception& e) {
	std::cerr << "Error: " << e.what() << '\n';
	return 1;
} catch (...) {
	std::cerr << "Unknown error" << '\n';
	return 2;
}

