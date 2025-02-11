#include "maze.h"
#include "keys.h"
#include "path.h"
#include <array>

namespace {
bool pos_is_xy(
    size_t pos, int x, int y, int w)
{
	return static_cast<int>(pos) == (y * w) + x;
}
} // namespace

aoc24_16::Maze::Maze(
    std::unique_ptr<aoc24::Dyn_bitset> dyn_bitset,
    const aoc24::Char_grid& reference_grid,
    int start_x,
    int start_y,
    int end_x,
    int end_y)
    : Bit_grid(reference_grid.width(),
               reference_grid.height(),
               std::move(dyn_bitset))
    // State
    , state_{Maze_state::Uninitialized}

    // Maps
    , dummy_(nullptr)   // Dummy
    , walls_(nullptr)   // Walls
    , fk_URDL_(nullptr) // Forks
    , frk_URD_(nullptr)
    , frk_RDL_(nullptr)
    , frk_URL_(nullptr)
    , frk_UDL_(nullptr)
    , crnr_UR_(nullptr) // Corners
    , crnr_RD_(nullptr)
    , crnr_DL_(nullptr)
    , crnr_UL_(nullptr)
    , uturn_U_(nullptr) // U-turns
    , uturn_R_(nullptr)
    , uturn_D_(nullptr)
    , uturn_L_(nullptr)
    , pass_UD_(nullptr) // Passages
    , pass_RL_(nullptr)
    , dd_ends_(nullptr) // Dead-ends
    , cmb_fcu_(nullptr) // Combined Forks, Corners and U-turns
    , pas_all_(nullptr) // Both horizontal and vertical passages

    // Start / End
    , start_(start_x, start_y)
    , end_(end_x, end_y)
{
	init_maps();
	add_walls(reference_grid);
	validate_start_and_end();
	mark_path_tiles();
	mark_dead_ends();
	if (state_ != Maze_state::Dead_ends_marked) {
		throw std::logic_error("Invalid state");
	}
}

int aoc24_16::Maze::number_of_forks_corners_and_u_turns() const
{
	throw_if_not_ready();

	return static_cast<int>(fork_corner_u_turn_map().count());
}

aoc24_16::Maze::Vec aoc24_16::Maze::end_position() const
{
	throw_if_not_ready();
	return {end_.x, end_.y};
}

aoc24_16::Maze::Vec aoc24_16::Maze::start_position() const
{
	throw_if_not_ready();
	return {start_.x, start_.y};
}

aoc24_16::Maze::Vec aoc24_16::Maze::get_next_point_in_dir(
    int pos_x, int pos_y, Dir dir) const
{
	throw_if_not_ready();

	const size_t start_pos = pos_y * sz + pos_x;
	size_t pos = start_pos;

	do {
		pos = offset_pos(pos, dir);
	} while (!pos_is_xy(pos, end_.x, end_.y, static_cast<int>(sz)) && !dead_end_map().test(pos)
	         && all_passages_map().test(pos));

	return {static_cast<int>(pos % sz), static_cast<int>(pos / sz)};
}

std::vector<aoc24::Direction> aoc24_16::Maze::get_start_point_directions() const
{
	throw_if_not_ready();

	std::array<bool, 4> dirs_bool = tile_directions(start_.x, start_.y);
	int num_dirs = dirs_bool[0] + dirs_bool[1] + dirs_bool[2] + dirs_bool[3];

	if (num_dirs == 0) {
		return {};
	}

	std::vector<aoc24::Direction> dirs;
	dirs.reserve(num_dirs);
	for (int i = 0; i < 4; ++i) {
		if (dirs_bool[i]) {
			dirs.emplace_back(static_cast<aoc24::Direction>(i));
		}
	}

	return dirs;
}

std::vector<aoc24_16::Maze::Dir> aoc24_16::Maze::get_available_directions_at_point(
    int pos_x, int pos_y, Dir in_dir) const
{
	throw_if_not_ready();

	if (!valid_xy(pos_x, pos_y)) {
		throw std::invalid_argument("Position OOB");
	}

	if (dead_end_map().test(static_cast<size_t>(pos_y * sz + pos_x))) {
		return {};
	}

	std::array<bool, 4> dirs_bool = tile_directions(pos_x, pos_y, true);
	int num_dirs = (std::max)(0,
	                          (dirs_bool[0] + dirs_bool[1] + dirs_bool[2]
	                           + dirs_bool[3] - 1));

	if (num_dirs == 0) {
		return {};
	}

	Dir inv_in_dir = aoc24::dir_inverted(in_dir);
	std::vector<Dir> dirs;
	dirs.reserve(num_dirs);
	for (int i = 0; i < 4; ++i) {
		if (dirs_bool[i]) {
			Dir new_dir = static_cast<Dir>(i);
			if (new_dir != inv_in_dir) {
				dirs.emplace_back(static_cast<Dir>(i));
			}
		}
	}

	return dirs;
}

