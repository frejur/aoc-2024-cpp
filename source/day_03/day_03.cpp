#include "day_03.h"
#include "../lib/puzzle.h"
#include <fstream>
#include <iostream>

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
try {
	using namespace aoc24_03;
	aoc24::Puzzle pz{3, "Mull It Over", argc, argv};

	std::string inp_s{read_input_into_string(pz.input_file_path())};

	std::vector<Mul> mul_v{mul_instr_from_string(inp_s)};
	long int p1_sum_all{sum_enabled(mul_v)};

	if (pz.is_testing()) {
		// Use different sample data for part 2 when Testing
		std::string new_p{pz.input_file_path()};
		new_p.resize(pz.input_file_path().size() - 4); // Remove ext
		new_p += "_p2.txt";
		inp_s = read_input_into_string(new_p);
		mul_v = mul_instr_from_string(inp_s);
	}

	disable_based_on_string(mul_v, inp_s);
	long int p2_sum_enabled{sum_enabled(mul_v)};

	pz.file_answer(1, "Sum of all operations", p1_sum_all);
	pz.file_answer(2, "Sum of enabled", p2_sum_enabled);
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
