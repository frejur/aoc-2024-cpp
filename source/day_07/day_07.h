#ifndef DAY_04_H
#define DAY_04_H
#include <iostream>
#include <map>
#include <vector>

namespace aoc24_07 {

class Equation; // Fwd. decl.
std::vector<Equation> read_equations_from_file(const std::string& p,
                                               size_t number_of_equations,
                                               size_t max_number_count);

//------------------------------------------------------------------------------
// Part 1 (definitions in `part_01.cpp`)

class Equation
{
public:
	Equation()
	    : is_extr(false)
	    , is_true(false)
	    , val(0)
	{}
	bool evaluate(const std::string& operators);
	bool successfully_extracted() const { return is_extr; }
	bool possibly_true() const { return is_true; }
	long long test_value() const { return val; }
	size_t number_count() const { return num.size(); }

	friend std::istream& operator>>(std::istream& iss, Equation& e);

private:
	bool is_extr;
	bool is_true;
	long long val;
	std::vector<int> num;
};

std::multimap<size_t, std::string> generate_op_permutations(
    size_t max_operator_count);

long long evaluate_expressions_and_get_sum(
    std::vector<Equation>& equations,
    const std::multimap<size_t, std::string>& operator_permutations);

//------------------------------------------------------------------------------
// Part 2 (definitions in `part_02.cpp`)

} // namespace aoc24_07

#endif // DAY_04_H