//------------------------------------------------------------------------------

void aoc24_16::Maze::init_maps()
{
	if (state_ != Maze_state::Uninitialized) {
		return;
	}
	dummy_ = &add_map(keys::dummy_key);
	walls_ = &add_map(keys::wall_key);
	fk_URDL_ = &add_map(keys::fork_URDL_key);
	frk_URD_ = &add_map(keys::fork_URD_key);
	frk_RDL_ = &add_map(keys::fork_RDL_key);
	frk_URL_ = &add_map(keys::fork_URL_key);
	frk_UDL_ = &add_map(keys::fork_UDL_key);
	crnr_UR_ = &add_map(keys::corner_UR_key);
	crnr_RD_ = &add_map(keys::corner_RD_key);
	crnr_DL_ = &add_map(keys::corner_DL_key);
	crnr_UL_ = &add_map(keys::corner_UL_key);
	uturn_U_ = &add_map(keys::u_turn_U_key);
	uturn_R_ = &add_map(keys::u_turn_R_key);
	uturn_D_ = &add_map(keys::u_turn_D_key);
	uturn_L_ = &add_map(keys::u_turn_L_key);
	pass_UD_ = &add_map(keys::passage_UD_key);
	pass_RL_ = &add_map(keys::passage_RL_key);
	dd_ends_ = &add_map(keys::dead_end_key);
	cmb_fcu_ = &add_map(keys::fork_corner_u_turn_key);
	pas_all_ = &add_map(keys::all_passages_key);

	state_ = Maze_state::Maps_initialized;
}

void aoc24_16::Maze::add_walls(
    const aoc24::Char_grid& reference_grid)
{
	throw_if_no_map();
	size_t pos = 0;
	for (size_t row = 0; row < sz_y; ++row) {
		for (size_t col = 0; col < sz; ++col) {
			char c = reference_grid.char_at(col, row);
			switch (c) {
			case chars::empty_char:
				break;
			case chars::wall_char:
				wall_map().set(pos);
				break;
			default:
				throw std::invalid_argument(
				    "The reference grid contains unknown character '"
				    + std::string{c} + "'.");
			}
			++pos;
		}
	}

	state_ = Maze_state::Walls_marked;
}

void aoc24_16::Maze::mark_path_tiles()
{
	if (state_ != Maze_state::Start_and_end_validated) {
		throw std::logic_error("Invalid state");
	}

	const size_t pos_begin = sz + 1;            // Skip first row and column
	const size_t pos_break = (sz - 1) * sz - 1; // Stop before last row

	for (size_t pos = pos_begin; pos < pos_break; ++pos) {
		// Skip first / last column
		if ((pos % sz) == 0 || (pos % sz) == (sz - 1)) {
			continue;
		}

		// Skip walls
		if (wall_map().test(pos)) {
			continue;
		}

		std::vector<Dir> adj_tiles{adjacent_empty_tiles(pos)};

		// Skip enclosed tiles
		if (adj_tiles.empty()) {
			continue;
		}

		// Get map and mark position
		Map& map = map_from_adjacent_empty_tiles(adj_tiles);
		map.set(pos);
	}

	// Combine Forks, Corners and U-turns into a single map
	fork_corner_u_turn_map() |= fork_map(Dirs::All_directions);
	fork_corner_u_turn_map() |= fork_map(Dir::Up, Dir::Right, Dir::Down);
	fork_corner_u_turn_map() |= fork_map(Dir::Right, Dir::Down, Dir::Left);
	fork_corner_u_turn_map() |= fork_map(Dir::Down, Dir::Left, Dir::Up);
	fork_corner_u_turn_map() |= fork_map(Dir::Left, Dir::Up, Dir::Right);
	fork_corner_u_turn_map() |= corner_map(Dir::Up, Dir::Right);
	fork_corner_u_turn_map() |= corner_map(Dir::Up, Dir::Left);
	fork_corner_u_turn_map() |= corner_map(Dir::Down, Dir::Right);
	fork_corner_u_turn_map() |= u_turn_map(Dir::Up);
	fork_corner_u_turn_map() |= u_turn_map(Dir::Right);
	fork_corner_u_turn_map() |= u_turn_map(Dir::Down);
	fork_corner_u_turn_map() |= u_turn_map(Dir::Left);

	// Combine Passages into a single map
	all_passages_map() |= passage_map(Dir::Up);
	all_passages_map() |= passage_map(Dir::Right);

	state_ = Maze_state::Path_tiles_marked;
}

