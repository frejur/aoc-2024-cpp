#include "box.h"
#include <stdexcept>

namespace {
constexpr int min_box_width_and_height{1};
constexpr int max_box_width_and_height{2};
int valid_box_dim(const int dim)
{
	if (dim < min_box_width_and_height || dim > max_box_width_and_height) {
		throw std::invalid_argument("Invalid box width / height");
	}
	return dim;
}
} // namespace

//------------------------------------------------------------------------------

aoc24_15::Dummy_box aoc24_15::Box_new::Dummy = Dummy_box();

aoc24_15::Box_new::Box_new(
    Warehouse* parent_grid, int pos_x, int pos_y, int w, int h)
    : is_dummy(false)
    , width(valid_box_dim(w))
    , height(valid_box_dim(h))
    , initial_position(pos_x, pos_y)
    , position(pos_x, pos_y)
    , prune_(false)
    , grid_(parent_grid)
{}
