#include "warehouse_col.h"
#include "box.h"
#include <array>

bool aoc24_15::Warehouse_collision::pt_has_box(int pos_x, int pos_y) const
{
	for (size_t i = 0; i < parent_.boxes_.size(); ++i) {
		const auto& box = *parent_.boxes_[i];
		if (box.contains_pt(pos_x, pos_y)) {
			return true;
		}
	}
	return false;
}

bool aoc24_15::Warehouse_collision::rect_is_oob(int top_L_x,
                                                int top_L_y,
                                                int w,
                                                int h) const
{
	using aoc24::Vec2d;
	std::array<aoc24::Vec2d, 4> corners = {Vec2d{top_L_x, top_L_y},
	                                       Vec2d{top_L_x + w, top_L_y},
	                                       Vec2d{top_L_x + w, top_L_y + h},
	                                       Vec2d{top_L_x, top_L_y + h}};
	for (const auto& c : corners) {
		if (!parent_.valid_xy(c.x, c.y)) {
			return true;
		}
	}
	return false;
}

bool aoc24_15::Warehouse_collision::rect_is_occupied(int top_L_x,
                                                     int top_L_y,
                                                     int w,
                                                     int h) const
{
	return rect_has_wall_or_stuck_box(top_L_x, top_L_y, w, h)
	       || rect_has_box(top_L_x, top_L_y, w, h);
}

bool aoc24_15::Warehouse_collision::wall_or_stuck_box_at(size_t pos) const
{
	return (parent_.walls_map().test(pos) || parent_.stuck_map().test(pos));
}

bool aoc24_15::Warehouse_collision::rect_has_wall_or_stuck_box(
    int top_L_x, int top_L_y, int w, int h, bool match_all_tiles) const
{
	const size_t area = static_cast<size_t>(w * h);
	const size_t start_pos = static_cast<size_t>(top_L_y * parent_.sz + top_L_x);
	size_t pos = start_pos;
	for (size_t count = 0; count < area; ++count) {
		if (wall_or_stuck_box_at(pos)) {
			if (!match_all_tiles) {
				return true;
			}
		} else if (match_all_tiles) {
			return false;
		}
		pos += (count > 0 && (count % w == 0)) ? parent_.sz - w : 1;
	}

	return match_all_tiles ? true : false;
}

bool aoc24_15::Warehouse_collision::rect_has_only_wall_or_stuck_box(int top_L_x,
                                                                    int top_L_y,
                                                                    int w,
                                                                    int h) const
{
	return rect_has_wall_or_stuck_box(top_L_x, top_L_y, w, h, true);
}

bool aoc24_15::Warehouse_collision::rect_has_box(int top_L_x,
                                                 int top_L_y,
                                                 int w,
                                                 int h) const
{
	for (size_t i = 0; i < parent_.boxes_.size(); ++i) {
		const auto& box = *parent_.boxes_[i];
		if (box.intersects_with_rect(top_L_x, top_L_y, w, h)) {
			return true;
		}
	}
	return false;
}

std::vector<const aoc24_15::Box*>
aoc24_15::Warehouse_collision::find_potentially_adj(
    const Box& center_box) const
{
	// Get all Box objects within the extended bounding box of the center
	std::vector<const Box*> boxes
	    = parent_.find_boxes_in_rect(center_box.position.x - 1,
	                                 center_box.position.y - 1,
	                                 center_box.width + 2,
	                                 center_box.height + 2);

	if (boxes.empty()) {
		return {};
	}

	// Remove center box from adjacent tiles
	for (auto it = boxes.begin(); it != boxes.end();) {
		if ((*it) == &center_box) {
			it = boxes.erase(it);
		} else {
			++it;
		}
	}
	return boxes;
}
