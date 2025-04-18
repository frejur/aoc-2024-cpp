#include "bytegrid.h"
#include "keys.h"
#include "point.h"
#include <array>

namespace {
constexpr size_t oob_pos = -1;
constexpr int no_idx{-1};
bool pos_is_xy(
    size_t pos, int x, int y, int w)
{
	return static_cast<int>(pos) == (y * w) + x;
}
} // namespace

constexpr std::array<int, 4> aoc24_18::Byte_grid::dir_flags_;

aoc24_18::Byte_grid::Byte_grid(
    std::unique_ptr<aoc24::Dyn_bitset> dyn_bitset,
    size_t size,
    int start_x,
    int start_y,
    int end_x,
    int end_y)
    : Bit_grid(size, std::move(dyn_bitset))
    // State
    , state_{Byte_grid_state::Uninitialized}

    // Maps
    , dummy_(nullptr)   // Dummy
    , bytes_(nullptr)   // Corrupted bytes
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
    , sh_path_(nullptr) // Shortest path

    // Start / End
    , start_(start_x, start_y)
    , end_(end_x, end_y)
{
	init_maps();
	update_state();
}
//------------------------------------------------------------------------------

void aoc24_18::Byte_grid::add_corrupted_byte(int pos_x, int pos_y, bool update)
{
	if (state_ != Byte_grid_state::Ready
	    && state_ != Byte_grid_state::Corrupted_bytes_marked) {
		throw std::runtime_error("Invalid state");
	}

	if (pos_x < 0 || pos_x > sz || pos_y < 0 || pos_y > (sz * sz - 1)) {
		throw std::invalid_argument("Position out of bounds");
	}
	new_cor_bytes_.emplace_back(pos_x, pos_y);
	state_ = Byte_grid_state::Corrupted_bytes_marked;
}

void aoc24_18::Byte_grid::update_corrupted_bytes()
{
	update_state();
}

//------------------------------------------------------------------------------

int aoc24_18::Byte_grid::number_of_forks_corners_and_u_turns() const
{
	throw_if_not_ready();

	return static_cast<int>(fork_corner_u_turn_map().count());
}

aoc24_18::Byte_grid::Vec aoc24_18::Byte_grid::end_position() const
{
	throw_if_not_ready();
	return {end_.x, end_.y};
}

aoc24_18::Byte_grid::Vec aoc24_18::Byte_grid::start_position() const
{
	throw_if_not_ready();
	return {start_.x, start_.y};
}

aoc24_18::Byte_grid::Vec aoc24_18::Byte_grid::get_next_point_in_dir(
    int pos_x, int pos_y, Dir dir) const
{
	throw_if_not_ready();

	const size_t start_pos = pos_y * sz + pos_x;
	size_t pos = start_pos;

	do {
		size_t offs{offset_pos(pos, dir)};
		if (offs != oob_pos) {
			pos = offs;
		}
	} while (!pos_is_xy(pos, end_.x, end_.y, static_cast<int>(sz))
	         && !dead_end_map().test(pos) && all_passages_map().test(pos));

	return {static_cast<int>(pos % sz), static_cast<int>(pos / sz)};
}

std::vector<aoc24::Direction> aoc24_18::Byte_grid::get_start_point_directions()
    const
{
	throw_if_not_ready();
	return get_available_directions_at_point(start_.x, start_.y);
}

std::vector<aoc24::Direction> aoc24_18::Byte_grid::get_end_point_directions() const
{
	throw_if_not_ready();
	return get_available_directions_at_point(end_.x, end_.y);
}

//------------------------------------------------------------------------------

std::vector<aoc24_18::Byte_grid::Dir>
aoc24_18::Byte_grid::get_available_directions_at_point(
    int pos_x, int pos_y) const
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
	                           + dirs_bool[3]));

	if (num_dirs == 0) {
		return {};
	}

	std::vector<Dir> dirs;
	dirs.reserve(num_dirs);
	for (int i = 0; i < 4; ++i) {
		if (dirs_bool[i]) {
			dirs.emplace_back(static_cast<Dir>(i));
		}
	}

	return dirs;
}

