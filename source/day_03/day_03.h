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
	int a;
	int b;
	size_t pos;
};

std::vector<Mul> mul_instr_from_string(const std::string& s);

long int sum_all_prod(const std::vector<Mul>& mul_operations);

//------------------------------------------------------------------------------
// Part 2 (definitions in `part_02.cpp`)

} // namespace aoc24_03

#endif // DAY_03_H
