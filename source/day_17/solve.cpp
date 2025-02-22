#include "solve.h"
#include <cmath>
#include <set>

namespace {
constexpr long noval{-1};
constexpr long nominmax{-1};
constexpr long nodivisor{-1};
} // namespace

long aoc24_17::reverse_engineer_instructions(Computer& comp)
{
	if (!comp.can_be_solved()) {
		throw std::logic_error(
		    "The provided input data cannot be solved by this program");
	}

	Min_max_values same_length_mm{find_range_of_same_length_values(comp)};

	// DEBUG
	const long reg_a_span{same_length_mm.max_value_found
	                      - same_length_mm.min_value_found};
	const std::string target_str = comp.instructions_string();
	std::cout << "Found range of values that will generate output of the same "
	             "length as the instructions"
	          << '\n';
	std::cout << "Target (Instructions): " << target_str << '\n';
	std::cout << "Number of digits: " << (target_str.length() / 2 + 1)
	          << ", Min: " << same_length_mm.min_value_found
	          << ", Max: " << same_length_mm.max_value_found
	          << ", Span: " << reg_a_span << "\n\n";
	// DEBUG

	constexpr int init_num_digits_to_match{5};
	constexpr int final_num_digits_to_match{init_num_digits_to_match + 0};
	Min_max_values partial_match_mm{
	    find_range_of_partial_matches(comp,
	                                  same_length_mm.min_value_found,
	                                  same_length_mm.max_value_found,
	                                  init_num_digits_to_match)};

	long match{
	    brute_force_using_common_divisor(comp,
	                                     partial_match_mm.min_value_found,
	                                     partial_match_mm.max_value_found,
	                                     final_num_digits_to_match)};

	return match;
}

long aoc24_17::random_unique_value(
    std::mt19937& generator,
    std::uniform_int_distribution<long>& full_range_distribution,
    const std::set<long>& existing_values)
{
	long rand{0};
	for (;;) {
		rand = full_range_distribution(generator);
		if (existing_values.find(rand) == existing_values.end()) {
			break;
		}
	}
	return rand;
}

long aoc24_17::random_unique_multiple_value(
    long start_value,
    int multiplier,
    std::mt19937& generator,
    std::uniform_int_distribution<long>& multiples_distribution,
    const std::set<long>& existing_values)
{
	long rand{0};
	for (;;) {
		rand = start_value + multiplier * multiples_distribution(generator);
		if (existing_values.find(rand) == existing_values.end()) {
			break;
		}
	}
	return rand;
}

aoc24_17::Min_max_values aoc24_17::find_pow_2_bounds(Computer& comp,
                                                     long min_a,
                                                     long max_a,
                                                     long start_min_a,
                                                     long start_max_a,
                                                     int divisor,
                                                     int num_digits_to_match,
                                                     bool match_both_ends)
{
	const std::string target_str{comp.instructions_string()};
	num_digits_to_match = std::min(num_digits_to_match,
	                               static_cast<int>(target_str.size()));
	const std::string target_sub_str{
	    trunc_instr(target_str, num_digits_to_match)};
	const std::string target_sub_str_end = (!match_both_ends)
	                                           ? ""
	                                           : trunc_instr(target_str,
	                                                         num_digits_to_match,
	                                                         true);

	long min_non_matching{min_a};
	long min_matching{(std::max)(min_non_matching, start_min_a)};
	auto calc_min_delta = [&]() { return min_matching - min_non_matching; };
	for (long min_delta = calc_min_delta(); min_delta > divisor;
	     min_delta = calc_min_delta()) {
		long min_delta_mod{min_delta / divisor};
		long test_a{min_matching - (std::max)(1L, min_delta_mod / 2L) * divisor};
		comp.reset();
		comp.set_register_value('a', test_a);

		if (!match_both_ends) {
			comp.run(num_digits_to_match);
		} else {
			comp.run();
		}

		std::string output = comp.get_output();
		bool match{false};
		if (!match_both_ends) {
			match = output == target_sub_str;
		} else {
			match = (target_sub_str == trunc_instr(output, num_digits_to_match))
			        && (target_sub_str_end
			            == trunc_instr(output, num_digits_to_match, true));
		}

		if (match) {
			min_matching = test_a;
		} else {
			min_non_matching = test_a;
		}
	}

	long max_non_matching{max_a};
	long max_matching{(std::min)(max_non_matching, start_max_a)};
	auto calc_max_delta = [&]() { return max_non_matching - max_matching; };
	for (long max_delta = calc_max_delta(); max_delta > divisor;
	     max_delta = calc_max_delta()) {
		long max_delta_mod{max_delta / divisor};
		long test_a = max_matching
		              + (std::max)(1L, max_delta_mod / 2L) * divisor;
		comp.reset();
		comp.set_register_value('a', test_a);

		if (!match_both_ends) {
			comp.run(num_digits_to_match);
		} else {
			comp.run();
		}

		std::string output = comp.get_output();
		bool match{false};
		if (!match_both_ends) {
			match = output == target_sub_str;
		} else {
			match = (target_sub_str == trunc_instr(output, num_digits_to_match))
			        && (target_sub_str_end
			            == trunc_instr(output, num_digits_to_match, true));
		}

		if (match) {
			max_matching = test_a;
		} else {
			max_non_matching = test_a;
		}
	}

	return {min_matching, max_matching};
}