std::vector<aoc24_18::Byte_grid::Dir>
aoc24_18::Byte_grid::get_available_directions_at_point(int pos_x,
                                                       int pos_y,
                                                       Dir in_dir) const
{
	std::vector<aoc24_18::Byte_grid::Dir> dirs{
	    get_available_directions_at_point(pos_x, pos_y)};

	Dir inv_in_dir = aoc24::dir_inverted(in_dir);
	int delete_idx = no_idx;
	for (int i = 0; i < dirs.size(); ++i) {
		if (dirs[i] == inv_in_dir) {
			delete_idx = i;
			break;
		}
	}
	if (delete_idx != no_idx) {
		dirs.erase(dirs.begin() + delete_idx);
	}

	return dirs;
}

//------------------------------------------------------------------------------

void aoc24_18::Byte_grid::init_maps()
{
	if (state_ != Byte_grid_state::Uninitialized) {
		return;
	}
	dummy_ = &add_map(keys::dummy_key);
	bytes_ = &add_map(keys::byte_key);
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
	sh_path_ = &add_map(keys::shortest_path_key);

	state_ = Byte_grid_state::Maps_initialized;
}

void aoc24_18::Byte_grid::mark_corrupted_bytes()
{
	throw_if_no_map();
	for (const aoc24::Vec2d& b : new_cor_bytes_) {
		const size_t pos{xy_to_idx(b.x, b.y, true)};
		byte_map().set(pos);
		clear_from_all_but_byte_map(pos);
	}
	state_ = Byte_grid_state::Corrupted_bytes_marked;
}

void aoc24_18::Byte_grid::mark_path_tiles()
{
	if (state_ != Byte_grid_state::Start_and_end_validated) {
		throw std::logic_error("Invalid state");
	}

	const size_t pos_break = sz * sz - 1;
	for (size_t pos = 0; pos <= pos_break; ++pos) {
		// Skip corrupted bytes
		if (byte_map().test(pos)) {
			continue;
		}

		std::vector<Dir> adj_tiles{adjacent_empty_tiles(pos)};

		// Skip enclosed tiles
		if (adj_tiles.empty()) {
			continue;
		}

		// Get map and mark position
		clear_from_all_but_byte_map(pos);
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

	state_ = Byte_grid_state::Path_tiles_marked;
}

void aoc24_18::Byte_grid::mark_dead_ends()
{
	if (state_ != Byte_grid_state::Path_tiles_marked) {
		throw std::logic_error("Invalid state");
	}

	// Search for U-turns
	const size_t pos_break = sz * sz;

	for (size_t pos = 0; pos < pos_break; ++pos) {
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
	state_ = Byte_grid_state::Dead_ends_marked;
}

void aoc24_18::Byte_grid::mark_shortest_path(
    const std::vector<Point> path) const
{
	auto& pmap = shortest_path_map();
	pmap.reset();

	for (int i = 0; i < path.size() - 1; ++i) {
		const Vec& a = path[i].position();
		const Vec& b = (path.size() == 1) ? path[i].position()
		                                  : path[i + 1].position();
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
			pmap.set(start_pos + offset * static_cast<size_t>(j));
		}
	}
}

void aoc24_18::Byte_grid::find_and_mark_connected_dead_ends(size_t pos, Dir dir)
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
		find_and_mark_connected_dead_ends(offset_pos_or_throw(pos, dir), dir);
		return;
	}

	// Passage
	Map& pass_map = passage_map(dir);
	if (pass_map.test(pos)) {
		dead_end_map().set(pos);
		find_and_mark_connected_dead_ends(offset_pos_or_throw(pos, dir), dir);
		return;
	}

	// Corner turning right
	Dir dir_R = aoc24::dir_turn_right(dir);
	Map& turn_R_map = corner_map(dir_inv, dir_R);
	if (turn_R_map.test(pos)) {
		dead_end_map().set(pos);
		find_and_mark_connected_dead_ends(offset_pos_or_throw(pos, dir_R),
		                                  dir_R);
		return;
	}

	// Corner turning left
	Dir dir_L = aoc24::dir_turn_left(dir);
	Map& turn_L_map = corner_map(dir_inv, dir_L);
	if (turn_L_map.test(pos)) {
		dead_end_map().set(pos);
		find_and_mark_connected_dead_ends(offset_pos_or_throw(pos, dir_L),
		                                  dir_L);
		return;
	}

	// Forks (Three/four-way)
	std::vector<Dir> adj_tiles{adjacent_empty_tiles(pos)};
	size_t num_adj{adj_tiles.size()};
	if (num_adj < 3 || num_adj > 4) {
		throw std::runtime_error(
		    "Expected a fork with 3 or 4 connecting pathways");
	}

	int matching_dir_idx{no_idx};
	for (int i = 0; i < num_adj; ++i) {
		if (dir_inv == adj_tiles[i]) {
			Map& old_fork_map = map_from_adjacent_empty_tiles(adj_tiles);
			old_fork_map.set(pos, false); // Clear current junction
			matching_dir_idx = i;
			break;
		}
	}

	if (matching_dir_idx == no_idx) {
		throw std::runtime_error(
		    "Dead end direction does not match adjacent fork");
	}

	adj_tiles.erase(adj_tiles.begin() + matching_dir_idx);

	Map& new_fork_map = map_from_adjacent_empty_tiles(adj_tiles);
	new_fork_map.set(pos);
}

