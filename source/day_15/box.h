#ifndef BOX_H
#define BOX_H
#include "../lib/direction.h"
#include "../lib/vec2d.h"
#include "edge_tile.h"
#include "warehouse.h"

namespace aoc24_15 {

class Box_new
{
public:
	const bool is_dummy;
	const int width;
	const int height;
	const aoc24::Vec2d initial_position;

	aoc24::Vec2d position;

	virtual bool is_stuck() const = 0;
	virtual bool contains_pt(int pos_x, int pos_y) const = 0;
	virtual bool intersects_with_rect(int top_L_x,
	                                  int top_L_y,
	                                  int w,
	                                  int h) const
	    = 0;

	// Returns a bitmap representing the tiles that the Box occupies.
	// The bitmap can be used by the Warehouse to mark / erase those tiles.
	virtual long long bitmap() const = 0;

	// Returns all tiles along the edge of the box
	virtual std::vector<Edge_tile> edge_tiles(aoc24::Direction dir) const = 0;
	virtual std::vector<Edge_tile> edge_tiles() const = 0;

	// Gets linked Box at edge tile
	virtual Box_new** linked_box_address(Edge_tile tile) const = 0;
	virtual Box_new* linked_box(Edge_tile tile) const = 0;
	virtual std::vector<Box_new*> linked_boxes() const = 0;
	virtual std::vector<Box_new*> linked_boxes(aoc24::Direction dir) const = 0;

	// Updated the links to all adjacent boxes,
	// Returns instructions for updating the inverted edge tiles of
	// the affected adjacent boxes
	virtual std::vector<Edge_tile_instruction> update_adj() = 0;

	virtual ~Box_new() = default;

	bool marked_for_pruning() const { return prune_; }

protected:
	bool prune_;
	Warehouse* grid_;
	Box_new(Warehouse* parent_grid, int pos_x, int pos_y, int w = 1, int h = 1);

	// Cleanup pointers
	void notify_destruction(Box_new* box) { box->unlink_box(this); }

	virtual void unlink_box(Box_new* box) = 0;
	virtual void unlink_edge_tile(Edge_tile edge_tile) = 0;
	virtual void update_edge_tile(Edge_tile edge_tile, Box_new* linked_box_ptr)
	    = 0;

private:
	enum class Dummy_box_type { Dummy = 1 };
	Box_new(Dummy_box_type)
	    : is_dummy(true)
	    , width(0)
	    , height(0)
	    , initial_position(0, 0)
	    , position(0, 0)
	    , prune_(false)
	    , grid_(nullptr)
	{}

public:
	// Needs access to the internals of other Box objects to clean up pointers
	friend void Box_new::notify_destruction(Box_new*);

	// The parent warehouse has access to the internals of its Box objects
	friend class Warehouse;

	// Dummy_box needs access to private enum `Dummy_type`
	friend class Dummy_box;
	static Dummy_box Dummy;
};

class Dummy_box : public Box_new
{
public:
	Dummy_box()
	    : Box_new(Dummy_box_type::Dummy)
	{}
	using Box_new::is_dummy;

private:
	// Hide members
	using Box_new::height;
	using Box_new::initial_position;
	using Box_new::marked_for_pruning;
	using Box_new::position;
	using Box_new::width;

	// Hide methods
	// clang-format off
	virtual bool is_stuck() const override { return false; }
	virtual bool contains_pt(int pos_x, int pos_y) const override {	return false; }
	virtual bool intersects_with_rect(int top_L_x, int top_L_y, int w, int h) const override { return false; }
	virtual long long bitmap() const override { return 0; }
	virtual std::vector<Edge_tile_instruction> update_adj() override { return {}; }
	virtual std::vector<Edge_tile> edge_tiles() const override { return {}; }
	virtual std::vector<Edge_tile> edge_tiles(aoc24::Direction dir) const override{ return {}; }
	virtual void unlink_edge_tile(Edge_tile edge_tile) override {};
	virtual void update_edge_tile(Edge_tile edge_tile, Box_new* linked_box_ptr) override {};
	virtual void unlink_box(Box_new* box) override {};
	virtual Box_new** linked_box_address(Edge_tile tile) const override { return nullptr; }
	virtual Box_new* linked_box(Edge_tile tile) const override { return nullptr; }
	virtual std::vector<Box_new*> linked_boxes() const override { return {}; }
	virtual std::vector<Box_new*> linked_boxes(aoc24::Direction dir) const override { return {}; }
	// clang-format on
};

class Box_factory
{
	friend Box_new;

protected:
	Box_factory(){};

public:
	virtual std::unique_ptr<Box_new> create_box(Warehouse& parent_grid,
	                                            int pos_x,
	                                            int pos_y)
	    = 0;
	virtual ~Box_factory() = default;
};

} // namespace aoc24_15

#endif // BOX_H
