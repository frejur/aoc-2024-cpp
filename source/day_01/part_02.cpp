#include "day_01.h"

int aoc24_01::total_l_r_similarity_score(const std::vector<int>& sorted_l,
                                         const std::vector<int>& sorted_r)
{
	int sim_score = 0;

	size_t l_idx = 0;
	size_t r_idx = 0;
	size_t max_idx = sorted_l.size() - 1;
	while (l_idx <= max_idx) {
		int l_val = sorted_l[l_idx];

		// Count occurences of left value
		int l_count = count_val_at_idx(sorted_l, l_idx);

		while (r_idx < max_idx && sorted_r[r_idx] < l_val) {
			++r_idx; // Skip values unique to the right side
		}

		int r_count = 0;
		if (l_val == sorted_r[r_idx]) {
			// Count occurences of right value == left value
			r_count = count_val_at_idx(sorted_r, r_idx);

			sim_score += l_r_similarity_score(l_count, r_count, sorted_l[l_idx]);
		}

		// Update loop indices
		l_idx += l_count;
		r_idx += r_count;
	}
	return sim_score;
}

//------------------------------------------------------------------------------

int aoc24_01::count_val_at_idx(const std::vector<int>& sorted_v, const int idx)
{
	const int val = sorted_v[idx];
	int num_val = 1;
	while ((idx + num_val < sorted_v.size() - 1)
	       && (val == sorted_v[idx + num_val])) {
		++num_val;
	}
	return num_val;
}

//------------------------------------------------------------------------------

inline int aoc24_01::l_r_similarity_score(const int num_l,
                                          const int num_r,
                                          const int l_val)
{
	return num_l * num_r * l_val;
};