void aoc24_18::Byte_grid::clear_from_all_but_byte_map(
    size_t pos, bool skip_check)
{
	if (!skip_check && (pos < 0 || pos > (sz * sz - 1))) {
		throw std::invalid_argument("Position is out of bounds");
	}

	// Forks
	fk_URDL_->set(pos, false);
	frk_URD_->set(pos, false);
	frk_RDL_->set(pos, false);
	frk_URL_->set(pos, false);
	frk_UDL_->set(pos, false);

	// Corners
	crnr_UR_->set(pos, false);
	crnr_RD_->set(pos, false);
	crnr_DL_->set(pos, false);
	crnr_UL_->set(pos, false);

	// U-Turns
	uturn_U_->set(pos, false);
	uturn_R_->set(pos, false);
	uturn_D_->set(pos, false);
	uturn_L_->set(pos, false);

	// Passages
	pass_UD_->set(pos, false);
	pass_RL_->set(pos, false);

	// Dead-ends
	dd_ends_->set(pos, false);

	// Combined maps
	cmb_fcu_->set(pos, false);
	pas_all_->set(pos, false);
}

//------------------------------------------------------------------------------

std::vector<aoc24_18::Byte_grid::Dir> aoc24_18::Byte_grid::adjacent_empty_tiles(
    size_t center_pos, bool skip_check) const
{
	if (!skip_check && (center_pos < 0 || center_pos > (sz * sz - 1))) {
		throw std::invalid_argument("Position is out of bounds");
	}
	bool left_bounds = ((center_pos == 0) || (center_pos % sz == 0));
	bool right_bounds = ((center_pos > 0) && (((center_pos + 1) % sz) == 0));
	bool top_bounds = (center_pos < sz);
	bool bottom_bounds = (center_pos >= sz * sz - sz);

	std::array<size_t, 4> positions{top_bounds ? oob_pos : center_pos - sz,
	                                right_bounds ? oob_pos : center_pos + 1,
	                                bottom_bounds ? oob_pos : center_pos + sz,
	                                left_bounds ? oob_pos : center_pos - 1};

	std::vector<Dir> dirs;
	for (int i = 0; i < 4; ++i) {
		if (!(positions[i] == oob_pos) && !byte_map().test(positions[i])) {
			dirs.push_back(static_cast<Dir>(i));
		}
	}
	return dirs;
}

