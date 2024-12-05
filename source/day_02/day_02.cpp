#include "day_02.h"
#include "../lib/puzzle.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
try {
	using namespace aoc24_02;
	aoc24::Puzzle pz{2, "Red-Nosed Reports", argc, argv};

	constexpr size_t sz_inp{1000};
	constexpr size_t sz_test{6};
	const size_t sz = (pz.is_testing() ? sz_test : sz_inp);

	std::vector<std::vector<int>> reports;
	reports.reserve(sz);
	read_input_into_reports(pz.input_file_path(), reports);
	assert(reports.size() == sz);

	std::vector<int> unsafe_idx_v = unsafe_indices(reports);
	const int num_safe = reports.size() - unsafe_idx_v.size();
	const int num_safe_dampened = count_safe_dampened(reports, unsafe_idx_v);
	const int num_safe_total = num_safe + num_safe_dampened;

	pz.file_answer(1, "Undampened safe reports", num_safe);
	pz.file_answer(2, "Total safe reports", num_safe_total);
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

void aoc24_02::read_input_into_reports(const std::string& p,
                                       std::vector<std::vector<int>>& reports)
{
	std::ifstream ifs{p};
	if (!ifs) {
		throw std::ios_base::failure(
			"Could not open sample input file: " + p);
	}

	std::string ln;
	std::istringstream iss;
	while (std::getline(ifs, ln)) {
		iss.clear(std::ios_base::goodbit);
		int lvl = 0;
		iss.str(ln);
		std::vector<int> lvl_v;
		while (iss >> lvl) {
			lvl_v.emplace_back(lvl);
		}
		reports.emplace_back(lvl_v);
	}
}
