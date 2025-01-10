#ifndef BOX_GRID_H
#define BOX_GRID_H
#include "../lib/bit_grid.h"
#include <deque>

namespace aoc24_15 {
struct Vec2d
{
	Vec2d(int xx, int yy)
	    : x(xx)
	    , y(yy)
	{}
	int x;
	int y;
	bool operator<(const Vec2d& rhs) const;
};
enum class Direction { Up = 0, Right, Down, Left };
std::ostream& operator<<(std::ostream& ostr, const Direction& dir);
std::istream& operator>>(std::istream& istr, Direction& dir);

Vec2d dir_to_offset(Direction dir, bool invert = false);
//------------------------------------------------------------------------------

class Robot
{
public:
	Robot()
	    : has_spawned_(false)
	    , init_pos_(0, 0)
	    , pos_(0, 0)
	{}
	void spawn(int pos_x, int pos_y);
	void move(Direction dir);
	Vec2d position() const { return pos_; }
	void reset() { pos_ = init_pos_; }

private:
	bool has_spawned_;
	Vec2d init_pos_;
	Vec2d pos_;
};

//------------------------------------------------------------------------------

class Box;

struct Adjacent_tile
{
	Adjacent_tile()
	    : type(Type::Empty)
	    , box(nullptr)
	{}
	enum class Type { Empty, Wall, Box_stuck, Box_movable };
	Type type;
	Box* box;
};

class Box
{
	friend class Box_grid;
	enum Create { Dummy };

public:
	Box(int pos_x, int pos_y)
	    : is_dummy_(false)
	    , marked_for_pruning_(false)
	    , init_pos_(pos_x, pos_y)
	    , pos_(pos_x, pos_y)
	{}
	Box(Create)
	    : is_dummy_(true)
	    , marked_for_pruning_(false)
	    , init_pos_(0, 0)
	    , pos_(0, 0)
	{}
	Vec2d position() const { return pos_; }
	bool is_dummy() const { return is_dummy_; }

	static Box dummy()
	{
		static const Box b{Create::Dummy};
		return b;
	}

private:
	bool is_dummy_;
	bool marked_for_pruning_;
	Vec2d init_pos_;
	Vec2d pos_;

	Adjacent_tile adj_U;
	Adjacent_tile adj_R;
	Adjacent_tile adj_D;
	Adjacent_tile adj_L;
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
	Box_grid(const aoc24::Char_grid& reference_grid,
	         std::unique_ptr<aoc24::Dyn_bitset> dyn_bitset);
	static constexpr size_t nopos = -1;
	void move_robot(Direction dir);
	void print_map(std::ostream& ostr = std::cout);
	int number_of_moves() const { return num_moves_; }
	bool boxes_match_maps();
	long long sum_of_all_box_coordinates() const;

private:
	bool map_ptr_init_;
	int num_moves_;
	aoc24::Dyn_bitset* walls_;
	aoc24::Dyn_bitset* movable_;
	aoc24::Dyn_bitset* stuck_;
	Robot rob_;
	std::list<Box> box_;

	// Maps
	void init_maps();
	aoc24::Dyn_bitset& walls_map() const;
	aoc24::Dyn_bitset& movable_map() const;
	aoc24::Dyn_bitset& stuck_map() const;

	// Boxes
	Box box_dummy_;
	void add_box(int pos_x, int pos_y);
	Box& find_box(int pos_x, int pos_y, bool skip_check_position = false);
	Adjacent_tile& adj_from_dir(Box& box, Direction dir, bool invert = false);
	void update_adj(Box& box, Direction dir);
	void link_adj_boxes(Box& box_a, Box& box_b, Direction from_a_to_b);
	void prune_boxes();
	void mark_as_stuck(Box& b);
	bool adj_is_on_stuck_map(const Box& b, Direction dir) const;
	Is_stuck is_stuck(const Box& b) const;

	// Robot
	std::vector<Box*> movable_boxes_in_dir(int pos_x, int pos_y, Direction dir);

	// Misc. helpers
	Direction dir_turn_left(const Direction dir) const;
	Direction dir_turn_right(const Direction dir) const;
	void throw_if_no_map() const;
	void throw_if_occupied(size_t pos) const;
	aoc24::XY to_xy(int x, int y, const bool skip_check = true);
};
} // namespace aoc24_15

#endif // BOX_GRID_H
