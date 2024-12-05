#include "day_03.h"
#include "../lib/puzzle.h"
#include <fstream>
#include <iostream>

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
try {
	using namespace aoc24_03;
	aoc24::Puzzle pz{3, "Mull It Over", argc, argv};

	const std::string inp_s{read_input_into_string(pz.input_file_path())};

	std::vector<Mul> mul_v{mul_instr_from_string(inp_s)};
	long int p1_sum_all{sum_all_prod(mul_v)};

	pz.file_answer(1, "Sum of all operations", p1_sum_all);
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

std::string aoc24_03::read_input_into_string(const std::string& p)
{
	std::ifstream ifs{p};
	if (!ifs) {
		throw std::ios_base::failure("Could not open sample input file: " + p);
	}

	int n = 0;
	std::string s;
	std::string ln;
	while (std::getline(ifs, ln)) {
		s += ((n++ > 0) ? "\n" : "") + ln;
	}

	return s;
}
