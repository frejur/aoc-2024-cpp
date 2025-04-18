#include "s_box.h"
#include "../lib/direction.h"
#include "warehouse_col.h"
#include <array>

aoc24_15::Simple_box::Simple_box(
    Warehouse* parent_grid, int pos_x, int pos_y)
    : Rect_box(parent_grid, pos_x, pos_y, 1, 1)
    , adj_box_U_(&Box::Dummy)
    , adj_box_R_(&Box::Dummy)
    , adj_box_D_(&Box::Dummy)
    , adj_box_L_(&Box::Dummy)
{}

aoc24_15::Simple_box::~Simple_box()
{
	std::vector<Box*> unique_boxes{Simple_box::linked_boxes()};
	for (Box* ub : unique_boxes) {
		notify_destruction(ub);
	}
}

void aoc24_15::Simple_box::unlink_box(
    Box* box)
{
	if (adj_box_U_ != nullptr && adj_box_U_ == box) {
		adj_box_U_ = &Box::Dummy;
	}
	if (adj_box_R_ != nullptr && adj_box_R_ == box) {
		adj_box_R_ = &Box::Dummy;
	}
	if (adj_box_D_ != nullptr && adj_box_D_ == box) {
		adj_box_D_ = &Box::Dummy;
	}
	if (adj_box_L_ != nullptr && adj_box_L_ == box) {
		adj_box_L_ = &Box::Dummy;
	}
}

bool aoc24_15::Simple_box::is_stuck() const
{
	const size_t w = grid_->width();
	const size_t h = grid_->height();
	const size_t max_pos = w * h - 1;
	const auto& col = *grid_->collision;
	const std::array<int, 4> dir_offset{-static_cast<int>(w),
	                                    1,
	                                    static_cast<int>(w),
	                                    -1};
	std::array<bool, 4> stuck_URDL;

	// Box is stuck if one of its corners is is stuck (2 conseq. tiles)
	const int mid_pos = static_cast<int>(position.y * h + position.x);
	for (int i = 0; i < 4; ++i) {
		int pos = mid_pos + dir_offset[i];
		stuck_URDL[i] = (pos < 0) || (static_cast<size_t>(pos) > max_pos)
		                || col.wall_or_stuck_box_at(static_cast<size_t>(pos));
		if (i > 0 && (stuck_URDL[i] && stuck_URDL[(i - 1) % 4])) {
			return true;
		}
	}
	return stuck_URDL[3] && stuck_URDL[0];
}

bool aoc24_15::Simple_box::contains_pt(int pos_x, int pos_y) const
{
	return (pos_x == position.x && pos_y == position.y);
}

bool aoc24_15::Simple_box::intersects_with_rect(int top_L_x,
                                                int top_L_y,
                                                int w,
                                                int h) const
{
	return ((top_L_x <= position.x && position.x <= (top_L_x + w))
	        && (top_L_y <= position.y && position.y <= (top_L_y + h)));
}

aoc24_15::Box** aoc24_15::Simple_box::linked_box_address(
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

std::vector<aoc24_15::Box*> aoc24_15::Simple_box::linked_boxes() const
{
	std::array<Box*, 4> linked_boxes{adj_box_U_,
	                                 adj_box_R_,
	                                 adj_box_D_,
	                                 adj_box_L_};
	std::vector<Box*> unique_boxes;
	for (Box* linked : linked_boxes) {
		if (linked != nullptr && !linked->is_dummy) {
			bool already_covered = false;
			for (const Box* unique : unique_boxes) {
				if (linked == unique) {
					already_covered = true;
					break;
				}
			}
			if (!already_covered) {
				unique_boxes.emplace_back(linked);
			}
		}
	}
	return unique_boxes;
}

std::vector<aoc24_15::Box*> aoc24_15::Simple_box::linked_boxes(
    aoc24::Direction dir) const
{
	Box* box = nullptr;
	switch (dir) {
	case Direction::Up:
		box = adj_box_U_;
		break;
	case aoc24::Direction::Right:
		box = adj_box_R_;
		break;
	case aoc24::Direction::Down:
		box = adj_box_D_;
		break;
	case aoc24::Direction::Left:
		box = adj_box_L_;
		break;
	default:
		break;
	}
	if (box != nullptr && !box->is_dummy) {
		return {box};
	}
	return {};
}

std::vector<aoc24_15::Edge_tile> aoc24_15::Simple_box::edge_tiles() const
{
	return {{position + offset_[0].position, offset_[0].direction},
	        {position + offset_[1].position, offset_[1].direction},
	        {position + offset_[2].position, offset_[2].direction},
	        {position + offset_[3].position, offset_[3].direction}};
}

std::vector<aoc24_15::Edge_tile> aoc24_15::Simple_box::edge_tiles(
    aoc24::Direction dir) const
{
	int idx = static_cast<int>(dir);
	if (-1 >= idx || idx >= 4) {
		throw std::invalid_argument(
		    "Invalid direction for fetching edge_tiles");
	}
	return {{position + offset_[idx].position, offset_[idx].direction}};
}

std::unique_ptr<aoc24_15::Box> aoc24_15::Simple_box_factory::create_box(
    Warehouse& parent_grid, int pos_x, int pos_y)
{
	return std::unique_ptr<Box>(new Simple_box{&parent_grid, pos_x, pos_y});
}