aoc24_18::Byte_grid::Map& aoc24_18::Byte_grid::map_from_adjacent_empty_tiles(
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

void aoc24_18::Byte_grid::throw_if_not_ready() const
{
	if (state_ != Byte_grid_state::Ready) {
		throw std::runtime_error("Byte_grid is not ready");
	}
}

void aoc24_18::Byte_grid::throw_if_no_map() const
{
	if (state_ == Byte_grid_state::Uninitialized) {
		throw std::runtime_error(
		    "Cannot access map: Maps have not been initialized");
	}
}

void aoc24_18::Byte_grid::update_state()
{
	mark_corrupted_bytes();
	validate_start_and_end();
	mark_path_tiles();
	mark_dead_ends();
	if (state_ != Byte_grid_state::Dead_ends_marked) {
		throw std::logic_error("Invalid state");
	}
	state_ = Byte_grid_state::Ready;
}

bool aoc24_18::Byte_grid::is_dummy(const Map& m) const
{
	return &m == dummy_;
}

//------------------------------------------------------------------------------

aoc24_18::Byte_grid::Map& aoc24_18::Byte_grid::shortest_path_map() const
{
	throw_if_no_map();
	return *sh_path_;
}

aoc24_18::Byte_grid::Map& aoc24_18::Byte_grid::all_passages_map() const
{
	throw_if_no_map();
	return *pas_all_;
}

aoc24_18::Byte_grid::Map& aoc24_18::Byte_grid::fork_corner_u_turn_map() const
{
	throw_if_no_map();
	return *cmb_fcu_;
}

aoc24_18::Byte_grid::Map& aoc24_18::Byte_grid::dummy_map()
{
	throw_if_no_map();
	return *dummy_;
}

aoc24_18::Byte_grid::Map& aoc24_18::Byte_grid::byte_map() const
{
	throw_if_no_map();
	return *bytes_;
}
aoc24_18::Byte_grid::Map& aoc24_18::Byte_grid::dead_end_map() const
{
	throw_if_no_map();
	return *dd_ends_;
}

aoc24_18::Byte_grid::Map& aoc24_18::Byte_grid::passage_map(Dir dir) const
{
	throw_if_no_map();
	return (dir == Dir::Up || dir == Dir::Down) ? *pass_UD_ : *pass_RL_;
}

aoc24_18::Byte_grid::Map& aoc24_18::Byte_grid::u_turn_map(Dir dir) const
{
	throw_if_no_map();
	static std::array<Map*, 4> ptrs{uturn_U_, uturn_R_, uturn_D_, uturn_L_};
	return *ptrs[static_cast<int>(dir)];
}

aoc24_18::Byte_grid::Map& aoc24_18::Byte_grid::corner_map(Dir a, Dir b) const
{
	int ab_sum = dir_flags_[static_cast<int>(a)]
	             + dir_flags_[static_cast<int>(b)];
	return get_corner_map_from_sum(ab_sum);
}

aoc24_18::Byte_grid::Map& aoc24_18::Byte_grid::fork_map(Dirs d) const
{
	throw_if_no_map();

	if (d == All_directions) {
		return *fk_URDL_;
	}

	throw std::invalid_argument("Invalid fork map directions");
}

aoc24_18::Byte_grid::Map& aoc24_18::Byte_grid::fork_map(Dir a,
                                                        Dir b,
                                                        Dir c) const
{
	int abc_sum = dir_flags_[static_cast<int>(a)]
	              + dir_flags_[static_cast<int>(b)]
	              + dir_flags_[static_cast<int>(c)];
	return get_fork_map_from_sum(abc_sum);
}

//------------------------------------------------------------------------------

void aoc24_18::Byte_grid::validate_start_and_end()
{
	if (state_ != Byte_grid_state::Corrupted_bytes_marked) {
		throw std::logic_error("Invalid state");
	}

	if (!valid_xy(static_cast<int>(start_.x), static_cast<int>(start_.y))) {
		throw std::logic_error("Start position is OOB");
	}

	if (!valid_xy(static_cast<int>(start_.x), static_cast<int>(start_.y))) {
		throw std::logic_error("End position is OOB");
	}

	size_t pos_start = start_.y * sz + start_.x;
	if (byte_map().test(pos_start)) {
		throw std::logic_error("Start position is a corrupted byte");
	}

	size_t pos_end = end_.y * sz + end_.x;
	if (byte_map().test(pos_end)) {
		throw std::logic_error("End position is a corrupted byte");
	}

	state_ = Byte_grid_state::Start_and_end_validated;
}

//------------------------------------------------------------------------------

aoc24_18::Byte_grid::Map& aoc24_18::Byte_grid::get_fork_map_from_sum(int sum) const
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

aoc24_18::Byte_grid::Map& aoc24_18::Byte_grid::get_corner_map_from_sum(
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

void aoc24_18::Byte_grid::internal_print_map(std::ostream& ostr,
                                             bool verbose) const
{
	size_t pos_start = start_.y * sz + start_.x;
	size_t pos_end = end_.y * sz + end_.x;
	for (size_t i = 0; i < (sz * sz_y); ++i) {
		char c = ' '; // Empty by default
		if (print_->test(i)) {
			c = chars::path_char;
		} else if (i == pos_start) {
			c = chars::start_char;
		} else if (i == pos_end) {
			c = chars::end_char;
		} else if (dead_end_map().test(i)) {
			c = chars::dead_end_char;
		} else if (byte_map().test(i)) {
			c = chars::byte_char;
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

void aoc24_18::Byte_grid::print_map(std::ostream& ostr) const
{
	print_->reset();
	internal_print_map(ostr, false);
}

void aoc24_18::Byte_grid::print_map_verbose(std::ostream& ostr) const
{
	print_->reset();
	internal_print_map(ostr, true);
}

void aoc24_18::Byte_grid::print_path(
    const std::vector<Point>& path, std::ostream& ostr) const
{
	throw_if_not_ready();
	if (path.empty()) {
		return;
	}
	merge_path_onto_print_map(path);
	internal_print_map(ostr, true);
}

void aoc24_18::Byte_grid::print_shortest_path(
    std::ostream& ostr) const
{
	throw_if_not_ready();
	print_->reset();
	*print_ |= shortest_path_map();
	internal_print_map(ostr, true);
}

void aoc24_18::Byte_grid::update_shortest_path(
    const std::vector<Point>& path)
{
	if (path.empty()) {
		return;
	}
	mark_shortest_path(path);
}

bool aoc24_18::Byte_grid::tile_is_on_shortest_path(
    int pos_x, int pos_y) const
{
	size_t pos_idx{xy_to_idx(pos_x, pos_y)};
	return shortest_path_map().test(pos_idx);
}

//------------------------------------------------------------------------------

int aoc24_18::Byte_grid::dir_to_index_based_offset(Dir dir) const
{
	static std::array<int, 4> offs{-static_cast<int>(sz),
	                               1,
	                               static_cast<int>(sz),
	                               -1};
	return offs[static_cast<int>(dir)];
}

size_t aoc24_18::Byte_grid::offset_pos(
    size_t pos, Dir dir) const
{
	if ((dir == Dir::Up && pos < sz)
	    || (dir == Dir::Right && (pos % sz) == (sz - 1))
	    || (dir == Dir::Down && pos >= (sz * (sz_y - 1)))
	    || (dir == Dir::Left && (pos % sz) == 0)) {
		return oob_pos;
	}
	return static_cast<size_t>(pos + dir_to_index_based_offset(dir));
}

size_t aoc24_18::Byte_grid::offset_pos_or_throw(
    size_t pos, Dir dir) const
{
	size_t offs{offset_pos(pos, dir)};
	if (offs == oob_pos) {
		throw std::runtime_error("Offset out of bounds");
	}
	return offs;
}

//------------------------------------------------------------------------------

void aoc24_18::Byte_grid::merge_path_onto_print_map(
    const std::vector<Point> path) const
{
	print_->reset();

	for (int i = 0; i < path.size() - 1; ++i) {
		const Vec& a = path[i].position();
		const Vec& b = (path.size() == 1) ? path[i].position()
		                                  : path[i + 1].position();
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

//------------------------------------------------------------------------------

std::array<bool, 4> aoc24_18::Byte_grid::tile_directions(int pos_x,
                                                         int pos_y,
                                                         bool skip_check) const
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
		return {false, true, false, false};
	}

	return {false, false, false, false};
}
