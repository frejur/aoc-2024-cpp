#ifndef BOX_GRID_WIDE_H
#define BOX_GRID_WIDE_H
#include "box_grid.h"

namespace aoc24_15 {

class Box_grid_wide : public Box_grid
{
public:
	Box_grid_wide(const aoc24::Char_grid& reference_grid,
	              std::unique_ptr<aoc24::Dyn_bitset> dyn_bitset);
};
} // namespace aoc24_15

#endif // BOX_GRID_WIDE_H
