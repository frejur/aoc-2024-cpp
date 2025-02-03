#include "move.h"
#include "io_utils.h"
#include <fstream>

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