void aoc24_16::Maze::mark_dead_ends()
{
	if (state_ != Maze_state::Path_tiles_marked) {
		throw std::logic_error("Invalid state");
	}

	// Search for U-turns
	const size_t pos_begin = sz + 1;            // Skip first row and column
	const size_t pos_break = (sz - 1) * sz - 1; // Stop before last row

	for (size_t pos = pos_begin; pos < pos_break; ++pos) {
		Dir dir;
		bool has_u_turn = false;
		for (int i = 0; i < 4; ++i) {
			Dir test_dir = static_cast<Dir>(i);
			if (u_turn_map(test_dir).test(pos)) {
				dir = aoc24::dir_inverted(test_dir);
				has_u_turn = true;
				break;
			}
		}

		if (!has_u_turn) {
			continue;
		}

		find_and_mark_connected_dead_ends(pos, dir);
	}
	state_ = Maze_state::Dead_ends_marked;
}

void aoc24_16::Maze::find_and_mark_connected_dead_ends(
    size_t pos, Dir dir)
{
	// Stop if the tile is already a dead end
	if (dead_end_map().test(pos)) {
		return;
	}

	// Stop if it's a start / end tile
	if ((pos == static_cast<int>(start_.y * sz + start_.x)
	     || pos == static_cast<int>(end_.y * sz + end_.x))) {
		return;
	}

	// U-turn in oppposite direction
	Map& uturn_same_dir_map = u_turn_map(dir);
	if (uturn_same_dir_map.test(pos)) {
		dead_end_map().set(pos);
		return;
	}

	// Initial U-turn
	Dir dir_inv = aoc24::dir_inverted(dir);
	Map& uturn_opp_dir_map = u_turn_map(dir_inv);
	if (uturn_opp_dir_map.test(pos)) {
		dead_end_map().set(pos);
		find_and_mark_connected_dead_ends(offset_pos(pos, dir), dir);
		return;
	}

	// Passage
	Map& pass_map = passage_map(dir);
	if (pass_map.test(pos)) {
		dead_end_map().set(pos);
		find_and_mark_connected_dead_ends(offset_pos(pos, dir), dir);
		return;
	}

	// Corner turning right
	Dir dir_R = aoc24::dir_turn_right(dir);
	Map& turn_R_map = corner_map(dir_inv, dir_R);
	if (turn_R_map.test(pos)) {
		dead_end_map().set(pos);
		find_and_mark_connected_dead_ends(offset_pos(pos, dir_R), dir_R);
		return;
	}

	// Corner turning left
	Dir dir_L = aoc24::dir_turn_left(dir);
	Map& turn_L_map = corner_map(dir_inv, dir_L);
	if (turn_L_map.test(pos)) {
		dead_end_map().set(pos);
		find_and_mark_connected_dead_ends(offset_pos(pos, dir_L), dir_L);
		return;
	}
}

//------------------------------------------------------------------------------

std::vector<aoc24_16::Maze::Dir> aoc24_16::Maze::adjacent_empty_tiles(
    size_t center_pos, bool skip_check) const
{
	if (!skip_check && (center_pos < 1 || center_pos >= (sz * sz_y - 1))) {
		throw std::invalid_argument(
		    "Cannot check adjacent, position too close to bounds");
	}
	std::array<size_t, 4> positions{center_pos - sz,
	                                center_pos + 1,
	                                center_pos + sz,
	                                center_pos - 1};
	std::vector<Dir> dirs;
	for (int i = 0; i < 4; ++i) {
		if (!wall_map().test(positions[i])) {
			dirs.push_back(static_cast<Dir>(i));
		}
	}
	return dirs;
}

