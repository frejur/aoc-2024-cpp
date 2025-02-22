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
	void run(int stop_after_x_out_instructions = -1);
	std::string get_output() const { return oss_.str(); }
	std::string instructions_string() const;
	long get_register_value(char id) const;
	void set_register_value(char id, long val);
	void print_register_values(std::ostream& outstr = std::cout) const;
	void reset();

	/**
     * Verifies if the program instructions added to this Computer object 
     * meet the following criteria, which should hold for both the test 
     * input and the provided puzzle input:
     *  - Contains exactly one `jnz`, one `out`, and one `adv` instruction.
     *  - The `jnz` instruction appears after the `adv` and `out` instructions.
     *  - The `out` instruction appears after the `adv` instruction.
     *  - The `jnz` instruction jumps to a position before or at the `adv` instruction.
     *  - The `adv` instruction's operand is not '0' nor does it point to any
     *              register ('1` - '3').
     *  - The `out` instruction's combo operand points to a register (`4` - '6').
     */
	bool can_be_solved() const;
	int number_of_instructions() const { return num_instr_; }

private:
	bool empty_out_;
	int ptr_;
	int num_instr_;
	const long init_a_;
	const long init_b_;
	const long init_c_;
	long a_;
	long b_;
	long c_;
	const std::array<long*, 3> reg_{&a_, &b_, &c_};
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
