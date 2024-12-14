#ifndef DAY_04_H
#define DAY_04_H
#include <iostream>
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
	friend std::ostream& operator<<(std::ostream& oss, const Equation& e);

private:
	bool is_extr;
	bool is_true;
	long long val;
	std::vector<int> num;
};

struct Result
{
	Result(long long sum_success_eval, size_t num_success_eval)
	    : sum(sum_success_eval)
	    , count_success(num_success_eval)
	{}
	long long sum;
	size_t count_success;
};

std::vector<std::string> generate_op_permutations(size_t max_operator_count);

size_t count_equations_of_size_n(const std::vector<Equation>& sorted_equations,
                                 size_t n,
                                 bool count_possibly_true_only = false);

Result evaluate_equations_of_size_n(std::vector<Equation>& sorted_equations,
                                    const std::string& operators);

long long evaluate_all_equations_and_get_sum(
    std::vector<Equation>& equations,
    const std::vector<std::string>& operator_permutations,
    size_t max_operator_count);

//------------------------------------------------------------------------------
// Part 2 (definitions in `part_02.cpp`)

std::vector<char> operators(const std::vector<std::string>& permutations);
void add_permutation_member(std::vector<std::string>& permutations, char member);
long long merge(long long a, long long b);

} // namespace aoc24_07

#endif // DAY_04_H
