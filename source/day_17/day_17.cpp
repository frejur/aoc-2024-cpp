#include "../lib/puzzle.h"
#include "comp.h"
#include "io.h"
#include "solve.h"
#include "tests.h"
#include <cmath>

int main(
    int argc, char* argv[])
try {
	using namespace aoc24_17;
	aoc24::Puzzle pz{17, "Chronospatial Computer", argc, argv};

	std::array<long, 3> reg_vals;
	std::vector<std::pair<int, int>> instr;
	get_register_values_and_instructions_from_file(pz.input_file_path(),
	                                               reg_vals,
	                                               instr);
	Computer comp{reg_vals[0], reg_vals[1], reg_vals[2]};
	for (auto& i : instr) {
		comp.add_instruction(i.first, i.second);
	}

	// Part one ----------------------------------------------------------------
	comp.run();
	pz.file_answer(1, "Output", comp.get_output());

	// Part two ----------------------------------------------------------------
	Computer quine{0, 0, 0};
	for (auto& i : instr) {
		quine.add_instruction(i.first, i.second);
	}

	long p2_reg_a_val{reverse_engineer_instructions(quine)};
	pz.file_answer(2, "Initial value of A", p2_reg_a_val);

	// Tests -------------------------------------------------------------------
	// run_tests(quine);

	pz.print_answers();
	return 0;
} catch (const std::exception& e) {
	std::cerr << "Error: " << e.what() << '\n';
	return 1;
} catch (...) {
	std::cerr << "Unknown error" << '\n';
	return 2;
}
