#include "../lib/puzzle.h"
#include "robo_grid.h"
#include <cassert>
#include <fstream>

namespace {
constexpr bool enable_debug{true};
}

std::vector<aoc24_14::Robot_position_and_velocity>
read_position_and_velocity_values_from_file(const std::string& file_path,
                                            size_t num_entries);

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
try {
	using namespace aoc24_14;
	using aoc24::Char_grid;
	using RPV = aoc24_14::Robot_position_and_velocity;

	aoc24::Puzzle pz{14, "Restroom Redoubt", argc, argv};

	constexpr size_t num_entries_inp{500};
	constexpr size_t num_entries_test{12};
	const size_t num_entries = (pz.is_testing() ? num_entries_test
	                                            : num_entries_inp);
	const std::vector<RPV> rpv_v
	    = read_position_and_velocity_values_from_file(pz.input_file_path(),
	                                                  num_entries);

	constexpr size_t sz_x_inp{101};
	constexpr size_t sz_x_test{11};
	constexpr size_t sz_y_inp{103};
	constexpr size_t sz_y_test{7};
	const size_t sz_x = (pz.is_testing() ? sz_x_test : sz_x_inp);
	const size_t sz_y = (pz.is_testing() ? sz_y_test : sz_y_inp);
	Robo_grid grid{rpv_v, sz_x, sz_y};

	// Part one
	grid.fast_forward(100);
	int p1_safety_f{grid.safety_factor()};
	pz.file_answer(1, "Safety factor", p1_safety_f);

	// Part two
	long long p2_xmas_tree_sec = 0;
	Vec2d pattern_pos = Vec2d::dummy();

	if (pz.is_testing()) {
		pz.file_answer(2, "Part two", "N/A");
	} else {
		grid.reset_time();
		if (enable_debug) {
			std::cout << "Advancing... Looking for a symmetrical pattern..."
			          << '\n';
		}
		constexpr int max_iter{100000};
		for (size_t i = 1; i <= max_iter; ++i) {
			grid.fast_forward(1);
			pattern_pos = grid.find_symmetrical_pattern();
			if (!pattern_pos.is_dummy()) {
				p2_xmas_tree_sec = grid.current_time();
				pz.file_answer(2, "Xmas tree timestamp", p2_xmas_tree_sec);
				break;
			} else if (enable_debug && i % 1000 == 0) {
				std::cout << " + Advanced " << grid.current_time()
				          << " seconds so far but found nothing" << '\n';
			}
		}

		if (enable_debug && pattern_pos.is_dummy()) {
			std::cout
			    << "Aborted search: Failed to find pattern after advancing "
			    << max_iter << " seconds." << '\n';
		}
	}

	if (enable_debug && !pattern_pos.is_dummy()) {
		std::cout << '\n'
		          << "Found the following pattern at " << p2_xmas_tree_sec
		          << " seconds:"
		          << "\n\n"
		          << '\t';
		grid.print_pattern_at_position(pattern_pos);
		std::cout << '\n';
	}

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

std::vector<aoc24_14::Robot_position_and_velocity>
read_position_and_velocity_values_from_file(const std::string& file_path,
                                            size_t num_entries)
{
	using RPV = aoc24_14::Robot_position_and_velocity;
	std::ifstream ifs{file_path};
	if (!ifs) {
		throw std::ios_base::failure("Could not open sample input file: "
		                             + file_path);
	}

	RPV rpv{0, 0, 0, 0};
	std::vector<RPV> rpv_v;
	rpv_v.reserve(num_entries);
	while (ifs >> rpv) {
		rpv_v.emplace_back(rpv);
	}
	assert(rpv_v.size() == num_entries);
	return rpv_v;
}
