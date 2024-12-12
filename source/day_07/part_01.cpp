#include "day_07.h"
#include <algorithm>
#include <string>

std::istream& aoc24_07::operator>>(std::istream& iss, Equation& e)
{
	char c{};

	e.is_extr = false;
	e.is_true = false;

	if (!(iss >> e.val >> c && c == ':')) {
		iss.clear(std::ios_base::failbit);
		return iss;
	}

	int n{0};
	size_t sz{0};
	while (iss >> n) {
		++sz;
		if (e.num.empty() || sz > e.num.size()) {
			e.num.emplace_back(n);
		} else {
			e.num[sz - 1] = n;
		}
	}

	if (e.num.size() > sz) {
		e.num.resize(sz);
	}

	if (sz < 2) {
		iss.clear(std::ios_base::failbit);
		return iss;
	}
	e.is_extr = true;

	return iss;
}

//------------------------------------------------------------------------------

std::multimap<size_t, std::string> aoc24_07::generate_op_permutations(
    size_t max_operator_count)
{
	std::multimap<size_t, std::string> pmap;
	for (size_t i = 1; i <= max_operator_count; ++i) {
		// Generate combinations for `i`
		std::string comb = std::string(i, '*');
		pmap.insert({i, comb});
		int int_i = static_cast<int>(i);
		for (int j = int_i - 1; j >= 1; --j) {
			comb[static_cast<size_t>(j)] = '+';

			// Generate permutations for `i`
			std::string perm = comb;
			do {
				pmap.insert({i, perm});
			} while (std::next_permutation(perm.begin(), perm.end()));
		}
		pmap.insert({i, std::string(i, '+')});
	}
	return pmap;
}

//------------------------------------------------------------------------------
bool aoc24_07::Equation::evaluate(const std::string& operators)
{
	is_true = false;
	if (operators.size() != num.size() - 1) {
		throw std::invalid_argument("Operator count must equal the number of "
		                            "'gaps' between numbers. Expected "
		                            + std::to_string(num.size() - 1)
		                            + " but got "
		                            + std::to_string(operators.size()));
	}
	long long sum = num.front();
	size_t n = 1;
	for (char op : operators) {
		if (op == '+') {
			sum += num[n];
		} else if (op == '*') {
			sum *= num[n];
		} else {
			throw std::logic_error("Found invalid operator char '"
			                       + std::string{op} + " in operator string "
			                       + operators);
		}
		++n;
	}

	if (sum == val) {
		return (is_true = true);
	}

	return false;
}

//------------------------------------------------------------------------------

long long aoc24_07::evaluate_expressions_and_get_sum(
    std::vector<Equation>& equations,
    const std::multimap<size_t, std::string>& operator_permutations)
{
	long long sum = 0;
	for (Equation& e : equations) {
		auto range = operator_permutations.equal_range(e.number_count() - 1);
		for (auto it = range.first; it != range.second; ++it) {
			if (e.evaluate(it->second)) {
				sum += e.test_value();
				break;
			}
		}
	}

	return sum;
}
