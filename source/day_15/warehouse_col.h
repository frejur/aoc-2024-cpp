#ifndef WAREHOUSE_COL_H
#define WAREHOUSE_COL_H
#include <vector>

namespace aoc24_15 {

class Box;

// Provides collision detection helpers for the linked Warehouse
// Only exposed to Warehouse and derivatives of Box
class Warehouse_collision
{
private:
	friend class Warehouse;
	friend class Simple_box;
	friend class Rect_box;
	Warehouse_collision(const Warehouse& parent_grid)
	    : parent_(parent_grid)
	{}

	const Warehouse& parent_;
	bool rect_is_oob(int top_L_x, int top_L_y, int w, int h) const;
	bool rect_is_occupied(int top_L_x, int top_L_y, int w, int h) const;
	bool wall_or_stuck_box_at(size_t pos) const;
	bool rect_has_wall_or_stuck_box(int top_L_x,
	                                int top_L_y,
	                                int w,
	                                int h,
	                                bool match_all_tiles = false) const;
	bool rect_has_only_wall_or_stuck_box(int top_L_x,
	                                     int top_L_y,
	                                     int w,
	                                     int h) const;
	bool pt_has_box(int pos_x, int pos_y) const;
	bool rect_has_box(int top_L_x, int top_L_y, int w, int h) const;
	std::vector<const Box*> find_potentially_adj(const Box&) const;
};

} // namespace aoc24_15

#endif // WAREHOUSE_COL_H
