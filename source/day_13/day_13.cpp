#include "day_13.h"
#include "../lib/puzzle.h"

namespace {
constexpr int token_cost_a{3};
constexpr int token_cost_b{1};
} // namespace

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
try {
	using namespace aoc24_13;

	aoc24::Puzzle pz{13, "Claw Contraption", argc, argv};
	const size_t sz = pz.is_testing() ? 4 : 320;
	auto m{read_machine_buttons_and_prize_from_file(pz.input_file_path(), sz)};

	long long p1_tokens = 0;
	for (const auto& mm : m) {
		Results r = solve(mm.button_a, mm.button_b, mm.prize);
		if (r.success && r.a <= 100 && r.b <= 100) {
			// Not needed for the input data used at the time of writing this...
			p1_tokens += r.a * token_cost_a + r.b * token_cost_b;
		}
	}

	long long p2_tokens = 0;
	const long long p2_modifier{10000000000000};
	for (auto& mm : m) {
		Vec2d mod_prize = {mm.prize.x + p2_modifier, mm.prize.y + p2_modifier};
		Results r = solve(mm.button_a, mm.button_b, mod_prize);
		if (r.success) {
			p2_tokens += r.a * token_cost_a + r.b * token_cost_b;
		}
	}
	pz.file_answer(1, "Fewest tokens needed", p1_tokens);
	pz.file_answer(2, "Corrected fewest tokens", p2_tokens);
	pz.print_answers();

	return 0;
} catch (const std::exception& e) {
	std::cerr << "Error: " << e.what() << '\n';
	return 1;
} catch (...) {
	std::cerr << "Unknown error" << '\n';
	return 2;
}
