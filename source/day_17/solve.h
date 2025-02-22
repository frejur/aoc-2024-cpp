#ifndef SOLVE_H
#define SOLVE_H
#include "comp.h"
#include <random>
#include <set>
#include <utility>

namespace aoc24_17 {

struct Min_max_values
{
	Min_max_values(long min, long max)
	    : min_value_found(min)
	    , max_value_found(max)
	{}
	long min_value_found;
	long max_value_found;
};

// Finds the initial value of register `A` that outputs the instructions
// contained in Computer `comp`
long reverse_engineer_instructions(Computer& comp);

// Finds the range of values that should contain the correct value for 'A'
Min_max_values find_range_of_same_length_values(Computer& comp);
Min_max_values find_range_of_partial_matches(Computer& comp,
                                             long min_a,
                                             long max_a,
                                             int num_digits_to_match);

// Generates random values
long random_unique_value(
    std::mt19937& generator,
    std::uniform_int_distribution<long>& full_range_distribution,
    const std::set<long>& existing_values);
long random_unique_multiple_value(
    long start_value,
    int multiplier,
    std::mt19937& generator,
    std::uniform_int_distribution<long>& multiples_distribution,
    const std::set<long>& existing_values);

// Narrows down the range of partially matching values by assuming that:
// - The correct pattern appears at regular intervals.
// - The interval is a power of two.
long largest_pow_2_divisor(long n);

long gcd(long a, long b);
int find_lowest_common_pow_2_divisor(const std::set<long>& values, int offset);
Min_max_values find_pow_2_bounds(Computer& comp,
                                 long min_a,
                                 long max_a,
                                 long start_min_a,
                                 long start_max_a,
                                 int divisor,
                                 int num_digits_to_match,
                                 bool match_both_ends = false);

long brute_force_using_common_divisor(Computer& comp,
                                      long min_a,
                                      long max_a,
                                      int num_digits_to_match);

// Helper functions
inline std::string trunc_instr(const std::string& instr,
                               int num_digits = -1,
                               bool from_end = false)
{
	return (!from_end) ? instr.substr(0, num_digits * 2 - 1)
	                   : instr.substr(instr.size() - (num_digits * 2 - 1));
}

inline std::string output_for_a(Computer& comp,
                                long value,
                                int match_number_of_digits = -1,
                                bool from_end = false)
{
	comp.reset();
	comp.set_register_value('a', value);
	comp.run(from_end ? -1 : match_number_of_digits);
	return (!from_end)
	           ? comp.get_output()
	           : trunc_instr(comp.get_output(), match_number_of_digits, true);
}

} // namespace aoc24_17

#endif // SOLVE_H
