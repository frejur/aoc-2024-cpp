#include "opcodes.h"
#include "solve.h"
#include <iostream>
#include <random>
#include <stack>
#include <string>
#include <utility>
#include <vector>

namespace {
constexpr long long noinita{-1};

// target constants
constexpr int reg_a_operand{5};
} // namespace

//------------------------------------------------------------------------------

long long aoc24_17::calc_init_reg_a(Computer& comp)
{
	const std::vector<std::pair<int, int>> instr{comp.instructions()};
	const int total_num_digits{static_cast<int>(instr.size() * 2)};

	const long long x{8};
	using Sim_instr = std::pair<long long, int>;
	std::stack<Sim_instr> sim_stack;
	sim_stack.emplace(Sim_instr{0, total_num_digits - 1});
	while (!sim_stack.empty()) {
		const auto& sim{sim_stack.top()};
		const long long a{sim.first};
		const int digit_idx{sim.second};
		sim_stack.pop();

		for (int bit = 0; bit < x; ++bit) {
			/* Shift left by (1|2|3) bits (mult. by `x`) and add `bit`, e.g.:
		              initial_a = 42      (101 010)
		    a = initial_a << 3 => 336 (101 010 000)
		                    bit = 5   (000 000 101)
		              a |= bit => 341 (101 010 101) */
			const long long prev_a{a * x + static_cast<long long>(bit)};
			const int digit_target{
			    comp.get_digit(Computer::Digit_type::Instruction, digit_idx)};
			const int target_num_digits{total_num_digits - digit_idx};
			const long long next_a{
			    simulate_and_match_digit_for_a(comp,
			                                   prev_a,
			                                   digit_target,
			                                   target_num_digits)};

			if (next_a != noinita) {
				if (digit_idx == 0) {
					return next_a;
				}
				sim_stack.emplace(Sim_instr{next_a, digit_idx - 1});
			}
		}
	}

	return noinita;
}

std::string aoc24_17::generate_solvable_instruction_string(Computer& comp,
                                                           int adv_operand)
{
	using namespace opcodes;
	if (adv_operand < 1 || adv_operand > 3) {
		throw std::invalid_argument("Invalid 'adv' operand");
	}

	std::random_device rd;
	std::mt19937 gen(rd());

	constexpr int max_attempts{50000};
	for (int attempt = 0; attempt < max_attempts; ++attempt) {
		std::vector<std::pair<int, int>> instr_pairs;

		// Random additional instructions
		std::uniform_int_distribution<> extra_count(2, 4);
		int extras = extra_count(gen);

		for (int i = 0; i < extras; i++) {
			static const std::array<int, 5> opc_a{bxl, bst, bxc, bdv, cdv};
			std::uniform_int_distribution<> op_dist(0, opc_a.size() - 1);
			int opcode = opc_a[op_dist(gen)];

			int operand = 0;
			if (opcode == bxl) {
				// Literal operand
				operand = gen() % 7;
			} else if (opcode == bxc) {
				operand = 0; // `bxc` discards the operand
			} else {
				// Combo operand
				operand = (gen() % 3) + 1;
			}

			instr_pairs.push_back({opcode, operand});
		}

		// Target always ends with: `adv,<operand>, out,<register-A>, jnz,0`
		instr_pairs.push_back({adv, adv_operand});
		instr_pairs.push_back({out, reg_a_operand});
		instr_pairs.push_back({jnz, 0});

		std::cout << "Trying to solve: ";
		for (const auto ip : instr_pairs) {
			std::cout << ip.first << "," << ip.second << '\t';
		}
		std::cout << "\n\n";
		if (is_solvable(comp, instr_pairs)) {
			return comp.instructions_string();
		}
	}

	return ""; // No valid combination found
}

long long aoc24_17::simulate_and_match_digit_for_a(Computer& comp,
                                                   long long a,
                                                   int digit,
                                                   int number_of_digits)
{
	using DT = Computer::Digit_type;

	// Single digit match
	comp.reset();
	comp.set_register_value('a', a);
	comp.run(1);
	const int digit_result{comp.get_digit(DT::Output, 0)};

	if (digit_result != digit) {
		return noinita;
	}

	// Output string length match
	comp.reset();
	comp.set_register_value('a', a);
	comp.run();

	if (((comp.get_output().size() + 1) / 2) != number_of_digits) {
		return noinita;
	}

	return a;
}

bool aoc24_17::is_solvable(Computer& comp,
                           const std::vector<std::pair<int, int>>& instr_pairs)
{
	comp.hard_reset();
	for (const auto& ip : instr_pairs) {
		comp.add_instruction(ip.first, ip.second);
	}

	long long a_val = calc_init_reg_a(comp);
	if (a_val == noinita) {
		return false;
	}

	comp.reset();
	comp.set_register_value('a', a_val);
	comp.run();
	return (comp.instructions_string() == comp.get_output());
}
