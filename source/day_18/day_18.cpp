#include "../lib/grid.h"
#include "../lib/puzzle.h"
#include "../lib/vec2d.h"
#include "bytegrid.h"
#include "io.h"
#include "point.h"
#include "solve.h"

int main(int argc, char* argv[])
try {
	using namespace aoc24_18;
	using aoc24::Char_grid;
	using aoc24::Vec2d;

	aoc24::Puzzle pz{18, "RAM Run", argc, argv};

	// Determine size of grid input
	constexpr size_t sz_test{7};
	constexpr size_t sz_inp{71};
	const size_t sz = (pz.is_testing() ? sz_test : sz_inp);
	auto bitset = (pz.is_testing())
	                  ? aoc24::Bitset_factory::create<sz_test * sz_test>()
	                  : aoc24::Bitset_factory::create<sz_inp * sz_inp>();

	// Read input from disk
	const size_t num_bytes = pz.is_testing() ? 25 : 3450;
	std::vector<Vec2d> byte_pos_v{
	    read_coordinates_from_file(pz.input_file_path(), num_bytes)};

	Vec2d start_xy{0, 0};
	Vec2d end_xy{static_cast<int>(sz) - 1, static_cast<int>(sz) - 1};

	// Setup grid
	Byte_grid g{std::move(bitset),
	            sz,
	            start_xy.x,
	            start_xy.y,
	            end_xy.x,
	            end_xy.y};
	const size_t p1_num_bytes = pz.is_testing() ? 12 : 1024;
	for (size_t i = 0; i < p1_num_bytes; ++i) {
		g.add_corrupted_byte(byte_pos_v[i].x, byte_pos_v[i].y);
	}
	g.update_corrupted_bytes();
	g.print_map();

	// Part one
	std::vector<Point> path = get_shortest_path(g);
	{
		std::cout << '\n', g.print_path(path);
	}
	pz.file_answer(1,
	               "Min. number of steps",
	               path.empty() ? -1 : path.back().number_of_steps());

	pz.print_answers();

	return 0;
} catch (const std::exception& e) {
	std::cerr << "Error: " << e.what() << '\n';
	return 1;
} catch (...) {
	std::cerr << "Unknown error" << '\n';
	return 2;
}

