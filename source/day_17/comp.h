#ifndef COMP_H
#define COMP_H
#include <array>
#include <iostream>
#include <sstream>

namespace aoc24_17 {

class Computer
{
public:
	Computer(long register_a, long register_b, long register_c);
	void add_instruction(int opcode, int operand);
	void run();
	std::string get_output() const { return oss_.str(); }
	void print_register_values(std::ostream& outstr = std::cout) const;

private:
	bool empty_out_;
	int ptr_;
	int num_instr_;
	long a_;
	long b_;
	long c_;
	std::array<std::pair<int, int>, 100> instr_;
	std::ostringstream oss_;

	long combo_operand_value(int operand) const;
	inline bool takes_literal_operand(int opcode) const
	{
		return (opcode == 1 || opcode == 3 || opcode == 4);
	};
};
} // namespace aoc24_17

#endif // COMP_H
