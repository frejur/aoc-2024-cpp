#include "grid_utils.h"
#include "keys.h"
#include <stdexcept>

aoc24::Char_grid aoc24_15::create_new_wider_grid(
    const aoc24::Char_grid& reference_grid)
{
	aoc24::Char_grid new_grid{'.',
	                          reference_grid.width() * 2,
	                          reference_grid.width()};
	for (size_t row = 0; row < reference_grid.height(); ++row) {
		for (size_t col = 0; col < reference_grid.width(); ++col) {
			char c = reference_grid.char_at(col, row);
			switch (c) {
			case chars::empty_ch:
				break;
			case chars::wall_ch:
				new_grid.set_char(col * 2, row, chars::wall_ch);
				new_grid.set_char(col * 2 + 1, row, chars::wall_ch);
				break;
			default:
				throw std::invalid_argument(
				    "The reference grid contains unknown character '"
				    + std::string{c} + "'.");
			}
		}
	}
	return new_grid;
}

void aoc24_15::extract_spawn_and_box_positions(
    aoc24::Char_grid& grid,
    aoc24::Vec2d& spawn_pos,
    std::vector<aoc24::Vec2d>& box_pos_v)
{
	int num_rob = 0;
	for (size_t row = 0; row < grid.height(); ++row) {
		for (size_t col = 0; col < grid.width(); ++col) {
			bool found_char = false;
			char c = grid.char_at(col, row);
			switch (c) {
			case chars::box_ch:
				box_pos_v.emplace_back(col, row);
				found_char = true;
				break;
			case chars::robot_ch:
				if (num_rob++ > 0) {
					// clang-format off
					throw std::invalid_argument(
					    "The reference grid contains more than one robot "
					    "character ('" + std::string{c} + "')");
					// clang-format on
				}
				spawn_pos.x = static_cast<int>(col);
				spawn_pos.y = static_cast<int>(row);
				found_char = true;
				break;
			default:
				break;
			}
			if (found_char) {
				// Replace Box or Robot with empty char
				grid.set_char(static_cast<size_t>(col),
				              static_cast<size_t>(row),
				              chars::empty_ch);
			}
		}
	}
}
