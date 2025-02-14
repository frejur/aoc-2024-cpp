#include "comp.h"
#include <cmath>

namespace {
// opcodes
constexpr int adv{0};
constexpr int bxl{1};
constexpr int bst{2};
constexpr int jnz{3};
constexpr int bxc{4};
constexpr int out{5};
constexpr int bdv{6};
constexpr int cdv{7};
} // namespace

//------------------------------------------------------------------------------

aoc24_17::Computer::Computer(long register_a, long register_b, long register_c)
    : empty_out_(true)
    , ptr_(0)
    , num_instr_(0)
    , a_(register_a)
    , b_(register_b)
    , c_(register_c)
    , instr_{}
{}

void aoc24_17::Computer::add_instruction(int opcode, int operand)
{
	if (opcode < 0 || opcode > 7) {
		throw std::invalid_argument("Invalid opcode");
	}
	if (operand < 0 || operand > 7) {
		throw std::invalid_argument("Invalid operand");
	}
	if (!takes_literal_operand(opcode) && (operand == 7)) {
		throw std::invalid_argument("Invalid combo operand");
	}
	if ((opcode == jnz) && ((operand % 2) == 1)) {
		throw std::invalid_argument("Cannot jump by odd number");
	}

	instr_[num_instr_++] = {opcode, operand};
}

void aoc24_17::Computer::run()
{
	if (empty_out_) {
		oss_.str("");
	}
	empty_out_ = true;
	for (int next_jump = 0; ptr_ < num_instr_; ptr_ = next_jump) {
		next_jump = ptr_ + 1; // Assumes a single jump forward
		const int opcode{instr_[ptr_].first};
		switch (instr_[ptr_].first) {
		case adv:
		case bdv:
		case cdv: {
			const long operand{combo_operand_value(instr_[ptr_].second)};
			long& reg = (opcode == adv) ? a_ : ((opcode == bdv) ? b_ : c_);
			reg = a_ / std::pow(2, operand);
			break;
		}
		case bxl:
		case bxc: {
			const long operand = (opcode == bxl) ? instr_[ptr_].second : c_;
			b_ ^= operand;
			break;
		}
		case bst:
		case out: {
			const long operand{combo_operand_value(instr_[ptr_].second)};
			const long result = operand % 8;
			if (opcode == bst) {
				b_ = result;
			} else {
				if (!empty_out_) {
					oss_ << ',';
				} else {
					empty_out_ = false;
				}
				oss_ << result;
			}
			break;
		}
		case jnz: {
			if (a_ != 0) {
				// Override pointer jump
				const int operand{instr_[ptr_].second};
				next_jump = operand / 2;
			}
			break;
		}
		}
	}
}

void aoc24_17::Computer::print_register_values(std::ostream& outstr) const
{
	outstr << "A: " << a_ << '\n';
	outstr << "B: " << b_ << '\n';
	outstr << "C: " << c_ << '\n';
}

long aoc24_17::Computer::combo_operand_value(int operand) const
{
	if (operand <= 3) {
		return operand;
	}
	const std::array<const long*, 3> reg{&a_, &b_, &c_};
	return *reg[operand - 4];
}
