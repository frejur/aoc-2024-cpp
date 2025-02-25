#ifndef IO_H
#define IO_H
#include <array>
#include <string>
#include <utility>
#include <vector>

namespace aoc24_17 {

struct Register
{
	Register()
	    : value(0)
	    , id{}
	{}
	long long value;
	char id;
};

struct Program
{
	Program() {}
	std::vector<std::pair<int, int>> instructions;
};

std::istream& operator>>(std::istream& istr, Register& reg);
std::istream& operator>>(std::istream& istr, Program& prog);

void get_register_values_and_instructions_from_file(
    const std::string& file_path,
    std::array<long long, 3>& registers,
    std::vector<std::pair<int, int>>& instructions);

} // namespace aoc24_17

#endif