aoc24_17::Min_max_values aoc24_17::find_range_of_same_length_values(
    Computer& comp)
{
	// The single `adv` instruction will divide the value of the A register by
	// either '2', '4' or '8'. Determine which.
	constexpr long adv_const{8};

	comp.reset();
	comp.set_register_value('a', adv_const);
	comp.run(1);

	long reg_a_growth_f = adv_const / comp.get_register_value('a');

	// Calculate min. and max. values of Register A to output X number of digits
	const int num_digits = comp.number_of_instructions() * 2;
	const long reg_a_min{
	    static_cast<long>(std::pow(reg_a_growth_f, num_digits - 1))};
	const long reg_a_max{static_cast<long>(std::pow(reg_a_growth_f, num_digits))
	                     - 1};

	return {reg_a_min, reg_a_max};
}

aoc24_17::Min_max_values aoc24_17::find_range_of_partial_matches(
    Computer& comp, long min_a, long max_a, int num_digits_to_match)
{
	constexpr int sample_sz{10};

	const std::string target_str{comp.instructions_string()};

	// Prepare random number generation
	std::random_device rd;
	std::mt19937 mt{rd()};
	std::uniform_int_distribution<long> distr_full{min_a, max_a};

	const std::string target_sub_str{
	    trunc_instr(target_str, num_digits_to_match)};
	const std::string target_sub_str_end = trunc_instr(target_str,
	                                                   num_digits_to_match,
	                                                   true);

	// Generate initial set of values that produce output matching the start
	// of the target string
	long init_min_start{nominmax}, init_max_start{nominmax};
	long init_min_end{nominmax}, init_max_end{nominmax};
	std::set<long> init_vals_start;
	std::set<long> init_vals_end;
	while (init_vals_start.size() < sample_sz
	       && init_vals_end.size() < sample_sz) {
		long rand_start = random_unique_value(mt, distr_full, init_vals_start);
		std::string output = output_for_a(comp, rand_start);
		if (trunc_instr(output, num_digits_to_match) == target_sub_str) {
			std::cout << "Found start: " << init_vals_start.size() << '\n';
			init_vals_start.insert(rand_start);
			init_min_start = (init_min_start == nominmax)
			                     ? rand_start
			                     : (std::min)(init_min_start, rand_start);
			init_max_start = (init_max_start == nominmax)
			                     ? rand_start
			                     : (std::max)(init_min_start, rand_start);
		}
		long rand_end = random_unique_value(mt, distr_full, init_vals_end);
		output = output_for_a(comp, rand_end);
		if (1 == 2
		    && trunc_instr(output, num_digits_to_match, true)
		           == target_sub_str_end) {
			std::cout << "Found end: " << init_vals_end.size() << '\n';
			init_vals_end.insert(rand_end);
			init_min_end = (init_min_end == nominmax)
			                   ? rand_end
			                   : (std::min)(init_min_end, rand_end);
			init_max_end = (init_max_end == nominmax)
			                   ? rand_end
			                   : (std::max)(init_min_end, rand_end);
		}
	}
	std::cout << '\n';

	bool found_start = (init_vals_start.size() == sample_sz);
	long init_min = (found_start) ? init_min_start : init_min_end;
	long init_max = (found_start) ? init_max_start : init_max_end;
	const std::set<long>& init_vals = (found_start) ? init_vals_start
	                                                : init_vals_end;

	std::cout
	    << "Found range of values with output matching the first and last "
	    << num_digits_to_match << " digits:" << '\n';
	std::cout << "Min: " << init_min << " => " << output_for_a(comp, init_min)
	          << '\n';
	std::cout << "Max: " << init_max << " => " << output_for_a(comp, init_max)
	          << "\n\n";

	int common_div{find_lowest_common_pow_2_divisor(init_vals, -init_min)};
	std::cout << "The lowest common divisor of the initial values: "
	          << common_div << "\n\n";

	Min_max_values final_mm{find_pow_2_bounds(comp,
	                                          min_a,
	                                          max_a,
	                                          init_min,
	                                          init_max,
	                                          common_div,
	                                          num_digits_to_match)};

	std::cout << "Used common divisor to narrow down range to:" << '\n';
	std::cout << "Min: " << final_mm.min_value_found << " => "
	          << output_for_a(comp, final_mm.min_value_found) << '\n';
	std::cout << "Max: " << final_mm.max_value_found << " => "
	          << output_for_a(comp, final_mm.max_value_found) << "\n\n";

	// Generate a set of values that produce output matching the end
	// of the target string
	long span_mult{(max_a - min_a) / common_div};
	std::uniform_int_distribution<long> distr_mult{0, span_mult};
	const int num_end_digits_to_match{num_digits_to_match};
	long end_min{nominmax}, end_max{nominmax};
	std::set<long> end_vals;
	while (end_vals.size() < sample_sz) {
		long rand = random_unique_multiple_value(init_min,
		                                         common_div,
		                                         mt,
		                                         distr_mult,
		                                         init_vals);
		std::string output = output_for_a(comp, rand);
		if ((target_sub_str == trunc_instr(output, num_digits_to_match))
		    && (target_sub_str_end
		        == trunc_instr(output, num_digits_to_match, true))) {
			end_vals.insert(rand);
			end_min = (end_min == nominmax) ? rand : (std::min)(end_min, rand);
			end_max = (end_max == nominmax) ? rand : (std::max)(end_min, rand);
		}
	}
	std::cout << "Found range of values with output matching the last "
	          << num_end_digits_to_match << " digits:" << '\n';
	std::cout << "Min: " << end_min << " => " << output_for_a(comp, end_min)
	          << '\n';
	std::cout << "Max: " << end_max << " => " << output_for_a(comp, end_max)
	          << "\n\n";

	return {end_min, end_max};
}

