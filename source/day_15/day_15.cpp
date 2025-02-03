#include "day_15.h"
#include "../lib/dynbs.h"
#include "../lib/grid.h"
#include "../lib/puzzle.h"
#include "keys.h"
#include "r_box.h"
#include "s_box.h"
#include <fstream>
#include <memory>
#include <sstream>

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

	auto bitset_p2_refactor
	    = (pz.is_testing())
	          ? aoc24::Bitset_factory::create<sz_test * sz_test * 2>()
	          : aoc24::Bitset_factory::create<sz_inp * sz_inp * 2>();
	Rect_box_factory rf;
	Warehouse p2_wh{sz * 2, sz, std::move(bitset_p2_refactor), char_grid_wide};

	for (const auto& xy : box_xy_v) {
		p2_wh.add_box(rf, xy.x * 2, xy.y);
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

//------------------------------------------------------------------------------

long long aoc24_15::move_robot_and_get_sum_of_coordinates(
    Warehouse& grid,
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

			auto integr = grid.box_integrity();
			if (integr.has_errors) {
				(*ofs) << "Error" << '\n'
				       << "Box with the init. pos. of "
				       << integr.faulty_box_initial_position
				       << ", the current pos. of " << integr.faulty_box_position
				       << ",\n"
				       << "and pointing to a tile at pos."
				       << integr.faulty_edge_tile_position << '\n'
				       << "Expected " << integr.expected_what << " but found "
				       << integr.found_what << '\n';
				return -1;
			}
		}
	}
	return grid.sum_of_all_box_coordinates();
}

void aoc24_15::extract_spawn_and_box_positions(
    aoc24::Char_grid& grid,
    aoc24::Vec2d& spawn_pos,
    std::vector<aoc24::Vec2d>& box_pos_v)
{
	int num_rob = 0;
	for (size_t row = 0; row < grid.height(); ++row) {
		for (size_t col = 0; col < grid.width(); ++col) {
			bool found_char = false;
			char c = grid.char_at(col, row);
			switch (c) {
			case chars::box_ch:
				box_pos_v.emplace_back(col, row);
				found_char = true;
				break;
			case chars::robot_ch:
				if (num_rob++ > 0) {
					throw std::invalid_argument(
					    "The reference grid contains more than one robot "
					    "character ('"
					    + std::string{c} + "')");
				}
				spawn_pos.x = static_cast<int>(col);
				spawn_pos.y = static_cast<int>(row);
				found_char = true;
				break;
			default:
				break;
			}
			if (found_char) {
				// Replace Box or Robot with empty char
				grid.set_char(static_cast<size_t>(col),
				              static_cast<size_t>(row),
				              chars::empty_ch);
			}
		}
	}
}

std::ostream& aoc24_15::operator<<(std::ostream& ostr, aoc24::Vec2d& vec)
{
	return ostr << '(' << vec.x << ", " << vec.y << ')';
}

aoc24::Char_grid aoc24_15::create_new_wider_grid(
    const aoc24::Char_grid& reference_grid)
{
	aoc24::Char_grid new_grid{'.',
	                          reference_grid.width() * 2,
	                          reference_grid.width()};
	for (size_t row = 0; row < reference_grid.height(); ++row) {
		for (size_t col = 0; col < reference_grid.width(); ++col) {
			char c = reference_grid.char_at(col, row);
			switch (c) {
			case chars::empty_ch:
				break;
			case chars::wall_ch:
				new_grid.set_char(col * 2, row, chars::wall_ch);
				new_grid.set_char(col * 2 + 1, row, chars::wall_ch);
				break;
			default:
				throw std::invalid_argument(
				    "The reference grid contains unknown character '"
				    + std::string{c} + "'.");
			}
		}
	}
	return new_grid;
}
