#ifndef S_BOX_H
#define S_BOX_H
#include "box.h"
#include <array>
#include <map>

namespace aoc24_15 {

class Simple_box : public Box_new
{
	friend class Simple_box_factory;
	Simple_box(Warehouse* parent_grid, int pos_x, int pos_y);
	~Simple_box();
	Box_new* adj_box_U_;
	Box_new* adj_box_R_;
	Box_new* adj_box_D_;
	Box_new* adj_box_L_;

	using Direction = aoc24::Direction;
	using Offset_array = std::array<Edge_tile, 4>;
	static constexpr Offset_array offset_ = {Edge_tile{0, -1, Direction::Up},
	                                         Edge_tile{1, 0, Direction::Right},
	                                         Edge_tile{0, 1, Direction::Down},
	                                         Edge_tile{-1, 0, Direction::Left}};

	using Edge_tile_map = std::map<Edge_tile, Box_new**, Edge_tile_comparator>;
	const Edge_tile_map edge_tiles_ = {{offset_[0], &adj_box_U_},
	                                   {offset_[1], &adj_box_R_},
	                                   {offset_[2], &adj_box_D_},
	                                   {offset_[3], &adj_box_L_}};

	virtual void unlink_edge_tile(Edge_tile edge_tile) override;
	virtual void update_edge_tile(Edge_tile edge_tile,
	                              Box_new* linked_box_ptr) override;
	virtual void unlink_box(Box_new* box) override;
	virtual std::vector<Edge_tile_instruction> update_adj() override;

	virtual Box_new** linked_box_address(Edge_tile tile) const override;
	virtual Box_new* linked_box(Edge_tile tile) const override;
	virtual std::vector<Box_new*> linked_boxes() const override;
	virtual std::vector<Box_new*> linked_boxes(
	    aoc24::Direction dir) const override;

public:
	virtual bool is_stuck() const override;
	virtual bool contains_pt(int pos_x, int pos_y) const override;
	virtual bool intersects_with_rect(int top_L_x,
	                                  int top_L_y,
	                                  int w,
	                                  int h) const override;
	virtual long long bitmap() const override { return 1; }
	virtual std::vector<Edge_tile> edge_tiles() const override;
	virtual std::vector<Edge_tile> edge_tiles(Direction dir) const override;
};

class Simple_box_factory : public Box_factory
{
public:
	using Box_factory::Box_factory;
	virtual std::unique_ptr<Box_new> create_box(Warehouse& parent_grid,
	                                            int pos_x,
	                                            int pos_y) override;
};

} // namespace aoc24_15

#endif // S_BOX_H
