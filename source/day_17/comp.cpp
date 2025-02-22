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
    , init_a_(register_a)
    , init_b_(register_b)
    , init_c_(register_c)
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

void aoc24_17::Computer::run(int stop_after_x_out_instructions)
{
	if (empty_out_) {
		oss_.str("");
	}
	empty_out_ = true;

	int count_out = 0;
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
				++count_out;
				if (stop_after_x_out_instructions > 0
				    && count_out == stop_after_x_out_instructions) {
					return;
				}
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

void aoc24_17::Computer::set_register_value(char id, long val)
{
	if (id != 'a' && id != 'b' && id != 'c') {
		throw std::invalid_argument("Invalid register ID");
	}
	*(reg_[id - 'a']) = val;
}

long aoc24_17::Computer::get_register_value(char id) const
{
	if (id != 'a' && id != 'b' && id != 'c') {
		throw std::invalid_argument("Invalid register ID");
	}
	return *reg_[id - 'a'];
}

std::string aoc24_17::Computer::instructions_string() const
{
	std::string s;
	for (int i = 0; i < num_instr_; ++i) {
		s += (s.empty() ? "" : ",") + std::to_string(instr_[i].first) + ","
		     + std::to_string(instr_[i].second);
	}
	return s;
}

void aoc24_17::Computer::print_register_values(std::ostream& outstr) const
{
	outstr << "A: " << a_ << '\n';
	outstr << "B: " << b_ << '\n';
	outstr << "C: " << c_ << '\n';
}

void aoc24_17::Computer::reset()
{
	ptr_ = 0;
	a_ = init_a_;
	b_ = init_b_;
	c_ = init_c_;
	oss_.str("");
	empty_out_ = true;
}

bool aoc24_17::Computer::can_be_solved() const
{
	if (num_instr_ < 3) {
		return false;
	}

	int count_adv = 0;
	int count_jnz = 0;
	int count_out = 0;
	int operand_adv = -1;
	int operand_jnz = -1;
	int operand_out = -1;
	int pos_adv = -1;
	int pos_jnz = -1;
	int pos_out = -1;
	for (int i = 0; i < num_instr_; ++i) {
		int* count = nullptr;
		int* operand = nullptr;
		int* pos = nullptr;
		switch (instr_[i].first) {
		case adv:
			count = &count_adv;
			operand = &operand_adv;
			pos = &pos_adv;
			break;
		case jnz:
			count = &count_jnz;
			operand = &operand_jnz;
			pos = &pos_jnz;
			break;
		case out:
			count = &count_out;
			operand = &operand_out;
			pos = &pos_jnz;
			break;
		}
		if (count != nullptr)
			++(*count);
		if (operand != nullptr)
			*operand = instr_[i].second;
		if (pos != nullptr)
			*pos = i;
	}
	return (count_adv == 1 || count_jnz == 1 || count_out == 1)
	       && (pos_jnz > pos_adv || pos_out > pos_adv)
	       && (operand_jnz <= pos_adv) && (operand_adv > 0 && operand_adv < 4)
	       && (operand_out >= 4);
}

long aoc24_17::Computer::combo_operand_value(int operand) const
{
	if (operand <= 3) {
		return operand;
	}
	return *reg_[operand - 4];
}
