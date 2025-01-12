#include "box_grid_wide.h"

aoc24_15::Box_grid_wide::Box_grid_wide(
    const aoc24::Char_grid& reference_grid,
    std::unique_ptr<aoc24::Dyn_bitset> dyn_bitset)
    : Box_grid(std::move(dyn_bitset))
{}
