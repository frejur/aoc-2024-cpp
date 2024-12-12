#include "day_07.h"
#include "../lib/puzzle.h"
#include <cassert>
#include <fstream>
#include <sstream>

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
try {
	using namespace aoc24_07;

	aoc24::Puzzle pz{7, "Bridge Repair", argc, argv};

	constexpr size_t sz_inp{850};
	constexpr size_t sz_test{9};
	constexpr size_t max_inp{12};
	constexpr size_t max_test{4};
	const size_t sz = (pz.is_testing() ? sz_test : sz_inp);
	const size_t max_ncount = (pz.is_testing() ? max_test : max_inp);

	std::vector<Equation> eqs{
	    read_equations_from_file(pz.input_file_path(), sz, max_ncount)};

	std::multimap<size_t, std::string> perm = generate_op_permutations(
	    max_ncount - 1);

	long long p1_sum_possible{evaluate_expressions_and_get_sum(eqs, perm)};

	pz.file_answer(1, "Sum of possible", p1_sum_possible);
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

std::vector<aoc24_07::Equation> aoc24_07::read_equations_from_file(
    const std::string& p, size_t number_of_equations, size_t max_number_count)
{
	std::ifstream ifs{p};
	if (!ifs) {
		throw std::ios_base::failure("Could not open sample input file: " + p);
	}

	std::vector<Equation> eq_v;
	eq_v.reserve(max_number_count);

	std::istringstream istr;
	std::string ln;
	Equation eq;
	int max_num{0};
	while (std::getline(ifs, ln)) {
		istr.str(ln), istr >> eq;
		eq_v.emplace_back(eq);
		istr.clear(std::ios_base::goodbit);
	}
	assert(max_num <= max_number_count);
	assert(eq_v.size() == number_of_equations);

	return eq_v;
}