aoc24_16::Maze::Map& aoc24_16::Maze::map_from_adjacent_empty_tiles(
    const std::vector<Dir> adjacent) const
{
	size_t num_tiles = adjacent.size();

	if (num_tiles == 4) {
		return fork_map(Dirs::All_directions);
	}

	if (num_tiles == 3) {
		return fork_map(adjacent[0], adjacent[1], adjacent[2]);
	}

	if (num_tiles == 2) {
		if (adjacent[0] == aoc24::dir_inverted(adjacent[1])) {
			return passage_map(adjacent.front());
		} else {
			return corner_map(adjacent[0], adjacent[1]);
		}
	}

	if (num_tiles == 1) {
		return u_turn_map(aoc24::dir_inverted(adjacent.front()));
	}

	throw std::invalid_argument("Invalid number of adjacent tiles");
}
//------------------------------------------------------------------------------

void aoc24_16::Maze::throw_if_not_ready() const
{
	if (state_ == Maze_state::Ready) {
		throw std::runtime_error("Maze is not ready");
	}
}

void aoc24_16::Maze::throw_if_no_map() const
{
	if (state_ == Maze_state::Uninitialized) {
		throw std::runtime_error(
		    "Cannot access map: Maps have not been initialized");
	}
}

bool aoc24_16::Maze::is_dummy(
    const Map& m) const
{
	return &m == dummy_;
}
//------------------------------------------------------------------------------

aoc24_16::Maze::Map& aoc24_16::Maze::all_passages_map() const
{
	throw_if_no_map();
	return *pas_all_;
}

aoc24_16::Maze::Map& aoc24_16::Maze::fork_corner_u_turn_map() const
{
	throw_if_no_map();
	return *cmb_fcu_;
}

aoc24_16::Maze::Map& aoc24_16::Maze::dummy_map()
{
	throw_if_no_map();
	return *dummy_;
}

aoc24_16::Maze::Map& aoc24_16::Maze::wall_map() const
{
	throw_if_no_map();
	return *walls_;
}
aoc24_16::Maze::Map& aoc24_16::Maze::dead_end_map() const
{
	throw_if_no_map();
	return *dd_ends_;
}

aoc24_16::Maze::Map& aoc24_16::Maze::passage_map(
    Dir dir) const
{
	throw_if_no_map();
	return (dir == Dir::Up || dir == Dir::Down) ? *pass_UD_ : *pass_RL_;
}

aoc24_16::Maze::Map& aoc24_16::Maze::u_turn_map(
    Dir dir) const
{
	throw_if_no_map();
	static std::array<Map*, 4> ptrs{uturn_U_, uturn_R_, uturn_D_, uturn_L_};
	return *ptrs[static_cast<int>(dir)];
}

aoc24_16::Maze::Map& aoc24_16::Maze::corner_map(
    Dir a, Dir b) const
{
	int ab_sum = dir_flags_[static_cast<int>(a)]
	             + dir_flags_[static_cast<int>(b)];
	return get_corner_map_from_sum(ab_sum);
}

aoc24_16::Maze::Map& aoc24_16::Maze::fork_map(
    Dirs d) const
{
	throw_if_no_map();

	if (d == All_directions) {
		return *fk_URDL_;
	}

	throw std::invalid_argument("Invalid fork map directions");
}

aoc24_16::Maze::Map& aoc24_16::Maze::fork_map(
    Dir a, Dir b, Dir c) const
{
	int abc_sum = dir_flags_[static_cast<int>(a)]
	              + dir_flags_[static_cast<int>(b)]
	              + dir_flags_[static_cast<int>(c)];
	return get_fork_map_from_sum(abc_sum);
}

//------------------------------------------------------------------------------

void aoc24_16::Maze::validate_start_and_end()
{
	if (state_ != Maze_state::Walls_marked) {
		throw std::logic_error("Invalid state");
	}

	if (!valid_xy(static_cast<int>(start_.x), static_cast<int>(start_.y))) {
		throw std::logic_error("Start position is OOB");
	}

	if (!valid_xy(static_cast<int>(start_.x), static_cast<int>(start_.y))) {
		throw std::logic_error("End position is OOB");
	}

	size_t pos_start = start_.y * sz + start_.x;
	if (wall_map().test(pos_start)) {
		throw std::logic_error("Start position is also a wall tile");
	}

	size_t pos_end = end_.y * sz + end_.x;
	if (wall_map().test(pos_end)) {
		throw std::logic_error("End position is also a wall tile");
	}

	state_ = Maze_state::Start_and_end_validated;
}

//------------------------------------------------------------------------------

aoc24_16::Maze::Map& aoc24_16::Maze::get_fork_map_from_sum(
    int sum) const
{
	throw_if_no_map();

	switch (sum) {
	case 14:
		return *frk_URD_;
	case 28:
		return *frk_RDL_;
	case 22:
		return *frk_URL_;
	case 26:
		return *frk_UDL_;
	}

	throw std::invalid_argument("Invalid fork map directions");
}

