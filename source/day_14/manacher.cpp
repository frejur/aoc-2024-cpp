#include "robo_grid.h"
std::vector<aoc24_14::Palindrome> aoc24_14::Robo_grid::max_w_palindromes(
    const std::vector<int>& value_row, const int min_w) const
{
	std::vector<int> radii = std::vector<int>(value_row.size(), 0);
	size_t len = value_row.size();
	int c = 0;
	int r = 0;

	while (c < value_row.size()) {
		// Find longest palindromes, starting at `c - r` and going to `c + r`
		int lhs = c - (r + 1);
		int rhs = c + (r + 1);
		while (lhs >= 0 && rhs < len
		       && ((value_row[lhs] == 0 && value_row[rhs] == 0)
		           || (value_row[lhs] > 0 && value_row[rhs] > 0))) {
			--lhs;
			++rhs;
			++r;
		}
		radii[c] = r;

		int old_c = c;
		int old_r = r;

		// Increase center position and reset radius for the next step
		++c;
		r = 0;

		while (c <= old_c + old_r) {
			// Center lies inside the old palindrome,
			// use data precomputed for the center's mirrored point
			int mir_c = old_c - (c - old_c);
			int max_mir_r = old_c + old_r - c;

			if (radii[mir_c] < max_mir_r) {
				// Palindrome at `mir_c` is within bounds of the old one
				radii[c] = radii[mir_c];
				++c;
			} else if (radii[mir_c] > max_mir_r) {
				// Palindrome at `mir_c` is out of bounds of the old one
				radii[c] = max_mir_r;
				++c;
			} else {
				// Palindrome at `mir_c` ends at edge of the old one
				r = max_mir_r;
				break;
			}
		}
	}

	std::vector<Palindrome> p_v;
	for (size_t i = 0; i < radii.size(); ++i) {
		if (radii[i] > 0) {
			// Trim zeros
			int new_r = radii[i];
			for (size_t j = i - new_r; j < i; ++j) {
				if (value_row[j] > 0) {
					break;
				}
				--new_r;
			}
			if ((new_r * 2 + 1) >= min_w) {
				p_v.emplace_back(Palindrome{i, new_r});
			}
		}
	}

	return p_v;
}
