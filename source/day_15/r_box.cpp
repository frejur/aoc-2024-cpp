#include "r_box.h"
#include "warehouse_col.h"

void aoc24_15::Rect_box::unlink_edge_tile(Edge_tile edge_tile)
{
	update_edge_tile(edge_tile, &Box::Dummy);
}

void aoc24_15::Rect_box::update_edge_tile(
    Edge_tile edge_tile, Box* linked_box_ptr)
{
	Box** current_box_ptr = linked_box_address(edge_tile);
	*current_box_ptr = linked_box_ptr;
}

aoc24_15::Rect_box::Rect_box(
    Warehouse* parent_grid, int pos_x, int pos_y, int w, int h)
    : Box(parent_grid, pos_x, pos_y, w, h)
{}

std::vector<aoc24_15::Edge_tile_instruction> aoc24_15::Rect_box::update_adj()
{
	using aoc24::Direction;
	std::vector<Edge_tile_instruction> inv_tile_instr;

	const auto& col = grid_->collision;
	std::vector<const Box*> adj_boxes{col->find_potentially_adj(*this)};
	std::vector<Edge_tile> center_edge_tiles{edge_tiles()};
	for (const Edge_tile& et : center_edge_tiles) {
		bool found_adj = false;
		for (size_t i = 0; !found_adj && i < adj_boxes.size(); ++i) {
			const Box* adj_box = adj_boxes[i];
			if (!adj_box->contains_pt(et.position.x, et.position.y)) {
				continue;
			}

			// The adjacent Box contains the given edge tile => Update link.
			Box* mutable_box_adj = const_cast<Box*>(adj_box);
			update_edge_tile(et, mutable_box_adj);

			// There SHOULD be an edge tile in the adjacent box, much like
			// the current center edge tile, but INVERTED.
			Direction dir_inv = aoc24::dir_inverted(et.direction);
			Edge_tile et_inv = {et.position + aoc24::dir_to_offset(dir_inv),
			                    dir_inv};

			inv_tile_instr.emplace_back(et_inv, mutable_box_adj);
			found_adj = true;
		}
		if (!found_adj) {
			unlink_edge_tile(et); // Dummy
		}
	}

	return inv_tile_instr;
}

bool aoc24_15::Rect_box::is_stuck() const
{
	const auto& col = *grid_->collision;
	bool stuck_U = col.rect_is_oob(position.x, position.y - 1, width, 1)
	               || col.rect_has_only_wall_or_stuck_box(position.x,
	                                                      position.y - 1,
	                                                      width,
	                                                      1);
	bool stuck_R = col.rect_is_oob(position.x + width, position.y, 1, height)
	               || col.rect_has_only_wall_or_stuck_box(position.x + width,
	                                                      position.y,
	                                                      1,
	                                                      height);
	if (stuck_U && stuck_R) {
		return true;
	}
	bool stuck_D = col.rect_is_oob(position.x, position.y + height, width, 1)
	               || col.rect_has_only_wall_or_stuck_box(position.x,
	                                                      position.y + height,
	                                                      width,
	                                                      1);
	if (stuck_R && stuck_D) {
		return true;
	}
	bool stuck_L = col.rect_is_oob(position.x - 1, position.y, 1, height)
	               || col.rect_has_only_wall_or_stuck_box(position.x - 1,
	                                                      position.y,
	                                                      1,
	                                                      height);
	return (stuck_L && stuck_D) || (stuck_L && stuck_U);
}

bool aoc24_15::Rect_box::contains_pt(int pos_x, int pos_y) const
{
	return ((position.x <= pos_x && pos_x < (position.x + width))
	        && (position.y <= pos_y && pos_y < (position.y + height)));
}

bool aoc24_15::Rect_box::intersects_with_rect(int top_L_x,
                                              int top_L_y,
                                              int w,
                                              int h) const
{
	// Compute intersection boundaries
	const int xsect_U = (std::max)(position.y, top_L_y);
	const int xsect_R = (std::min)(position.x + width, top_L_x + w);
	const int xsect_D = (std::min)(position.y + height, top_L_y + h);
	const int xsect_L = (std::max)(position.x, top_L_x);

	return ((xsect_L < xsect_R) && (xsect_U < xsect_D));
}

aoc24_15::Box* aoc24_15::Rect_box::linked_box(
    Edge_tile tile) const
{
	Box** edge_tile_ptr = linked_box_address(tile);
	return *edge_tile_ptr;
}