aoc24_16::Maze::Map& aoc24_16::Maze::get_corner_map_from_sum(
    int sum) const
{
	throw_if_no_map();

	switch (sum) {
	case 6:
		return *crnr_UR_;
	case 12:
		return *crnr_RD_;
	case 18:
		return *crnr_UL_;
	case 24:
		return *crnr_DL_;
	}

	throw std::invalid_argument("Invalid corner map directions");
}

//------------------------------------------------------------------------------

void aoc24_16::Maze::internal_print_map(
    std::ostream& ostr, bool verbose) const
{
	size_t pos_start = start_.y * sz + start_.x;
	size_t pos_end = end_.y * sz + end_.x;
	for (size_t i = 0; i < (sz * sz_y); ++i) {
		char c = ' '; // Empty by default
		// if (all_passages_map().test(i)) {
		// 	c = 'P';
		// }
		if (print_->test(i)) {
			c = chars::seat_char;
		} else if (i == pos_start) {
			c = chars::start_char;
		} else if (i == pos_end) {
			c = chars::end_char;
		} else if (dead_end_map().test(i)) {
			c = chars::dead_end_char;
		} else if (wall_map().test(i)) {
			c = chars::wall_char;
		} else if (fork_map(Dirs::All_directions).test(i)) {
			c = !verbose ? chars::generic_fork_char : chars::fork_URDL_char;
		} else if (fork_map(Dir::Up, Dir::Right, Dir::Down).test(i)) {
			c = !verbose ? chars::generic_fork_char : chars::fork_URD_char;
		} else if (fork_map(Dir::Right, Dir::Down, Dir::Left).test(i)) {
			c = !verbose ? chars::generic_fork_char : chars::fork_RDL_char;
		} else if (fork_map(Dir::Down, Dir::Left, Dir::Up).test(i)) {
			c = !verbose ? chars::generic_fork_char : chars::fork_UDL_char;
		} else if (fork_map(Dir::Left, Dir::Up, Dir::Right).test(i)) {
			c = !verbose ? chars::generic_fork_char : chars::fork_URL_char;
		} else if (corner_map(Dir::Up, Dir::Right).test(i)) {
			c = !verbose ? chars::generic_fork_char : chars::corner_UR_char;
		} else if (corner_map(Dir::Right, Dir::Down).test(i)) {
			c = !verbose ? chars::generic_fork_char : chars::corner_RD_char;
		} else if (corner_map(Dir::Down, Dir::Left).test(i)) {
			c = !verbose ? chars::generic_fork_char : chars::corner_DL_char;
		} else if (corner_map(Dir::Left, Dir::Up).test(i)) {
			c = !verbose ? chars::generic_fork_char : chars::corner_UL_char;
		} else if (u_turn_map(Dir::Up).test(i)) {
			c = !verbose ? chars::generic_u_turn_char : chars::u_turn_U_char;
		} else if (u_turn_map(Dir::Right).test(i)) {
			c = !verbose ? chars::generic_u_turn_char : chars::u_turn_R_char;
		} else if (u_turn_map(Dir::Down).test(i)) {
			c = !verbose ? chars::generic_u_turn_char : chars::u_turn_D_char;
		} else if (u_turn_map(Dir::Left).test(i)) {
			c = !verbose ? chars::generic_u_turn_char : chars::u_turn_L_char;
		} else if (passage_map(Dir::Right).test(i)) {
			c = chars::passage_RL_char;
		} else if (passage_map(Dir::Up).test(i)) {
			c = chars::passage_UD_char;
		}
		ostr << ((i % sz) == 0 ? "" : " ") << c;
		if ((i != 0) && (((i + 1) % sz) == 0)) {
			ostr << '\n';
		}
	}
}

void aoc24_16::Maze::print_map(
    std::ostream& ostr) const
{
	print_->reset();
	internal_print_map(ostr, false);
}

void aoc24_16::Maze::print_map_verbose(
    std::ostream& ostr) const
{
	print_->reset();
	internal_print_map(ostr, true);
}

void aoc24_16::Maze::print_paths(
    const std::vector<Path>& paths, std::ostream& ostr) const
{
	merge_paths_onto_print_map(paths);
	internal_print_map(ostr, true);
}

//------------------------------------------------------------------------------

