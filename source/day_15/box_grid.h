#ifndef BOX_GRID_H
#define BOX_GRID_H
#include "../lib/bit_grid.h"
#include "../lib/direction.h"
#include "../lib/vec2d.h"
#include "robot.h"
#include <set>

namespace aoc24_15 {

struct Adjacent_tile
{
protected:
	Adjacent_tile(){};
};

class Box
{
	friend class Box_grid;

public:
	Box(int pos_x, int pos_y)
	    : is_dummy_(false)
	    , marked_for_pruning_(false)
	    , init_pos_(pos_x, pos_y)
	    , pos_(pos_x, pos_y)
	{}

	enum Create { Dummy };
	Box(Create)
	    : is_dummy_(true)
	    , marked_for_pruning_(false)
	    , init_pos_(0, 0)
	    , pos_(0, 0)
	{}
	aoc24::Vec2d position() const { return pos_; }
	bool is_dummy() const { return is_dummy_; }

	static Box dummy()
	{
		static const Box b{Create::Dummy};
		return b;
	}

protected:
	bool is_dummy_;
	bool marked_for_pruning_;
	aoc24::Vec2d init_pos_;
	aoc24::Vec2d pos_;
};

class Is_stuck
{
public:
	Is_stuck()
	    : up(false)
	    , right(false)
	    , down(false)
	    , left(false)
	{}

	bool up;
	bool right;
	bool down;
	bool left;

	bool operator()(int dir_n) const
	{
		dir_n *= (dir_n < 0) ? -1 : 1;
		dir_n %= 4;
		bool is_stuck = false;
		switch (dir_n) {
		case 0:
			is_stuck = up;
			break;
		case 1:
			is_stuck = right;
			break;
		case 2:
			is_stuck = down;
			break;
		case 3:
			is_stuck = left;
			break;
		}
		return is_stuck;
	}

	bool has_corner_stuck() const
	{
		for (int i = 0; i < 4; ++i) {
			if ((*this)(i) && (*this)(i + 1)) {
				return true;
			}
		}
		return false;
	}
};

//------------------------------------------------------------------------------
class Box_grid : public aoc24::Bit_grid
{
public:
	static constexpr size_t nopos = -1;
	int number_of_moves() const { return num_moves_; }

	virtual void move_robot(aoc24::Direction dir) = 0;
	virtual void print_map(std::ostream& ostr = std::cout) = 0;
	virtual bool boxes_match_maps() = 0;
	virtual long long sum_of_all_box_coordinates() const = 0;

protected:
	Box_grid(std::unique_ptr<aoc24::Dyn_bitset> dyn_bitset);
	Box_grid(size_t width,
	         size_t height,
	         std::unique_ptr<aoc24::Dyn_bitset> dyn_bitset);

	bool map_ptr_init_;
	int num_moves_;
	Robot rob_;
	Box box_dummy_;

	virtual void init_maps() = 0;

	// Boxes
	virtual void add_box(int pos_x, int pos_y) = 0;
	virtual Box& find_box(int pos_x, int pos_y, bool skip_check_position = false)
	    = 0;

	// Adjacent tiles
	virtual Adjacent_tile& adj_from_dir(Box& box,
	                                    aoc24::Direction dir,
	                                    bool invert = false)
	    = 0;
	virtual void update_adj(Box& box, aoc24::Direction dir) = 0;
	virtual void link_adj_boxes(Box& box_a,
	                            Box& box_b,
	                            aoc24::Direction from_a_to_b)
	    = 0;

	// Stuck boxes
	virtual void prune_boxes() = 0;
	virtual void mark_as_stuck(Box& b) = 0;
	virtual bool adj_is_on_stuck_map(const Box& b, aoc24::Direction dir) const
	    = 0;
	virtual Is_stuck is_stuck(const Box& b) const = 0;

	// Robot
	virtual std::vector<Box*> movable_boxes_in_dir(int pos_x,
	                                               int pos_y,
	                                               aoc24::Direction dir)
	    = 0;

	// Misc. helpers
	aoc24::XY to_xy(int x, int y, const bool skip_check = true);
	bool has_cached_l(
	    const aoc24::Vec2d& pos,
	    const std::set<aoc24::Vec2d, aoc24::Vec2d_comparator> cache);
	bool has_cached_u(
	    const aoc24::Vec2d& pos,
	    const std::set<aoc24::Vec2d, aoc24::Vec2d_comparator> cache);
	void throw_if_no_map() const;

	virtual void throw_if_occupied(size_t pos) const = 0;
};
} // namespace aoc24_15

#endif // BOX_GRID_H