long aoc24_17::gcd(long a, long b)
{
	if (a == 0 || b == 0) {
		throw std::invalid_argument("Cannot calculate GCD with 0");
	}
	a = std::abs(a);
	b = std::abs(b);
	while (b != 0) {
		long temp = b;
		b = a % b;
		a = temp;
	}
	return a;
}

// int aoc24_17::find_lowest_common_pow_2_divisor(const std::set<long>& values,
//                                                int offset)
// {
// 	if (values.empty()) {
// 		throw std::invalid_argument("Find lowest common divisor: "
// 		                            "No values passed to function");
// 	}

// 	long lcd = 0; // Initialize to 0, will be updated in the first iteration

// 	for (long val : values) {
// 		val += offset;
// 		if (val <= 0) {
// 			throw std::logic_error("Cannot calculate divisor for a value <= 0");
// 		}

// 		if (lcd == 0) {
// 			lcd = val; // Initialize with the first value
// 		} else {
// 			lcd = gcd(lcd, val); // Calculate GCD iteratively
// 		}
// 	}

// 	if (lcd == 1) {
// 		throw std::logic_error("The values have no common divisor");
// 	}

// 	return lcd;
// }

long aoc24_17::largest_pow_2_divisor(long n)
{
	if (n == 0) {
		throw std::invalid_argument("Cannot calculate divisor of 0");
	}

	long power_of_two = 1;
	while ((n & power_of_two) == 0) {
		power_of_two <<= 1;
		if (power_of_two == 0) {
			return nodivisor;
		}
	}
	return power_of_two >> 1;
}

