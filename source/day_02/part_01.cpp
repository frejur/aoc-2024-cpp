#include "day_02.h"

namespace {
constexpr int min_delta{1};
constexpr int max_delta{3};
} // namespace

int aoc24_02::count_safe(const std::vector<std::vector<int>>& reports)
{
	int n = 0;
	for (const auto& r : reports) {
		n += static_cast<int>(is_safe(r));
	}
	return n;
}

//------------------------------------------------------------------------------

bool aoc24_02::is_safe(const std::vector<int>& report)
{
	bool inv = false;
	for (size_t i = 0; i < report.size() - 1; ++i) {
		int delta{report[i + 1] - report[i]};
		if (delta == 0) {
			return false;
		}
		if (i == 0) {
			inv = delta < 0;
		} else if (inv) {
			if (delta > 0) {
				return false;
			}
		}
		delta *= inv ? -1 : 1;
		if (min_delta > delta || delta > max_delta) {
			return false;
		}
	}

	return true;
}
