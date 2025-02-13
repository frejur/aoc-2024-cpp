#ifndef COMP_H
#define COMP_H
#include <array>

namespace aoc24_17 {

class Computer
{
public:
	Computer(int register_a, int register_b, int register_c);

private:
	int a_;
	int b_;
	int c_;
	std::array<int, 100> instr_;
};
} // namespace aoc24_17

#endif // COMP_H
