#ifndef BOX_GRID_SINGLE_H
#define BOX_GRID_SINGLE_H
#include "box_grid.h"

namespace aoc24_15 {

class Box_single;

struct Adjacent_tile_single : public Adjacent_tile
{
	Adjacent_tile_single()
	    : type(Type::Empty)
	    , box(nullptr)
	{}
	enum class Type { Empty, Wall, Box_stuck, Box_movable };
	Type type;
	Box* box;
};

class Box_single : public Box
{
	friend class Box_grid_single;

public:
	Box_single(int pos_x, int pos_y)
	    : Box(pos_x, pos_y)
	{}
	Box_single(Box::Create dummy)
	    : Box(dummy)
	{}

private:
	Adjacent_tile_single adj_U;
	Adjacent_tile_single adj_R;
	Adjacent_tile_single adj_D;
	Adjacent_tile_single adj_L;
};

class Box_grid_single : public Box_grid
{
public:
	Box_grid_single(const aoc24::Char_grid& reference_grid,
	                std::unique_ptr<aoc24::Dyn_bitset> dyn_bitset);
	virtual void move_robot(Direction dir) override;
	virtual void print_map(std::ostream& ostr = std::cout) override;
	virtual bool boxes_match_maps() override;
	virtual long long sum_of_all_box_coordinates() const override;

private:
	std::list<Box_single> box_;

	aoc24::Dyn_bitset* walls_;
	aoc24::Dyn_bitset* movable_;
	aoc24::Dyn_bitset* stuck_;

	aoc24::Dyn_bitset& walls_map() const;
	aoc24::Dyn_bitset& movable_map() const;
	aoc24::Dyn_bitset& stuck_map() const;

	virtual void init_maps() override;

	// Boxes
	virtual void add_box(int pos_x, int pos_y) override;
	virtual Box& find_box(int pos_x,
	                      int pos_y,
	                      bool skip_check_position = false) override;

	// Adjacent tiles
	virtual Adjacent_tile& adj_from_dir(Box& box,
	                                    Direction dir,
	                                    bool invert = false) override;
	virtual void update_adj(Box& box, Direction dir) override;
	virtual void link_adj_boxes(Box& box_a,
	                            Box& box_b,
	                            Direction from_a_to_b) override;

	// Stuck boxes
	virtual void prune_boxes() override;
	virtual void mark_as_stuck(Box& b) override;
	virtual bool adj_is_on_stuck_map(const Box& b, Direction dir) const override;
	virtual Is_stuck is_stuck(const Box& b) const override;

	// Robot
	virtual std::vector<Box*> movable_boxes_in_dir(int pos_x,
	                                               int pos_y,
	                                               Direction dir) override;

	virtual void throw_if_occupied(size_t pos) const override;
};

} // namespace aoc24_15

#endif // BOX_GRID_SINGLE_H
