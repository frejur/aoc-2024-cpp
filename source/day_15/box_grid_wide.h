#ifndef BOX_GRID_WIDE_H
#define BOX_GRID_WIDE_H
#include "box_grid.h"

namespace aoc24_15 {

// Fwd. decl.
class Box_wide;

struct Adjacent_subtile
{
	enum class Type {
		Unknown,
		Empty,
		Wall,
		Box_stuck_left,
		Box_stuck_right,
		Box_movable_left,
		Box_movable_right,
		Pending_left,
		Pending_right
	};
	Adjacent_subtile()
	    : type(Adjacent_subtile::Type::Empty)
	    , box(nullptr)
	{}
	Adjacent_subtile::Type type;
	Box* box;
};

struct Adjacent_tile_wide : public Adjacent_tile
{
	Adjacent_tile_wide()
	    : left_tile(new Adjacent_subtile())
	    , right_tile(new Adjacent_subtile())
	{}
	// If the adjacent tile is (In relation to the center tile that 'owns' it):
	// - Below or above => Adj. tile has BOTH left and right subtiles
	// - To the left    => Adj. tile has a SINGLE left subtile
	// - To the right   => Adj. tile has a SINGLE right subtile
	std::unique_ptr<Adjacent_subtile> left_tile;
	std::unique_ptr<Adjacent_subtile> right_tile;
};

struct Tile_validation
{
	Tile_validation(bool is_v, bool found_L, bool found_R)
	    : is_valid(is_v)
	    , found_left(found_L)
	    , found_right(found_R)
	{}
	bool is_valid;
	bool found_left;
	bool found_right;
};

class Box_wide : public Box
{
	friend class Box_grid_wide;

public:
	Box_wide(int pos_x, int pos_y)
	    : Box(pos_x, pos_y)
	{
		using ATT = Adjacent_subtile::Type;
		adj_R.left_tile->type = ATT::Unknown;
		adj_L.right_tile->type = ATT::Unknown;
	}
	Box_wide(Box::Create dummy)
	    : Box(dummy)
	{}

private:
	Adjacent_tile_wide adj_U;
	Adjacent_tile_wide adj_R;
	Adjacent_tile_wide adj_D;
	Adjacent_tile_wide adj_L;
};

class Box_grid_wide : public Box_grid
{
public:
	Box_grid_wide(const aoc24::Char_grid& reference_grid,
	              std::unique_ptr<aoc24::Dyn_bitset> dyn_bitset);
	virtual void move_robot(aoc24::Direction dir) override;
	virtual void print_map(std::ostream& ostr = std::cout) override;
	virtual bool boxes_match_maps() override;
	virtual long long sum_of_all_box_coordinates() const override;

private:
	std::list<Box_wide> box_;

	aoc24::Dyn_bitset* walls_;
	aoc24::Dyn_bitset* movable_L_;
	aoc24::Dyn_bitset* movable_R_;
	aoc24::Dyn_bitset* stuck_L_;
	aoc24::Dyn_bitset* stuck_R_;

	aoc24::Dyn_bitset& walls_map() const;
	aoc24::Dyn_bitset& movable_map(aoc24::Direction dir) const;
	aoc24::Dyn_bitset& stuck_map(aoc24::Direction dir) const;

	virtual void init_maps() override;
	bool is_on_box_map(size_t pos) const;

	// Boxes
	virtual void add_box(int pos_x, int pos_y) override;
	virtual Box& find_box(int pos_x,
	                      int pos_y,
	                      bool skip_check_position = false) override;

	// Adjacent tiles
	virtual Adjacent_tile& adj_from_dir(Box& box,
	                                    aoc24::Direction dir,
	                                    bool invert = false) override;
	virtual void update_adj(Box& box, aoc24::Direction dir) override;
	virtual void link_adj_boxes(Box& box_a,
	                            Box& box_b,
	                            aoc24::Direction from_a_to_b) override;
	// Stuck boxes
	virtual void prune_boxes() override;
	virtual void mark_as_stuck(Box& b) override;
	virtual bool adj_is_on_stuck_map(const Box& b,
	                                 aoc24::Direction dir) const override;
	virtual Is_stuck is_stuck(const Box& b) const override;

	// Robot
	virtual std::vector<Box*> movable_boxes_in_dir(
	    int pos_x, int pos_y, aoc24::Direction dir) override;

	virtual void throw_if_occupied(size_t pos) const override;

	// Validation helpers
	bool dir_is_UD(const aoc24::Direction dir) const;
	bool dir_is_L(const aoc24::Direction dir) const;
	bool dir_is_R(const aoc24::Direction dir) const;
	bool dir_is_UDL(const aoc24::Direction dir) const;
	bool dir_is_UDR(const aoc24::Direction dir) const;
	bool adj_type_is_box(Adjacent_subtile::Type t) const;
	const aoc24::Dyn_bitset& box_map(Adjacent_subtile::Type t) const;
	const std::string type_to_str(Adjacent_subtile::Type t) const;
	int presence_in_maps(const size_t pos) const;
	bool on_no_map(const aoc24::Vec2d& xy, const aoc24::Direction dir) const;
	bool only_on_a_single_map(const aoc24::Vec2d& pos,
	                          const aoc24::Direction dir,
	                          const aoc24::Dyn_bitset& only_map,
	                          const std::string& expected_type) const;
	bool adj_subtile_is_invalid_side(const aoc24::Vec2d& pos,
	                                 const Adjacent_tile_wide& tile,
	                                 const aoc24::Direction dir,
	                                 const bool stuck = false) const;
	bool unused_subtile_is_unknown(const aoc24::Vec2d& pos,
	                               const Adjacent_tile_wide& tile,
	                               const aoc24::Direction dir) const;
	Tile_validation validate_if_adj_unknown(const Adjacent_tile_wide& adj_t,
	                                        const aoc24::Vec2d& adj_xy_L,
	                                        const aoc24::Vec2d& adj_xy_R,
	                                        const aoc24::Direction dir,
	                                        const bool found_left,
	                                        const bool found_right);
	Tile_validation validate_if_adj_empty(const Adjacent_tile_wide& adj_t,
	                                      const aoc24::Vec2d& adj_xy_L,
	                                      const aoc24::Vec2d& adj_xy_R,
	                                      const aoc24::Direction dir,
	                                      const bool found_left,
	                                      const bool found_right);
	Tile_validation validate_if_adj_wall(const Adjacent_tile_wide& adj_t,
	                                     const aoc24::Vec2d& adj_xy_L,
	                                     const aoc24::Vec2d& adj_xy_R,
	                                     const aoc24::Direction dir,
	                                     const bool found_left,
	                                     const bool found_right);
	Tile_validation validate_if_adj_box(const Adjacent_tile_wide& adj_t,
	                                    const aoc24::Vec2d& adj_xy_L,
	                                    const aoc24::Vec2d& adj_xy_R,
	                                    const aoc24::Direction dir,
	                                    const bool found_left,
	                                    const bool found_right);
};
} // namespace aoc24_15

#endif // BOX_GRID_WIDE_H