int aoc24_16::Maze::dir_to_index_based_offset(
    Dir dir) const
{
	static std::array<int, 4> offs{-static_cast<int>(sz),
	                               1,
	                               static_cast<int>(sz),
	                               -1};
	return offs[static_cast<int>(dir)];
}

size_t aoc24_16::Maze::offset_pos(
    size_t pos, Dir dir, bool skip_check) const
{
	if (!skip_check) {
		if ((dir == Dir::Up && pos < sz)
		    || (dir == Dir::Right && (pos % sz) == (sz - 1))
		    || (dir == Dir::Down && pos >= (sz * (sz_y - 1)))
		    || (dir == Dir::Left && (pos % sz) == 0)) {
			throw std::invalid_argument("Offset position is OOB");
		}
	}

	return static_cast<size_t>(pos + dir_to_index_based_offset(dir));
}

//------------------------------------------------------------------------------
int aoc24_16::Maze::merge_and_count_path_tiles(
    const std::vector<Path>& paths) const
{
	merge_paths_onto_print_map(paths);
	return static_cast<int>(print_->count());
}

void aoc24_16::Maze::merge_paths_onto_print_map(
    const std::vector<Path> paths) const
{
	print_->reset();
	for (const auto& path : paths) {
		std::vector<Vec> pts = path.points();
		for (int i = 0; i < pts.size() - 1; ++i) {
			const Vec& a = pts[i];
			const Vec& b = (pts.size() == 1) ? pts[i] : pts[i + 1];
			if (i == 0) {
				if (!valid_xy(a.x, a.y)) {
					throw std::invalid_argument("Point is OOB");
				}
			}
			if (!valid_xy(b.x, b.y)) {
				throw std::invalid_argument("Point is OOB");
			}

			const size_t start_pos = a.y * sz + a.x;
			int num_tiles = 0;
			int offset = 0;
			if (a.y != b.y) {
				// Row
				num_tiles = (std::abs)(a.y - b.y) + 1;
				offset = (a.y <= b.y) ? sz : -sz;
			} else {
				// Column
				num_tiles = (std::abs)(a.x - b.x) + 1;
				offset = (a.x <= b.x) ? 1 : -1;
			}
			int start_tile_idx = (i == 0) ? 0 : 1;
			for (int j = start_tile_idx; j < num_tiles; ++j) {
				print_->set(start_pos + offset * static_cast<size_t>(j));
			}
		}
	}
}

//------------------------------------------------------------------------------

std::array<bool, 4> aoc24_16::Maze::tile_directions(
    int pos_x, int pos_y, bool skip_check) const
{
	if (!skip_check && !valid_xy(pos_x, pos_y)) {
		throw std::invalid_argument("Position OOB");
	}

	size_t pos = pos_y * sz + pos_x;

	if (fork_map(Dirs::All_directions).test(pos)) {
		return {true, true, true, true};
	}

	if (fork_map(Dir::Up, Dir::Right, Dir::Down).test(pos)) {
		return {true, true, true, false};
	}

	if (fork_map(Dir::Right, Dir::Down, Dir::Left).test(pos)) {
		return {false, true, true, true};
	}

	if (fork_map(Dir::Down, Dir::Left, Dir::Up).test(pos)) {
		return {true, false, true, true};
	}

	if (fork_map(Dir::Left, Dir::Up, Dir::Right).test(pos)) {
		return {true, true, false, true};
	}

	if (corner_map(Dir::Up, Dir::Right).test(pos)) {
		return {true, true, false, false};
	}

	if (corner_map(Dir::Up, Dir::Left).test(pos)) {
		return {true, false, false, true};
	}

	if (corner_map(Dir::Down, Dir::Right).test(pos)) {
		return {false, true, true, false};
	}

	if (corner_map(Dir::Down, Dir::Left).test(pos)) {
		return {false, false, true, true};
	}

	if (passage_map(Dir::Up).test(pos)) {
		return {true, false, true, false};
	}

	if (passage_map(Dir::Right).test(pos)) {
		return {false, true, false, true};
	}

	if (u_turn_map(Dir::Up).test(pos)) {
		return {false, false, true, false};
	}

	if (u_turn_map(Dir::Right).test(pos)) {
		return {false, false, false, true};
	}

	if (u_turn_map(Dir::Down).test(pos)) {
		return {true, false, false, false};
	}

	if (u_turn_map(Dir::Left).test(pos)) {
		return {false, false, false, true};
	}

	return {false, false, false, false};
}
