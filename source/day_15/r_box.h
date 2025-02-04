#ifndef R_BOX_H
#define R_BOX_H
#include "box.h"
#include <vector>

namespace aoc24_15 {

class Rect_box : public Box_new
{
protected:
	virtual void unlink_edge_tile(Edge_tile edge_tile) override;
	virtual void update_edge_tile(Edge_tile edge_tile,
	                              Box_new* linked_box_ptr) override;
	virtual std::vector<Edge_tile_instruction> update_adj() override;
	virtual std::vector<Edge_tile> edge_tiles(aoc24::Direction dir) const override
	    = 0;
	virtual std::vector<Edge_tile> edge_tiles() const override = 0;
	virtual Box_new* linked_box(Edge_tile tile) const override;

	Rect_box(Warehouse* parent_grid, int pos_x, int pos_y, int w, int h);

public:
	virtual bool is_stuck() const override;
	virtual bool contains_pt(int pos_x, int pos_y) const override;
	virtual bool intersects_with_rect(int top_L_x,
	                                  int top_L_y,
	                                  int w,
	                                  int h) const override;
	~Rect_box() = default;
};

} // namespace aoc24_15

#endif // R_BOX_H
