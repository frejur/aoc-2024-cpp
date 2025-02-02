#ifndef R_BOX_H
#define R_BOX_H
#include "box.h"
#include <array>
#include <map>
#include <vector>

namespace aoc24_15 {

class Rect_box : public Box_new
{
	friend class Rect_box_factory;
	Rect_box(Warehouse* parent_grid, int pos_x, int pos_y, int w, int h);
	~Rect_box();

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

	virtual void unlink_edge_tile(Edge_tile edge_tile) override;
	virtual void update_edge_tile(Edge_tile edge_tile,
	                              Box_new* linked_box_ptr) override;
	virtual void unlink_box(Box_new* box) override;
	virtual std::vector<Edge_tile_instruction> update_adj() override;
	virtual std::vector<Edge_tile_instruction> update_adj(
	    aoc24::Direction dir) override;
	virtual std::vector<Edge_tile> edge_tiles() const override;
	virtual std::vector<Edge_tile> edge_tiles(
	    aoc24::Direction dir) const override;
	virtual Box_new** linked_box_address(Edge_tile tile) const override;
	;
	virtual Box_new* linked_box(Edge_tile tile) const override;
	;
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
	virtual long long bitmap() const override { return 0b11; }
	virtual void test() const override { std::cout << "Rectangle" << '\n'; }
};

class Rect_box_factory : public Box_factory
{
public:
	using Box_factory::Box_factory;
	virtual std::unique_ptr<Box_new> create_box(Warehouse& parent_grid,
	                                            int pos_x,
	                                            int pos_y) override;
};

} // namespace aoc24_15

#endif // R_BOX_H
