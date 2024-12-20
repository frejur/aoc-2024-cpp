#include "../lib/puzzle.h"
#include "pluto_sim.h"
#include <cassert>
#include <fstream>
#include <vector>

std::vector<aoc24_11::Stone> read_stones_from_file(const std::string& file_path,
                                                   size_t number_of_stones);
//------------------------------------------------------------------------------

int main(int argc, char* argv[])
try {
	using namespace aoc24_11;

	aoc24::Puzzle pz{11, "Plutonian Pebbles", argc, argv};
	const size_t init_num_stones = pz.is_testing() ? 2 : 8;

	const std::vector<Stone> init_stones{
	    read_stones_from_file(pz.input_file_path(), init_num_stones)};

	Plutonian_simulator sim;
	for (const Stone& s : init_stones) {
		sim.add_stone(s);
	}

	long long p1_num_stones_25{0};

	do {
		sim.blink();
		if (sim.number_of_blinks() == 25) {
			p1_num_stones_25 = sim.number_of_stones();
		}
	} while (sim.number_of_blinks() < 75);

	long long p2_num_stones_75{sim.number_of_stones()};

	pz.file_answer(1, "Stones after 25 blinks", p1_num_stones_25);
	pz.file_answer(2, "Stones after 75 blinks", p2_num_stones_75);

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

std::vector<aoc24_11::Stone> read_stones_from_file(const std::string& file_path,
                                                   size_t number_of_stones)
{
	using aoc24_11::Stone;
	std::ifstream ifs{file_path};
	if (!ifs) {
		throw std::ios_base::failure("Could not open sample input file: "
		                             + file_path);
	}

	std::vector<Stone> stones;
	stones.reserve(number_of_stones);
	Stone s(0);
	while (ifs >> s) {
		stones.emplace_back(s);
	}

	assert(stones.size() == number_of_stones);
	return stones;
}
