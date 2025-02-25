#ifndef SOLVE_H
#define SOLVE_H
#include "comp.h"

namespace aoc24_17 {
long long calc_init_reg_a(Computer& comp);
long long simulate_and_match_digit_for_a(Computer& comp,
                                         long long a,
                                         int digit_idx,
                                         int number_of_digits);

std::string generate_solvable_instruction_string(Computer& comp,
                                                 int adv_operand);
bool is_solvable(Computer& comp,
                 const std::vector<std::pair<int, int>>& instr_pairs);

} // namespace aoc24_17

#endif // SOLVE_H