int aoc24_17::find_lowest_common_pow_2_divisor(const std::set<long>& values,
                                               int offset)
{
	if (values.empty()) {
		throw std::invalid_argument("Find lowest common power of two divisor: "
		                            "No values passed to function");
	}

	long min_largest_pow = nodivisor;

	for (long val : values) {
		val += offset;
		if (val <= 0) {
			throw std::logic_error(
			    "Cannot calculate power of two divisor for a value <= 0");
		}

		// Find the largest power of 2 that divides the current value
		long largest_power = largest_pow_2_divisor(val);

		if (min_largest_pow == nodivisor) {
			min_largest_pow = largest_power;
		} else {
			min_largest_pow = std::min(min_largest_pow, largest_power);
		}
	}

	if (min_largest_pow == nodivisor) {
		throw std::logic_error("The values have no power of two divisor");
	}

	return min_largest_pow;
}

long aoc24_17::brute_force_using_common_divisor(Computer& comp,
                                                long min_a,
                                                long max_a,
                                                int num_digits_to_match)
{
	std::cout << "Proceeding to brute force..." << "\n\n";

	constexpr int sample_sz{10};
	const std::string target_str{comp.instructions_string()};
	// Prepare random number generation
	std::random_device rd;
	std::mt19937 mt{rd()};
	std::uniform_int_distribution<long> distr_full{min_a, max_a};

	std::cout << "Attempting to find " << sample_sz
	          << " values with output matching the first "
	          << num_digits_to_match << " characters..." << '\n';

	// Generate initial set of values that produce output matching the start
	// of the target string
	long init_min{nominmax}, init_max{nominmax};
	std::set<long> vals;
	vals.insert(min_a);
	vals.insert(max_a);

	std::cout << '1' << ":\t" << "Found: " << min_a << " => "
	          << output_for_a(comp, min_a) << '\n';
	std::cout << '2' << ":\t" << "Found: " << max_a << " => "
	          << output_for_a(comp, max_a) << '\n';

	while (vals.size() < sample_sz) {
		const std::string target_substr_start{
		    trunc_instr(target_str, num_digits_to_match)};
		const std::string target_substr_end{
		    trunc_instr(target_str, num_digits_to_match, true)};
		long rand = random_unique_value(mt, distr_full, vals);
		const std::string output = output_for_a(comp, rand, num_digits_to_match);
		if (output == target_substr_start) {
			std::cout << (vals.size() + 1) << ":\t" << "Found: " << rand
			          << " => " << output_for_a(comp, rand) << '\n';
			vals.insert(rand);
			init_min = (init_min == nominmax) ? rand
			                                  : (std::min)(init_min, rand);
			init_max = (init_max == nominmax) ? rand
			                                  : (std::max)(init_min, rand);
		}
	}

	min_a = (std::min)(init_min, min_a);
	max_a = (std::max)(init_max, max_a);

	std::cout << '\n';
	std::cout << "Found the following range:" << '\n';
	std::cout << "Min: " << min_a << " => " << output_for_a(comp, min_a)
	          << '\n';
	std::cout << "Max: " << max_a << " => " << output_for_a(comp, max_a)
	          << "\n\n";

	int common_div{find_lowest_common_pow_2_divisor(vals, -min_a)};
	std::cout << "The lowest common divisor of the initial values: "
	          << common_div << "\n\n";

	// Find bounds
	Min_max_values final_mm{find_pow_2_bounds(comp,
	                                          0,
	                                          std::numeric_limits<long>::max(),
	                                          min_a,
	                                          max_a,
	                                          common_div,
	                                          num_digits_to_match,
	                                          true)};

	std::cout << "Used common divisor to narrow down range to:" << '\n';
	std::cout << "Min: " << final_mm.min_value_found << " => "
	          << output_for_a(comp, final_mm.min_value_found) << '\n';
	std::cout << "Max: " << final_mm.max_value_found << " => "
	          << output_for_a(comp, final_mm.max_value_found) << "\n\n";

	std::cout
	    << "Using common divisor to step through range and find final value..."
	    << '\n';

	for (long a = final_mm.min_value_found; a <= final_mm.max_value_found;
	     a += common_div) {
		std::string output{output_for_a(comp, a)};
		if (output_for_a(comp, a) == target_str) {
			std::cout << "Found final match: " << a << " => "
			          << output_for_a(comp, a) << '\n';
			std::string empty_str(std::to_string(a).size(), ' ');
			std::cout << "This should match: " << empty_str << " => "
			          << output_for_a(comp, a) << '\n';
			return a;
		}
	}

	std::cout
	    << "Failed to find a value whose output matches the target string."
	    << '\n';

	return noval;
}
