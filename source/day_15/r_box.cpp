#include "r_box.h"
#include "warehouse_col.h"

aoc24_15::Rect_box::Rect_box(
    Warehouse* parent_grid, int pos_x, int pos_y, int w, int h)
    : Box_new(parent_grid, pos_x, pos_y, w, h)
    , adj_boxes_U_(std::vector<Box_new*>(width, &Box_new::Dummy))
    , adj_boxes_D_(std::vector<Box_new*>(width, &Box_new::Dummy))
    , adj_box_R_(&Box_new::Dummy)
    , adj_box_L_(&Box_new::Dummy)
{}

aoc24_15::Rect_box::~Rect_box()
{
	std::vector<Box_new*> unique_boxes{Rect_box::linked_boxes()};
	for (Box_new* ub : unique_boxes) {
		notify_destruction(ub);
	}
}

void aoc24_15::Rect_box::unlink_edge_tile(Edge_tile edge_tile)
{
	update_edge_tile(edge_tile, &Box_new::Dummy);
}

void aoc24_15::Rect_box::update_edge_tile(Edge_tile edge_tile,
                                          Box_new* linked_box_ptr)
{
	Box_new** current_box_ptr = linked_box_address(edge_tile);
	*current_box_ptr = linked_box_ptr;
}

void aoc24_15::Rect_box::unlink_box(Box_new* box)
{
	for (int i = 0; i < 2; ++i) {
		if (adj_boxes_U_[i] != nullptr && adj_boxes_U_[i] == box) {
			adj_boxes_U_[i] = &Box_new::Dummy;
		}
	}
	for (int j = 0; j < 2; ++j) {
		if (adj_boxes_D_[j] != nullptr && adj_boxes_D_[j] == box) {
			adj_boxes_D_[j] = &Box_new::Dummy;
		}
	}
	if (adj_box_R_ != nullptr && adj_box_R_ == box) {
		adj_box_R_ = &Box_new::Dummy;
	}
	if (adj_box_L_ != nullptr && adj_box_L_ == box) {
		adj_box_L_ = &Box_new::Dummy;
	}
}

std::vector<aoc24_15::Edge_tile_instruction> aoc24_15::Rect_box::update_adj()
{
	std::vector<Edge_tile_instruction> inv_tile_instr;

	const auto& col = grid_->collision;
	std::vector<const Box_new*> adj_boxes{col->find_potentially_adj(*this)};
	std::vector<Edge_tile> center_edge_tiles{edge_tiles()};
	for (const Edge_tile& et : center_edge_tiles) {
		bool found_adj = false;
		for (size_t i = 0; !found_adj && i < adj_boxes.size(); ++i) {
			const Box_new* adj_box = adj_boxes[i];
			if (!adj_box->contains_pt(et.position.x, et.position.y)) {
				continue;
			}

			// The adjacent Box contains the given edge tile => Update link.
			Box_new* mutable_box_adj = const_cast<Box_new*>(adj_box);
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

std::vector<aoc24_15::Edge_tile_instruction> aoc24_15::Rect_box::update_adj(
    aoc24::Direction dir)
{
	return {};
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

std::vector<aoc24_15::Edge_tile> aoc24_15::Rect_box::edge_tiles() const
{
	std::vector<Edge_tile> tiles;
	for (const auto& t : offset_) {
		tiles.emplace_back(position + t.position, t.direction);
	}
	return tiles;
}

std::vector<aoc24_15::Edge_tile> aoc24_15::Rect_box::edge_tiles(
    aoc24::Direction dir) const
{
	std::vector<Edge_tile> tiles;
	for (const auto& t : offset_) {
		if (t.direction == dir) {
			tiles.emplace_back(position + t.position, t.direction);
		}
	}
	return tiles;
}

aoc24_15::Box_new** aoc24_15::Rect_box::linked_box_address(Edge_tile tile) const
{
	// Calculate offset from center to find edge tile pointer
	Edge_tile offs_edge_tile{tile.position - position, tile.direction};
	auto match = edge_tiles_.find(offs_edge_tile);
	if (match == edge_tiles_.end()) {
		throw std::logic_error("Could not find edge tile");
	}
	return match->second;
}

aoc24_15::Box_new* aoc24_15::Rect_box::linked_box(Edge_tile tile) const
{
	Box_new** edge_tile_ptr = linked_box_address(tile);
	return *edge_tile_ptr;
}

std::vector<aoc24_15::Box_new*> aoc24_15::Rect_box::linked_boxes() const
{
	std::vector<Box_new*> unique_boxes;
	for (auto linked : edge_tiles_) {
		Box_new* box = *linked.second;
		if (box != nullptr && !box->is_dummy) {
			bool already_covered = false;
			for (const Box_new* unique : unique_boxes) {
				if (box == unique) {
					already_covered = true;
					break;
				}
			}
			if (!already_covered) {
				unique_boxes.emplace_back(box);
			}
		}
	}
	return unique_boxes;
}

std::vector<aoc24_15::Box_new*> aoc24_15::Rect_box::linked_boxes(
    aoc24::Direction dir) const
{
	std::vector<Box_new*> unique_boxes;
	for (auto linked : edge_tiles_) {
		if (linked.first.direction != dir) {
			continue;
		}
		Box_new* box = *linked.second;
		if (box != nullptr && !box->is_dummy) {
			bool already_covered = false;
			for (const Box_new* unique : unique_boxes) {
				if (box == unique) {
					already_covered = true;
					break;
				}
			}
			if (!already_covered) {
				unique_boxes.emplace_back(box);
			}
		}
	}
	return unique_boxes;
}

std::unique_ptr<aoc24_15::Box_new> aoc24_15::Rect_box_factory::create_box(
    Warehouse& parent_grid, int pos_x, int pos_y)
{
	return std::unique_ptr<Box_new>(
	    new Rect_box{&parent_grid, pos_x, pos_y, 2, 1});
}
