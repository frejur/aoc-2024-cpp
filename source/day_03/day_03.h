#ifndef DAY_03_H
#define DAY_03_H
#include <string>
#include <vector>

namespace aoc24_03 {

std::string read_input_into_string(const std::string& path_to_input);

//------------------------------------------------------------------------------
// Part 1 (definitions in `part_01.cpp`)

struct Mul
{
	Mul(size_t position, int factor_a, int factor_b);
	inline int product() const { return a * b; }
	bool enabled;
	int a;
	int b;
	size_t pos;
};

std::vector<Mul> mul_instr_from_string(const std::string& s);

long int sum_enabled(const std::vector<Mul>& mul_operations);

//------------------------------------------------------------------------------
// Part 2 (definitions in `part_02.cpp`)

struct Toggle
{
	Toggle()
	    : enable(false)
	{}
	bool enable;
};

void disable_based_on_string(std::vector<Mul>& mul_operations,
                             const std::string& s);
size_t disable_based_on_pos(std::vector<Mul>& mul_operations,
                            const size_t mul_start_idx,
                            const size_t char_start_idx,
                            const size_t chart_end_idx);

} // namespace aoc24_03

#endif // DAY_03_H
