#ifndef MAZE_H
#define MAZE_H

#include "../lib/bit_grid.h"
#include "../lib/direction.h"
#include "../lib/vec2d.h"
#include <array>

namespace aoc24_16 {

class Path;

class Maze : public aoc24::Bit_grid
{
	//
	// State
	//--------------------------------------------------------------------------
	enum class Maze_state {
		Uninitialized,
		Maps_initialized,
		Walls_marked,
		Start_and_end_validated,
		Path_tiles_marked,
		Dead_ends_marked,
		Ready
	};
	Maze_state state_;
	void throw_if_not_ready() const;
	void throw_if_no_map() const;

	//
	// Direction flags
	// Used for fetching Fork and Corner maps.
	// Each direction is assigned a unique pow. of 2 to allow combinations of
	// directions to be represented as sums (Order is not important)
	//--------------------------------------------------------------------------
	static constexpr std::array<int, 4> dir_flags_ = {
	    2, // Up    (2^1)
	    4, // Right (2^2)
	    8, // Down  (2^3)
	    16 // Left  (2^4)
	};

	//
	// Maps
	//--------------------------------------------------------------------------

	void init_maps();

	using Dir = aoc24::Direction;
	using Map = aoc24::Dyn_bitset;
	enum Dirs { All_directions };

	// Dummy
	Map* dummy_;
	Map& dummy_map();
	bool is_dummy(const Map& m) const;

	// Walls
	Map* walls_;
	Map& wall_map() const;

	// Forks
	Map* fk_URDL_;
	Map* frk_URD_;
	Map* frk_RDL_;
	Map* frk_URL_;
	Map* frk_UDL_;
	Map& fork_map(Dir a, Dir b, Dir c) const;
	Map& fork_map(Dirs) const;
	Map& get_fork_map_from_sum(int sum) const;

	// Corners
	Map* crnr_UR_;
	Map* crnr_RD_;
	Map* crnr_DL_;
	Map* crnr_UL_;
	Map& corner_map(Dir a, Dir b) const;
	Map& get_corner_map_from_sum(int sum) const;

	// U-Turns
	Map* uturn_U_;
	Map* uturn_R_;
	Map* uturn_D_;
	Map* uturn_L_;
	Map& u_turn_map(Dir dir) const;

	// Passages
	Map* pass_UD_;
	Map* pass_RL_;
	Map& passage_map(Dir dir) const;

	// Dead-ends (Passages and Corners leading to a U-Turn)
	Map* dd_ends_;
	Map& dead_end_map() const;

	// Combined maps
	Map* cmb_fcu_; // Forks, corners and U-turns
	Map* pas_all_; // Both horizontal and vertical passages
	Map& fork_corner_u_turn_map() const;
	Map& all_passages_map() const;

	//
	// Start / End
	//--------------------------------------------------------------------------

	using Vec = aoc24::Vec2d;
	Vec start_;
	Vec end_;
	void validate_start_and_end();

	//
	// Add / Mark Tiles
	//--------------------------------------------------------------------------

	void add_walls(const aoc24::Char_grid& reference_grid);
	void mark_path_tiles(); // Forks, corners, u-turns, passages
	void mark_dead_ends();

	// Mark the tile at `start_pos` as a dead-end if applicable.
	// Recursively repeat for any connected tiles.
	void find_and_mark_connected_dead_ends(size_t pos, Dir dir);

	std::vector<Dir> adjacent_empty_tiles(size_t pos,
	                                      bool skip_check = true) const;
	Map& map_from_adjacent_empty_tiles(const std::vector<Dir> adjacent) const;

	//
	// Printing
	//--------------------------------------------------------------------------
	void internal_print_map(std::ostream& ostr = std::cout,
	                        bool verbose = false) const;
	//
	// Direction helpers
	//--------------------------------------------------------------------------
	int dir_to_index_based_offset(Dir dir) const;
	size_t offset_pos(size_t pos, Dir dir, bool skip_check = false) const;

	//
	// Paths
	//--------------------------------------------------------------------------
	std::array<bool, 4> tile_directions(int pos_x,
	                                    int pos_y,
	                                    bool skip_check = false) const;
	void merge_paths_onto_print_map(const std::vector<Path> paths) const;

public:
	Maze(std::unique_ptr<aoc24::Dyn_bitset> dyn_bitset,
	     const aoc24::Char_grid& reference_grid,
	     int start_x,
	     int start_y,
	     int end_x,
	     int end_y);

	//
	// Paths
	//--------------------------------------------------------------------------

	// Fetches the available directions for the start point
	std::vector<Dir> get_start_point_directions() const;

	// Fetches the available directions for the tile at the given point
	std::vector<Dir> get_available_directions_at_point(int pos_x,
	                                                   int pos_y,
	                                                   Dir in_dir) const;

	// Fetches the next Fork, Corner, U_turn or Dead-End
	Vec get_next_point_in_dir(int pos_x, int pos_y, Dir dir) const;

	int merge_and_count_path_tiles(const std::vector<Path>& paths) const;

	//
	// Start and end positions
	Vec start_position() const;
	Vec end_position() const;

	int number_of_forks_corners_and_u_turns() const;

	void print_map(std::ostream& ostr = std::cout) const;
	void print_map_verbose(std::ostream& ostr = std::cout) const;
	void print_paths(const std::vector<Path>& paths,
	                 std::ostream& ostr = std::cout) const;
};

} // namespace aoc24_16

#endif // MAZE_H
