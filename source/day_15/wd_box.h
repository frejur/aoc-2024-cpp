#ifndef WD_BOX_H
#define WD_BOX_H

#include "r_box.h"
#include <array>
#include <map>
#include <vector>

namespace aoc24_15 {

class Wide_box : public Rect_box
{
	friend class Wide_box_factory;
	Wide_box(Warehouse* parent_grid, int pos_x, int pos_y);

	std::vector<Box_new*> adj_boxes_U_;
	std::vector<Box_new*> adj_boxes_D_;
	Box_new* adj_box_R_;
	Box_new* adj_box_L_;

	using Direction = aoc24::Direction;
	using Offset_array = std::array<Edge_tile, 6>;
	static constexpr Offset_array offset_ = {Edge_tile{0, -1, Direction::Up},
	                                         Edge_tile{1, -1, Direction::Up},
	                                         Edge_tile{2, 0, Direction::Right},
	                                         Edge_tile{0, 1, Direction::Down},
	                                         Edge_tile{1, 1, Direction::Down},
	                                         Edge_tile{-1, 0, Direction::Left}};

	using Edge_tile_map = std::map<Edge_tile, Box_new**, Edge_tile_comparator>;
	const Edge_tile_map edge_tiles_ = {{offset_[0], &adj_boxes_U_[0]},
	                                   {offset_[1], &adj_boxes_U_[1]},
	                                   {offset_[2], &adj_box_R_},
	                                   {offset_[3], &adj_boxes_D_[0]},
	                                   {offset_[4], &adj_boxes_D_[1]},
	                                   {offset_[5], &adj_box_L_}};

	virtual void unlink_box(Box_new* box) override;
	virtual std::vector<Edge_tile> edge_tiles() const override;
	virtual std::vector<Edge_tile> edge_tiles(
	    aoc24::Direction dir) const override;
	virtual Box_new** linked_box_address(Edge_tile tile) const override;
	virtual std::vector<Box_new*> linked_boxes() const override;
	virtual std::vector<Box_new*> linked_boxes(
	    aoc24::Direction dir) const override;

public:
	virtual long long bitmap() const override { return 0b11; }
	~Wide_box();
};

class Wide_box_factory : public Box_factory
{
public:
	using Box_factory::Box_factory;
	virtual std::unique_ptr<Box_new> create_box(Warehouse& parent_grid,
	                                            int pos_x,
	                                            int pos_y) override;
};

} // namespace aoc24_15

#endif // WD_BOX_H
