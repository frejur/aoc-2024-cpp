#include "tests.h"
#include "comp.h"
#include "solve.h"
#include <iomanip>
#include <iostream>

void aoc24_17::run_tests(Computer& comp)
{
	auto same_len_bnds{find_range_of_same_length_values(comp)};
	// const long a_start_val{same_len_bnds.min_value_found + 23245235};
	const long a_start_val{265601188299675L - 2};
	// const long a_start_val{108107566389757L};
	for (int i = 1; i <= 16; ++i) {
		std::cout << "--- " << i << "---" << '\n';
		for (long a = a_start_val + 2; a <= same_len_bnds.max_value_found;
		     a = a_start_val + (a - a_start_val) * 2) {
			comp.reset();
			comp.set_register_value('a', a);
			comp.run(1);
			long b_init{comp.get_register_value('b')};
			long c_init{comp.get_register_value('c')};
			comp.reset();
			comp.set_register_value('a', a);
			comp.run(i);
			long b_end{comp.get_register_value('b')};
			long c_end{comp.get_register_value('c')};

			double b = (b_end == 0) ? 0 : b_init / (double) b_end;
			double c = (c_end == 0) ? 0 : c_init / (double) c_end;

			std::cout << std::setw(20) << (a - a_start_val) << ":\t";
			std::cout << "a: " << std::setw(20) << a << '\t';
			std::cout << "b: " << std::setw(20) << b << '\t';
			std::cout << "c: " << std::setw(20) << c << '\t';
			std::cout << "=> " << comp.get_output() << '\n';
		}
		std::cout << '\n';
	}
}
