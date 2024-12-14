#include "day_07.h"
#include <algorithm>
#include <string>

std::istream& aoc24_07::operator>>(std::istream& iss, Equation& e)
{
	char c{};

	e.is_extr = false;
	e.is_true = false;

	if (!(iss >> e.val >> c) || c != ':') {
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
std::ostream& aoc24_07::operator<<(std::ostream& oss, const Equation& e)
{
	oss << "Value: " << e.val << ", Numbers: (";
	size_t i = 0;
	for (const auto n : e.num) {
		oss << (i++ == 0 ? "" : ", ") << n;
	}
	oss << ")" << '\n';
	return oss;
}

//------------------------------------------------------------------------------

std::vector<std::string> aoc24_07::generate_op_permutations(
    size_t max_operator_count)
{
	std::vector<std::string> perm_v;
	// Generate combinations for the max count
	std::string comb = std::string(max_operator_count, '*');
	perm_v.emplace_back(comb);
	int int_i = static_cast<int>(max_operator_count);
	for (int j = int_i - 1; j >= 1; --j) {
		comb[static_cast<size_t>(j)] = '+';

		// Generate permutations for `i`
		std::string perm = comb;
		do {
			perm_v.emplace_back(perm);
		} while (std::next_permutation(perm.begin(), perm.end()));
	}
	std::sort(perm_v.begin(), perm_v.end());
	perm_v.emplace_back(std::string(max_operator_count, '+'));
	return perm_v;
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

	// Process '*', '+' and '|'
	long long sum = num.front();
	size_t n = 1;
	for (char op : operators) {
		if (op == '+') {
			sum += num[n];
		} else if (op == '*') {
			sum *= num[n];
		} else if (op == '|') {
			sum = merge(sum, num[n]);
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

size_t aoc24_07::count_equations_of_size_n(
    const std::vector<Equation>& sorted_equations,
    size_t n,
    bool count_possibly_true_only)
{
	size_t count = 0;
	for (const Equation& e : sorted_equations) {
		if (e.number_count() < n) {
			continue;
		}
		if (e.number_count() > n) {
			break;
		}
		if (!count_possibly_true_only || e.possibly_true()) {
			++count;
		}
	}
	return count;
}
aoc24_07::Result aoc24_07::evaluate_equations_of_size_n(
    std::vector<Equation>& sorted_equations, const std::string& operators)
{
	size_t n = operators.size() + 1;
	long long sum = 0;
	size_t count = 0;
	for (Equation& e : sorted_equations) {
		if (e.number_count() < n || e.possibly_true()) {
			continue;
		}
		if (e.number_count() > n) {
			break;
		}
		if (e.evaluate(operators)) {
			sum += e.test_value();
			++count;
		}
	}
	return {sum, count};
}

long long aoc24_07::evaluate_all_equations_and_get_sum(
    std::vector<Equation>& sorted_equations,
    const std::vector<std::string>& operator_permutations,
    size_t max_operator_count)
{
	long long sum = 0;
	std::string prev_perm;
	std::string this_perm;
	Result result(0, 0);
	size_t count_all = 0;
	size_t count_successful = 0;

	// Iterate over all permutations,
	// truncate to `i` and evaluate unique permutations
	for (size_t i = 1; i <= max_operator_count; ++i) {
		// Reset variables
		count_all = count_equations_of_size_n(sorted_equations, i + 1);
		count_successful = count_equations_of_size_n(sorted_equations,
		                                             i + 1,
		                                             true);
		prev_perm.clear();

		for (auto it = operator_permutations.begin();
		     (count_successful < count_all && it != operator_permutations.end());
		     ++it) {
			const auto& p = *it;

			if (i < max_operator_count) {
				if (!prev_perm.empty()
				    && (std::equal(p.begin(),
				                   p.end() - (p.size() - i),
				                   prev_perm.begin()))) {
					continue;
				}
				prev_perm = p;
				result = evaluate_equations_of_size_n(sorted_equations,
				                                      p.substr(0, i));
			} else {
				result = evaluate_equations_of_size_n(sorted_equations, p);
			}

			if (result.count_success) {
				count_successful += result.count_success;
				sum += result.sum;
			}
		}
	}

	return sum;
}
