#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

namespace {
const std::string& input_file() {
	static const std::string s{"input/20241201_input.txt"};
	return s;
}

void read_input_into_l_r(const std::string& path_to_input,
		         std::vector<int>& left_vector,
		         std::vector<int>& right_vector);
}

//------------------------------------------------------------------------------

int main()
try {
	std::vector<int> left_v, right_v;
	read_input_into_l_r(input_file(), left_v, right_v);

	std::sort(left_v.begin(), left_v.end());
	std::sort(right_v.begin(), right_v.end());

	int dist_sum = 0;

	for (size_t i = 0; i < left_v.size(); ++i) {
		int dist{left_v[i] - right_v[i]};
		dist_sum += (dist < 0) ? -dist : dist;
		std::cout << left_v[i] << '\t' << right_v[i] << '\t' << dist << '\n';
	}
	std::cout << "Sum: " << dist_sum << '\t';

	return 0;
}
catch (const std::exception& e) {
	std::cerr << "Error: " << e.what() << '\n';
}
catch (...) {
	std::cerr << "Unknown error" << '\n';
}

namespace {
void read_input_into_l_r(const std::string& p,
		         std::vector<int>& left_v,
		         std::vector<int>& right_v) {
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
}
