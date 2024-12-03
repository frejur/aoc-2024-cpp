#include "day_01.h"

int aoc24_01::total_l_r_dist(const std::vector<int>& sorted_l,
                             const std::vector<int>& sorted_r)
{
	int dist = 0;

	for (size_t i = 0; i < sorted_l.size(); ++i) {
		dist += l_r_dist(sorted_l[i], sorted_r[i]);
	}

	return dist;
}

//------------------------------------------------------------------------------

int aoc24_01::l_r_dist(const int l, const int r)
{
	int dist{l - r};
	return (dist < 0) ? -dist : dist;
}
