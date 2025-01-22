#include "day_15.h"
#include "../lib/dynbs.h"
#include "../lib/grid.h"
#include "../lib/puzzle.h"
#include <fstream>
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

	Char_grid char_grid(chars, sz);

	// Part one
	Box_grid_single grid_p1{char_grid, std::move(bitset_p1)};

	long long p1_coord_sum{
	    move_robot_and_get_sum_of_coordinates(grid_p1,
	                                          moves,
	                                          enable_debug,
	                                          debug_output_filename_p1())};
	pz.file_answer(1, "Sum of coordinates", p1_coord_sum);

	Box_grid_wide grid_p2{char_grid, std::move(bitset_p2)};
	long long p2_coord_sum{
	    move_robot_and_get_sum_of_coordinates(grid_p2,
	                                          moves,
	                                          enable_debug,
	                                          debug_output_filename_p2())};
	pz.file_answer(2, "Sum of scaled up warehouse", p2_coord_sum);
	pz.print_answers();

	return 0;
} catch (const std::exception& e) {
	std::cerr << "Error: " << e.what() << '\n';
	return 1;
} catch (...) {
	std::cerr << "Unknown error" << '\n';
	return 2;
}

//------------------------------------------------------------------------------

long long aoc24_15::move_robot_and_get_sum_of_coordinates(
    Box_grid& grid,
    std::queue<aoc24::Direction> moves,
    bool enable_debug,
    const std::string debug_output_filename)
{
	std::unique_ptr<std::ofstream> ofs;
	if (enable_debug) {
		ofs.reset(new std::ofstream(debug_output_filename));
		if (!(*ofs)) {
			throw std::ios_base::failure("Could not open sample input file: ");
		}
		(*ofs) << "Initial state:" << '\n';
		grid.print_map(*ofs);
	}

	const int total_num_moves = static_cast<int>(moves.size());
	while (!moves.empty()) {
		auto& m = moves.front();
		grid.move_robot(m);
		moves.pop();

		if (enable_debug) {
			if (total_num_moves > 1000 && grid.number_of_moves() % 1000 != 0) {
				continue;
			}

			(*ofs) << "Move #" << grid.number_of_moves() << ": " << m << '\n';
			grid.print_map(*ofs);

			if (!grid.boxes_match_maps()) {
				(*ofs) << "Error: The Box objects do not match the maps";
				return -1;
			}
		}
	}
	return grid.sum_of_all_box_coordinates();
}
