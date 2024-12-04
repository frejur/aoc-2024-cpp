#include "day_01.h"
#include "../lib/puzzle.h"
#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <vector>

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
try {
	using namespace aoc24_01;
	aoc24::Puzzle pz{1, "Historian Hysteria", argc, argv};

	constexpr size_t sz_inp{1000};
	constexpr size_t sz_test{6};
	const size_t sz = (pz.is_testing() ? sz_test : sz_inp);

	std::vector<int> left_v, right_v;
	left_v.reserve(sz), right_v.reserve(sz);
	read_input_into_l_r(pz.input_file_path(), left_v, right_v);
	assert(left_v.size() == sz && right_v.size() == sz);

	std::sort(left_v.begin(), left_v.end());
	std::sort(right_v.begin(), right_v.end());

	int p1_dist = total_l_r_dist(left_v, right_v);
	int p2_sim_score = total_l_r_similarity_score(left_v, right_v);

	pz.file_answer(1, "Total distance", p1_dist);
	pz.file_answer(2, "Total similarity score", p2_sim_score);
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

void aoc24_01::read_input_into_l_r(const std::string& p,
                                   std::vector<int>& left_v,
                                   std::vector<int>& right_v)
{
	std::ifstream ifs{p};
	if (!ifs) {
		throw std::ios_base::failure(
			"Could not open sample input file: " + p);
	}

	int left_val = 0;
	int right_val = 0;
	while (ifs >> left_val >> right_val) {
		left_v.push_back(left_val);
		right_v.push_back(right_val);
	}
}
