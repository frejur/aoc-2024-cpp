#include "day_13.h"
#include <cassert>
#include <fstream>

std::vector<aoc24_13::Machine_buttons_and_prize>
aoc24_13::read_machine_buttons_and_prize_from_file(const std::string& file_path,
                                                   size_t number_of_entries)
{
	using Machine = aoc24_13::Machine_buttons_and_prize;
	std::ifstream ifs{file_path};
	if (!ifs) {
		throw std::ios_base::failure("Could not open sample input file: "
		                             + file_path);
	}

	std::vector<Machine> m_btn_pr_v;
	m_btn_pr_v.reserve(number_of_entries);
	Machine m;
	while (ifs >> m) {
		m_btn_pr_v.emplace_back(m);
	}

	assert(m_btn_pr_v.size() == number_of_entries);
	return m_btn_pr_v;
}

std::istream& aoc24_13::operator>>(std::istream& iss, Vec2d& vec)
{
	char xy{}, op{};
	int val = 0;
	for (int i = 0; i < 2; ++i) {
		if (!(iss >> xy >> op >> val) || !(xy == 'X' || xy == 'Y')
		    || !(op == '+' || op == '=')) {
			iss.clear(std::ios_base::failbit);
			return iss;
		}
		if (i == 0) {
			char comma;
			if (!(iss >> comma) || comma != ',') {
				iss.clear(std::ios_base::failbit);
				return iss;
			}
			vec.x = val;
		} else {
			vec.y = val;
		}
	}
	return iss;
}

std::istream& aoc24_13::operator>>(std::istream& iss,
                                   Machine_buttons_and_prize& m)
{
	std::string str;
	char c{}, colon{};
	Vec2d v;
	for (int i = 0; i < 2; ++i) {
		if (!(iss >> str >> c >> colon >> v)
		    || (str != "Button" || !(c == 'A' || c == 'B') || colon != ':')) {
			iss.clear(std::ios_base::failbit);
			return iss;
		}
		if (i == 0) {
			m.button_a = v;
		} else {
			m.button_b = v;
		}
	}

	if (!(iss >> str) || str != "Prize:") {
		iss.clear(std::ios_base::failbit);
		return iss;
	}

	return iss >> m.prize;
}
