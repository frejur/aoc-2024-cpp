#include "day_07.h"
#include <algorithm>

std::vector<char> aoc24_07::operators(
    const std::vector<std::string>& permutations)
{
	char prev_c = permutations.front().front();
	std::vector<char> ch_v;
	for (auto it = permutations.begin(); it != permutations.end(); ++it) {
		if ((*it).front() == prev_c) {
			continue;
		}
		ch_v.push_back((*it).front());
	}
	return ch_v;
}
//------------------------------------------------------------------------------
long long aoc24_07::merge(long long a, long long b)
{
	long long p = 1;
	while (p <= b) {
		p *= 10;
	}
	return a * p + b;
}

//------------------------------------------------------------------------------

void aoc24_07::add_permutation_member(std::vector<std::string>& permutations,
                                      char member)
{
	for (auto op : operators(permutations)) {
		if (member <= op) {
			throw std::invalid_argument(
			    "The member to be added must be lexiographically greater than "
			    "existing members, but "
			    + std::string{member} + " is <= " + std::string{op});
		}
	}
	// Get existing combinations
	std::vector<std::string> combinations = permutations;
	for (std::string& unsorted : combinations) {
		std::sort(unsorted.begin(), unsorted.end());
	}
	sort(combinations.begin(), combinations.end());
	combinations.erase(std::unique(combinations.begin(), combinations.end()),
	                   combinations.end());
	const size_t old_comb_sz = combinations.size();

	// Use existing combinations to generate new combinations
	const size_t op_sz = permutations.front().size();
	const size_t perm_sz = permutations.size();
	for (size_t i = 0; i < perm_sz; ++i) {
		const std::string& c = combinations[i];
		std::string temp = c;
		int int_i = static_cast<int>(op_sz);
		for (int j = int_i - 1; j >= 1; --j) {
			temp[static_cast<size_t>(j)] = member;
			std::sort(temp.begin(), temp.end());
			combinations.emplace_back(temp);
		}
	}
	combinations.emplace_back(std::string(op_sz, member));

	// Remove duplicate combinations
	std::sort(combinations.begin() + old_comb_sz, combinations.end());
	combinations.erase(std::unique(combinations.begin() + old_comb_sz,
	                               combinations.end()),
	                   combinations.end());

	// Use new combinations to generate and add new permutations
	for (auto it = combinations.begin() + old_comb_sz; it != combinations.end();
	     ++it) {
		do {
			permutations.emplace_back(*it);
		} while (std::next_permutation((*it).begin(), (*it).end()));
	}
}
