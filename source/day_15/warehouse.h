#ifndef WAREHOUSE_H
#define WAREHOUSE_H
#include "../lib/bit_grid.h"
#include "robot.h"
#include <iostream>
#include <memory>
#include <vector>

namespace aoc24_15 {

// Forward declarations
class Box_new;
class Box_factory;

struct Box_integrity;

class Warehouse : public aoc24::Bit_grid
{
	friend class Warehouse_collision;

private:
	bool map_ptr_init_;
	int num_moves_;
	Robot robot_;
	std::vector<std::unique_ptr<Box_new>> boxes_;
	std::vector<aoc24::Vec2d> stuck_box_positions_;

	aoc24::Dyn_bitset* walls_;
	aoc24::Dyn_bitset* stuck_boxes_;

	aoc24::Dyn_bitset& walls_map() const;
	aoc24::Dyn_bitset& stuck_map() const;

	void init_maps();
	void add_walls(const aoc24::Char_grid& reference_grid);
	void prune_stuck_boxes();

	void add_box_to_map(const Box_new& box, aoc24::Dyn_bitset& map);
	void add_box_to_print_map(const Box_new& box) const;

	void throw_if_no_map() const;

	// Collision detection
	const Box_new* find_box_at(int top_L_x, int top_L_y) const;
	Box_new* find_mutable_box_at(int top_L_x, int top_L_y) const;
	std::vector<const Box_new*> find_boxes_in_rect(int top_L_x,
	                                               int top_L_y,
	                                               int w,
	                                               int h) const;
	std::vector<Box_new*> find_mutable_boxes_in_rect(int top_L_x,
	                                                 int top_L_y,
	                                                 int w,
	                                                 int h) const;

	// Push boxes

	// Returns a vector of pointers to Box objects that are movable in the
	// given direction, starting from AND including the given start Box object.
	// (A chain of connected boxes that can be moved together)
	//
	// Returns an empty vector if any of the Boxes are blocked by the bounds of
	// the Warehouse or by a stationary object.
	std::vector<Box_new*> movable_adj_boxes_in_dir(const Box_new& start_box,
	                                               aoc24::Direction dir) const;

	void move_boxes(std::vector<Box_new*> boxes, aoc24::Direction dir);

public:
	const Warehouse_collision* collision;
	Warehouse(size_t width,
	          size_t height,
	          std::unique_ptr<aoc24::Dyn_bitset> dyn_bitset,
	          const aoc24::Char_grid& reference_grid);
	void spawn_robot(int pos_x, int pos_y);
	void move_robot(aoc24::Direction move_dir);
	void add_box(Box_factory& f, int pos_x, int pos_y);
	void print_map(std::ostream& ostr = std::cout) const;

	void test_boxes() const;
	int number_of_moves() const { return num_moves_; }
	Box_integrity box_integrity() const;
	long long sum_of_all_box_coordinates() const;
};

struct Box_integrity
{
	Box_integrity()
	    : has_errors(false)
	{}
	Box_integrity(const Box_new& box,
	              const aoc24::Vec2d& tile_pos,
	              const std::string& expected,
	              const std::string& found);
	bool has_errors;
	aoc24::Vec2d faulty_box_initial_position;
	aoc24::Vec2d faulty_box_position;
	aoc24::Vec2d faulty_edge_tile_position;
	std::string expected_what;
	std::string found_what;
};

} // namespace aoc24_15

#endif // WAREHOUSE_H
