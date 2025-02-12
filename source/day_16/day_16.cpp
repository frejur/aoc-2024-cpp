#include "../lib/grid.h"
#include "../lib/puzzle.h"
#include "../lib/vec2d.h"
#include "io.h"
#include "maze.h"
#include "path.h"
#include "solve.h"

int main(int argc, char* argv[])
try {
	using namespace aoc24_16;
	using aoc24::Char_grid;
	using aoc24::Vec2d;

	aoc24::Puzzle pz{16, "Reindeer Maze", argc, argv};

	// Determine size of grid input
	constexpr size_t sz_test{15};
	constexpr size_t sz_inp{141};
	const size_t sz = (pz.is_testing() ? sz_test : sz_inp);
	auto bitset = (pz.is_testing())
	                  ? aoc24::Bitset_factory::create<sz_test * sz_test>()
	                  : aoc24::Bitset_factory::create<sz_inp * sz_inp>();

	// Read input from disk
	Char_grid chars{pz.input_file_path(), sz};

	// Extract start and end positions
	Vec2d start_xy;
	Vec2d end_xy;
	extract_start_and_end_pos(chars, start_xy, end_xy);

	// Setup Maze
	Maze m{std::move(bitset), chars, start_xy.x, start_xy.y, end_xy.x, end_xy.y};

	// Part one
	std::vector<Path> paths = get_shortest_paths(m);
	pz.file_answer(1,
	               "Shortest path score",
	               paths.empty() ? -1 : paths.front().final_score());

	// Part two
	pz.file_answer(2, "Unique best tiles", m.merge_and_count_path_tiles(paths));
	pz.print_answers();

	return 0;
} catch (const std::exception& e) {
	std::cerr << "Error: " << e.what() << '\n';
	return 1;
} catch (...) {
	std::cerr << "Unknown error" << '\n';
	return 2;
}

