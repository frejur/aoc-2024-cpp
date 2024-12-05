#include "day_02.h"

namespace {
constexpr int min_delta{1};
constexpr int max_delta{3};
} // namespace

std::vector<int> aoc24_02::unsafe_indices(
    const std::vector<std::vector<int>>& reports)
{
	std::vector<int> idx_v;
	for (size_t i = 0; i < reports.size(); ++i) {
		if (!is_safe(reports[i])) {
			idx_v.emplace_back(i);
		}
	}
	return idx_v;
}

//------------------------------------------------------------------------------

bool aoc24_02::delta_is_safe(const int value_a,
                             const int value_b,
                             const bool increase)
{
	int delta{value_b - value_a};

	if (delta == 0 || (increase && delta < 0) || (!increase && delta > 0)) {
		return false;
	}

	delta *= (value_b < value_a) ? -1 : 1;
	return (min_delta <= delta && delta <= max_delta);
}

bool aoc24_02::is_safe(const std::vector<int>& report, const size_t skip_idx)
{
	const bool skip = (skip_idx > -1 && skip_idx < report.size());
	bool incr;
	for (size_t i = 0; i < report.size() - 1; ++i) {
		if (skip && i == skip_idx && skip_idx == report.size() - 2) {
			return true;
		}

		int offs_this = static_cast<int>(skip && i == skip_idx);
		int offs_next = static_cast<int>(skip && i == skip_idx + 1);
		int this_val = report[i + offs_this];
		int next_val = report[i + 1 + offs_next];

		if (i == 0) {
			incr = (next_val >= this_val);
		}

		if (!delta_is_safe(this_val, next_val, incr)) {
			return false;
		}
	}

	return true;
}
