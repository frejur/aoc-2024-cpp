#include "day_01.h"
#include <algorithm>
#include <cassert>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

const std::string& aoc24_01::input_file()
{
	static const std::string f{"input/20241201_input.txt"};
	return f;
}

namespace {
constexpr int ttl_w{20}; // Width (including padding) of answer title
}

//------------------------------------------------------------------------------

int main()
try {
	using namespace aoc24_01;

	std::vector<int> left_v, right_v;
	left_v.reserve(1000), right_v.reserve(1000);
	read_input_into_l_r(input_file(), left_v, right_v);
	assert(left_v.size() == 1000 && right_v.size() == 1000);

	std::sort(left_v.begin(), left_v.end());
	std::sort(right_v.begin(), right_v.end());

	int p1_dist = total_l_r_dist(left_v, right_v);

	// Answers
	std::cout << std::left;
	std::cout << std::setw(ttl_w) << "Total distance:" << p1_dist << '\n';

	return 0;
} catch (const std::exception& e) {
	std::cerr << "Error: " << e.what() << '\n';
} catch (...) {
	std::cerr << "Unknown error" << '\n';
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
