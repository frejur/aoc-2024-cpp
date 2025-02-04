#include "wd_box.h"

aoc24_15::Wide_box::Wide_box(
    Warehouse* parent_grid, int pos_x, int pos_y)
    : Rect_box(parent_grid, pos_x, pos_y, 2, 1)
    , adj_boxes_U_(std::vector<Box*>(width, &Box::Dummy))
    , adj_boxes_D_(std::vector<Box*>(width, &Box::Dummy))
    , adj_box_R_(&Box::Dummy)
    , adj_box_L_(&Box::Dummy)
{}

aoc24_15::Wide_box::~Wide_box()
{
	std::vector<Box*> unique_boxes{Wide_box::linked_boxes()};
	for (Box* ub : unique_boxes) {
		notify_destruction(ub);
	}
}

void aoc24_15::Wide_box::unlink_box(
    Box* box)
{
	for (int i = 0; i < 2; ++i) {
		if (adj_boxes_U_[i] != nullptr && adj_boxes_U_[i] == box) {
			adj_boxes_U_[i] = &Box::Dummy;
		}
	}
	for (int j = 0; j < 2; ++j) {
		if (adj_boxes_D_[j] != nullptr && adj_boxes_D_[j] == box) {
			adj_boxes_D_[j] = &Box::Dummy;
		}
	}
	if (adj_box_R_ != nullptr && adj_box_R_ == box) {
		adj_box_R_ = &Box::Dummy;
	}
	if (adj_box_L_ != nullptr && adj_box_L_ == box) {
		adj_box_L_ = &Box::Dummy;
	}
}

std::vector<aoc24_15::Edge_tile> aoc24_15::Wide_box::edge_tiles() const
{
	std::vector<Edge_tile> tiles;
	for (const auto& t : offset_) {
		tiles.emplace_back(position + t.position, t.direction);
	}
	return tiles;
}

std::vector<aoc24_15::Edge_tile> aoc24_15::Wide_box::edge_tiles(
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

aoc24_15::Box** aoc24_15::Wide_box::linked_box_address(
    Edge_tile tile) const
{
	// Calculate offset from center to find edge tile pointer
	Edge_tile offs_edge_tile{tile.position - position, tile.direction};
	auto match = edge_tiles_.find(offs_edge_tile);
	if (match == edge_tiles_.end()) {
		throw std::logic_error("Could not find edge tile");
	}
	return match->second;
}

std::vector<aoc24_15::Box*> aoc24_15::Wide_box::linked_boxes() const
{
	std::vector<Box*> unique_boxes;
	for (auto linked : edge_tiles_) {
		Box* box = *linked.second;
		if (box != nullptr && !box->is_dummy) {
			bool already_covered = false;
			for (const Box* unique : unique_boxes) {
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

std::vector<aoc24_15::Box*> aoc24_15::Wide_box::linked_boxes(
    aoc24::Direction dir) const
{
	std::vector<Box*> unique_boxes;
	for (auto linked : edge_tiles_) {
		if (linked.first.direction != dir) {
			continue;
		}
		Box* box = *linked.second;
		if (box != nullptr && !box->is_dummy) {
			bool already_covered = false;
			for (const Box* unique : unique_boxes) {
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

//------------------------------------------------------------------------------

std::unique_ptr<aoc24_15::Box> aoc24_15::Wide_box_factory::create_box(
    Warehouse& parent_grid, int pos_x, int pos_y)
{
	return std::unique_ptr<Box>(new Wide_box{&parent_grid, pos_x, pos_y});
}
