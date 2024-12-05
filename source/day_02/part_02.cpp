#include "day_02.h"
namespace {
bool is_safe_skip(const std::vector<int>& report, int skip_idx);
}

int aoc24_02::count_safe_dampened(const std::vector<std::vector<int>>& reports,
                                  const std::vector<int> indices)
{
	int n = 0;
	for (int i : indices) {
		bool is_safe = false;
		for (int j = 0; j < reports[i].size(); ++j) {
			if (is_safe_skip(reports[i], j)) {
				++n;
				break;
			}
		}
	}
	return n;
}

std::vector<int> aoc24_02::unsafe_dampened_indices(
    const std::vector<std::vector<int>>& reports, const std::vector<int> indices)
{
	std::vector<int> idx_v;
	for (int i : indices) {
		if (!is_safe(reports[i], true)) {
			idx_v.emplace_back(i);
		}
	}
	return idx_v;
}

std::vector<int> aoc24_02::unsafe_dampened_indices_brute(
    const std::vector<std::vector<int>>& reports, const std::vector<int> indices)
{
	std::vector<int> idx_v;
	for (int i : indices) {
		bool is_safe = false;
		for (int j = 0; j < reports[i].size(); ++j) {
			if (is_safe_skip(reports[i], j)) {
				is_safe = true;
				break;
			}
		}
		if (!is_safe) {
			idx_v.emplace_back(i);
		}
	}
	return idx_v;
}

namespace {
bool is_safe_skip(const std::vector<int>& report, int skip_idx)
{
	std::vector<int> new_r = report;
	new_r.erase(new_r.begin() + skip_idx);
	bool inv = false;
	bool is_safe = true;
	for (size_t i = 0; i < new_r.size() - 1; ++i) {
		int delta{new_r[i + 1] - new_r[i]};
		if (delta == 0) {
			is_safe = false;
		} else if (i == 0) {
			inv = delta < 0;
		} else if (inv && delta > 0) {
			is_safe = false;
		}

		delta *= inv ? -1 : 1;
		if (!is_safe || 1 > delta || delta > 3) {
			return false;
		}
	}

	return true;
}
} // namespace
