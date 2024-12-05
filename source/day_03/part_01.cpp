#include "day_03.h"
#include <regex>
#include <sstream>

aoc24_03::Mul::Mul(size_t position, int factor_a, int factor_b)
    : a(factor_a)
    , b(factor_b)
    , pos(position)
{}

//------------------------------------------------------------------------------

std::vector<aoc24_03::Mul> aoc24_03::mul_instr_from_string(const std::string& s)
{
	std::vector<Mul> mul_v;
	std::regex re{R"(mul\(([0-9]+),([0-9]+)\))"};

	auto mul_begin{std::sregex_iterator(s.begin(), s.end(), re)};
	auto mul_end{std::sregex_iterator()};

	std::istringstream iss;

	for (auto i{mul_begin}; i != mul_end; ++i) {
		size_t pos = static_cast<size_t>(i->position());

		int a, b;
		iss.str((*i).str(1) + " " + (*i).str(2));
		iss >> a >> b;
		iss.clear(std::ios_base::goodbit);

		mul_v.emplace_back(Mul{pos, a, b});
	}

	return mul_v;
}

long aoc24_03::sum_all_prod(const std::vector<Mul>& mul_v)
{
	long int sum = 0;
	for (const auto& m : mul_v) {
		sum += m.product();
	}
	return sum;
}
