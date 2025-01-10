#include "box_grid.h"
#include <set>

namespace {
constexpr char empty_ch{'.'};
constexpr char wall_ch{'#'};
constexpr char robot_ch{'@'};
constexpr char box_ch{'O'};

const std::string& wall_key()
{
	static const std::string k{"Walls"};
	return k;
}
const std::string& movable_key()
{
	static const std::string k{"Movable_boxes"};
	return k;
}
const std::string& stuck_key()
{
	static const std::string k{"Stuck_boxes"};
	return k;
}

bool has_cached_l(const aoc24_15::Vec2d& pos,
                  const std::set<aoc24_15::Vec2d> cache)
{
	return (cache.find({pos.x - 1, pos.y}) != cache.end());
};

bool has_cached_u(const aoc24_15::Vec2d& pos,
                  const std::set<aoc24_15::Vec2d> cache)
{
	return (cache.find({pos.x, pos.y - 1}) != cache.end());
};

} // namespace
//------------------------------------------------------------------------------

bool aoc24_15::Vec2d::operator<(const Vec2d& rhs) const
{
	if (y != rhs.y) {
		return y < rhs.y;
	} else {
		return x < rhs.x;
	}
}

//------------------------------------------------------------------------------

aoc24_15::Vec2d aoc24_15::dir_to_offset(Direction dir, bool invert)
{
	if (invert) {
		dir = static_cast<Direction>((static_cast<int>(dir) + 2) % 4);
	}
	switch (dir) {
	case Direction::Up:
		return {0, -1};
	case Direction::Down:
		return {0, 1};
	case Direction::Right:
		return {1, 0};
	case Direction::Left:
		return {-1, 0};
		break;
	}
	throw std::invalid_argument("Cannot convert direction of value "
	                            + std::to_string(static_cast<int>(dir)));
	return {0, 0};
}

//------------------------------------------------------------------------------

aoc24_15::Box_grid::Box_grid(const aoc24::Char_grid& reference_grid,
                             std::unique_ptr<aoc24::Dyn_bitset> dyn_bitset)
    : Bit_grid(reference_grid.size(), std::move(dyn_bitset))
    , map_ptr_init_(false)
    , num_moves_(0)
    , box_dummy_(Box::dummy())

{
	init_maps();

	size_t pos = 0;
	int num_rob = 0;
	for (size_t row = 0; row < sz; ++row) {
		for (size_t col = 0; col < sz; ++col) {
			char c = reference_grid.char_at(col, row);
			switch (c) {
			case empty_ch:
				break;
			case wall_ch:
				walls_map().set(pos);
				break;
			case box_ch:
				add_box(static_cast<int>(col), static_cast<int>(row));
				break;
			case robot_ch:
				if (num_rob++ > 0) {
					throw std::invalid_argument(
					    "The reference grid contains more than one robot "
					    "character ('"
					    + std::string{c} + "')");
				}
				rob_.spawn(static_cast<int>(col), static_cast<int>(row));
				break;
			default:
				throw std::invalid_argument(
				    "The reference grid contains unknown character '"
				    + std::string{c} + "'.");
			}
			++pos;
		}
	}

	// Update adjacent
	std::set<Vec2d> cache_updated;
	for (Box& b : box_) {
		if (b.position().x > 0 && !has_cached_l(b.position(), cache_updated)) {
			update_adj(b, Direction::Left);
		}
		if (b.position().y > 0 && !has_cached_u(b.position(), cache_updated)) {
			update_adj(b, Direction::Up);
		}
		update_adj(b, Direction::Right);
		update_adj(b, Direction::Down);
		cache_updated.insert(b.position());
	}

	prune_boxes();
}

//------------------------------------------------------------------------------

void aoc24_15::Box_grid::init_maps()
{
	if (map_ptr_init_) {
		return;
	}
	walls_ = &add_map(wall_key());
	movable_ = &add_map(movable_key());
	stuck_ = &add_map(stuck_key());
	map_ptr_init_ = true;
}

aoc24::Dyn_bitset& aoc24_15::Box_grid::walls_map() const
{
	throw_if_no_map();
	return *walls_;
}

aoc24::Dyn_bitset& aoc24_15::Box_grid::movable_map() const
{
	throw_if_no_map();
	return *movable_;
}

aoc24::Dyn_bitset& aoc24_15::Box_grid::stuck_map() const
{
	throw_if_no_map();
	return *stuck_;
}

//------------------------------------------------------------------------------

void aoc24_15::Box_grid::add_box(int pos_x, int pos_y)
{
	size_t pos = pos_y * sz + pos_x;
	if (!valid_xy(pos_x, pos_y)) {
		throw std::invalid_argument(
		    "Cannot add box, its current position (" + std::to_string(pos_x)
		    + ", " + std::to_string(pos_y) + ") is out of bounds");
	}
	throw_if_occupied(pos);

	movable_map().set(pos);
	box_.emplace_back(Box(pos_x, pos_y));
}

aoc24_15::Box& aoc24_15::Box_grid::find_box(int pos_x,
                                            int pos_y,
                                            bool skip_check_position)
{
	if (!skip_check_position && !valid_xy(pos_x, pos_y)) {
		throw std::invalid_argument(
		    "Cannot search for box, the given position (" + std::to_string(pos_x)
		    + ", " + std::to_string(pos_y) + ") is out of bounds");
	}
	for (auto& b : box_) {
		if (b.position().x == pos_x && b.position().y == pos_y) {
			return b;
		}
	}
	return box_dummy_;
}

aoc24_15::Adjacent_tile& aoc24_15::Box_grid::adj_from_dir(Box& box,
                                                          Direction dir,
                                                          bool invert)
{
	if (invert) {
		dir = static_cast<Direction>((static_cast<int>(dir) + 2) % 4);
	}

	Adjacent_tile* t = nullptr;
	switch (dir) {
	case Direction::Up:
		t = &(box.adj_U);
		break;
	case Direction::Right:
		t = &(box.adj_R);
		break;
	case Direction::Down:
		t = &(box.adj_D);
		break;
	case Direction::Left:
		t = &(box.adj_L);
		break;
	default:
		throw std::invalid_argument(
		    "Cannot convert Direction to adjacent: Unknown Directional value");
	}
	return *t;
}

void aoc24_15::Box_grid::update_adj(Box& box, Direction dir)
{
	Vec2d offs = dir_to_offset(dir);
	Vec2d adj_xy = {box.pos_.x + offs.x, box.pos_.y + offs.y};
	if (is_oob(adj_xy.x, adj_xy.y)) {
		return;
	}

	Adjacent_tile& tile = adj_from_dir(box, dir);
	size_t adj_pos = adj_xy.y * size() + adj_xy.x;
	if (movable_map().test(adj_pos)) {
		Box& adj_box = find_box(adj_xy.x, adj_xy.y, true);
		if (adj_box.is_dummy()) {
			throw std::logic_error(
			    "Cannot link boxes: No box found with coordinates ("
			    + std::to_string(adj_xy.x) + ", " + std::to_string(adj_xy.y)
			    + ")");
		}
		link_adj_boxes(box, adj_box, dir);
	} else if (stuck_map().test(adj_pos)) {
		tile.type = Adjacent_tile::Type::Box_stuck;
		tile.box = nullptr;
	} else if (walls_map().test(adj_pos)) {
		tile.type = Adjacent_tile::Type::Wall;
		tile.box = nullptr;
	} else {
		tile.type = Adjacent_tile::Type::Empty;
		tile.box = nullptr;
	}
}

void aoc24_15::Box_grid::link_adj_boxes(Box& box_a,
                                        Box& box_b,
                                        Direction from_a_to_b)
{
	auto& adj_tile_a = adj_from_dir(box_a, from_a_to_b);
	auto& adj_tile_b = adj_from_dir(box_b, from_a_to_b, true);
	adj_tile_a.box = &box_b;
	adj_tile_b.box = &box_a;
	adj_tile_a.type = Adjacent_tile::Type::Box_movable;
	adj_tile_b.type = Adjacent_tile::Type::Box_movable;
}

bool aoc24_15::Box_grid::adj_is_on_stuck_map(const Box& b, Direction dir) const
{
	Vec2d offs = dir_to_offset(dir);
	Vec2d adj_xy = {b.pos_.x + offs.x, b.pos_.y + offs.y};
	if (is_oob(adj_xy.x, adj_xy.y)) {
		return false;
	}
	return stuck_map().test(adj_xy.y * size() + adj_xy.x);
}

aoc24_15::Is_stuck aoc24_15::Box_grid::is_stuck(const Box& b) const
{
	using AT = Adjacent_tile::Type;
	Is_stuck s;
	s.up = (b.adj_U.type == AT::Wall || b.adj_U.type == AT::Box_stuck
	        || adj_is_on_stuck_map(b, Direction::Up));
	s.right = (b.adj_R.type == AT::Wall || b.adj_R.type == AT::Box_stuck
	           || adj_is_on_stuck_map(b, Direction::Right));
	s.down = (b.adj_D.type == AT::Wall || b.adj_D.type == AT::Box_stuck
	          || adj_is_on_stuck_map(b, Direction::Down));
	s.left = (b.adj_L.type == AT::Wall || b.adj_L.type == AT::Box_stuck
	          || adj_is_on_stuck_map(b, Direction::Left));
	return s;
}

void aoc24_15::Box_grid::move_robot(Direction dir)
{
	++num_moves_;

	Vec2d offs = dir_to_offset(dir);
	Vec2d start_xy = rob_.position();
	Vec2d adj_xy = {start_xy.x + offs.x, start_xy.y + offs.y};
	if (is_oob(adj_xy.x, adj_xy.y)) {
		return;
	}
	size_t adj_pos = adj_xy.y * size() + adj_xy.x;

	if (walls_map().test(adj_pos) || stuck_map().test(adj_pos)) {
		return;
	} else if (movable_map().test(adj_pos)) {
		Direction dir_l = dir_turn_left(dir);
		Direction dir_r = dir_turn_right(dir);

		auto box_ptr_v = movable_boxes_in_dir(start_xy.x, start_xy.y, dir);
		bool move_boxes = box_ptr_v.size() > 0;
		for (size_t i = 0; i < box_ptr_v.size(); ++i) {
			// Move boxes
			Box& b = *box_ptr_v[i];
			b.pos_.x += offs.x;
			b.pos_.y += offs.y;
			update_adj(b, dir_l);
			update_adj(b, dir_r);
			if (i == box_ptr_v.size() - 1) {
				// Last iter.
				update_adj(b, dir);
			}
		}

		if (move_boxes) {
			// Clear link of boxes adj. to the now empty tile
			Vec2d offs_l = dir_to_offset(dir_l);
			Vec2d offs_r = dir_to_offset(dir_r);
			auto& start_box_l = find_box(adj_xy.x + offs_l.x,
			                             adj_xy.y + offs_l.y);
			if (!start_box_l.is_dummy()) {
				auto& adj_to_clear = adj_from_dir(start_box_l, dir_r);
				adj_to_clear.type = Adjacent_tile::Type::Empty;
				adj_to_clear.box = nullptr;
			}
			auto& start_box_r = find_box(adj_xy.x + offs_r.x,
			                             adj_xy.y + offs_r.y);
			if (!start_box_r.is_dummy()) {
				auto& adj_to_clear = adj_from_dir(start_box_r, dir_l);
				adj_to_clear.type = Adjacent_tile::Type::Empty;
				adj_to_clear.box = nullptr;
			}

			rob_.move(dir);

			// Update maps
			movable_map().reset(adj_pos);
			int num_boxes = static_cast<int>(box_ptr_v.size());
			Vec2d end_xy = {adj_xy.x + offs.x * num_boxes,
			                adj_xy.y + offs.y * num_boxes};
			movable_map().set(end_xy.y * size() + end_xy.x);

			// Prune if the box at the end may have gotten stuck
			const auto& adj_end = adj_from_dir(*box_ptr_v.back(), dir);
			if (adj_end.type == Adjacent_tile::Type::Box_stuck
			    || adj_end.type == Adjacent_tile::Type::Wall) {
				prune_boxes();
			}
		}
	} else {
		rob_.move(dir);
	}
}

void aoc24_15::Box_grid::print_map(std::ostream& ostr)
{
	size_t i = 0;
	for (size_t y = 0; y < sz; ++y) {
		for (size_t x = 0; x < sz; ++x) {
			bool has_rob = rob_.position().x == x && rob_.position().y == y;
			bool has_wal = walls_map().test(i);
			bool has_mov = movable_map().test(i);
			bool has_stk = stuck_map().test(i);
			char c{};
			if (has_rob + has_wal + has_mov + has_stk > 1) {
				c = '!';
			} else {
				if (has_rob) {
					c = '@';
				} else if (has_wal) {
					c = '#';
				} else if (has_mov) {
					c = 'O';
				} else if (has_stk) {
					c = 'X';
				} else {
					c = ' ';
				}
			}
			ostr << (x == 0 ? "" : " ") << c;
			++i;
		}
		ostr << '\n';
	}
}

bool aoc24_15::Box_grid::boxes_match_maps()
{
	for (auto& b : box_) {
		for (int i = 0; i < 4; ++i) {
			Vec2d offs = dir_to_offset(static_cast<Direction>(i));
			Vec2d adj_xy = {b.position().x + offs.x, b.position().y + offs.y};
			size_t adj_pos = adj_xy.y * size() + adj_xy.x;
			const auto adj_t = adj_from_dir(b, static_cast<Direction>(i));
			if (adj_t.type == Adjacent_tile::Type::Empty) {
				if (!walls_map().test(adj_pos) && !movable_map().test(adj_pos)
				    && !stuck_map().test(adj_pos)) {
					continue;
				}
				std::cout << "Expected empty at position (" << adj_xy.x << ", "
				          << adj_xy.y << "). Dir " << static_cast<Direction>(i)
				          << '\n';
			} else if (adj_t.type == Adjacent_tile::Type::Wall) {
				if (walls_map().test(adj_pos) && !movable_map().test(adj_pos)
				    && !stuck_map().test(adj_pos)) {
					continue;
				}
				std::cout << "Expected wall at position (" << adj_xy.x << ", "
				          << adj_xy.y << "). Dir " << static_cast<Direction>(i)
				          << '\n';
			} else if (adj_t.type == Adjacent_tile::Type::Box_movable) {
				if (adj_t.box->position().x != adj_xy.x
				    || adj_t.box->position().y != adj_xy.y) {
					std::cout << "Expected box with position (" << adj_xy.x
					          << ", " << adj_xy.y << ") but got position ("
					          << adj_t.box->position().x << ", "
					          << adj_t.box->position().y << "). Dir "
					          << static_cast<Direction>(i) << '\n';
					return false;
				}
				if (!walls_map().test(adj_pos) && movable_map().test(adj_pos)
				    && !stuck_map().test(adj_pos)) {
					continue;
				}
				std::cout << "Expected box at position (" << adj_xy.x << ", "
				          << adj_xy.y << "). Dir " << static_cast<Direction>(i)
				          << '\n';
			} else if (adj_t.type == Adjacent_tile::Type::Box_stuck) {
				if (!walls_map().test(adj_pos) && !movable_map().test(adj_pos)
				    && stuck_map().test(adj_pos)) {
					continue;
				};
				std::cout << "Expected STUCK box at position (" << adj_xy.x
				          << ", " << adj_xy.y << "). Dir "
				          << static_cast<Direction>(i) << '\n';
			}
			return false;
		}
	}
	return true;
}

long long aoc24_15::Box_grid::sum_of_all_box_coordinates() const
{
	constexpr int top_dist_f{100};

	// Use print map to temporarily combine box maps
	print_->reset();
	(*print_) |= movable_map();
	(*print_) |= stuck_map();

	long long sum = 0;
	size_t pos = 0;
	for (int row = 0; row < static_cast<int>(size()); ++row) {
		for (size_t col = 0; col < static_cast<int>(size()); ++col) {
			if (print_->test(pos)) {
				sum += top_dist_f * row + col;
			}
			++pos;
		}
	}
	return sum;
}

std::vector<aoc24_15::Box*> aoc24_15::Box_grid::movable_boxes_in_dir(
    int pos_x, int pos_y, Direction dir)
{
	Vec2d offs = dir_to_offset(dir);
	Vec2d adj_pos = {pos_x + offs.x, pos_y + offs.y};
	if (is_oob(adj_pos.x, adj_pos.y)) {
		return {};
	}

	std::vector<Box*> box_ptr;
	Box& start_b = find_box(adj_pos.x, adj_pos.y, true);
	if (start_b.is_dummy()) {
		throw std::logic_error(
		    "Cannot scan for movable boxes: No box found with coordinates ("
		    + std::to_string(adj_pos.x) + ", " + std::to_string(adj_pos.y)
		    + ")");
	}
	box_ptr.push_back(&start_b);

	auto found_adj = adj_from_dir(start_b, dir); // Copy
	while (found_adj.type == Adjacent_tile::Type::Box_movable) {
		box_ptr.push_back(found_adj.box);
		found_adj = adj_from_dir(*found_adj.box, dir);
	}

	if (found_adj.type != Adjacent_tile::Type::Empty) {
		return {};
	}

	return box_ptr;
}

void aoc24_15::Box_grid::prune_boxes()
{
	bool keep_looking = true;
	while (keep_looking) {
		keep_looking = false; // Only keep looking if new matches are found
		for (auto it = box_.begin(); it != box_.end(); ++it) {
			Box& b = *it;
			const auto& b_stuck = is_stuck(b);
			if (b_stuck.has_corner_stuck()) {
				keep_looking = true;
				mark_as_stuck(b);
			}
		}

		if (keep_looking) {
			// Prune
			for (auto it = box_.begin(); it != box_.end();) {
				if ((*it).marked_for_pruning_) {
					it = box_.erase(it);
				} else {
					++it;
				}
			}
		}
	}
}

void aoc24_15::Box_grid::mark_as_stuck(Box& box)
{
	using AT = Adjacent_tile::Type;
	box.marked_for_pruning_ = true;
	stuck_map().set(box.pos_.y * size() + box.pos_.x);
	movable_map().reset(box.pos_.y * size() + box.pos_.x);
	for (int i = 0; i < 4; ++i) {
		Direction adj_dir = static_cast<Direction>(i);
		const auto& adj_t_a = adj_from_dir(box, adj_dir);
		if (adj_t_a.type == AT::Box_movable) {
			auto& adj_t_b = adj_from_dir(*adj_t_a.box, adj_dir, true);
			adj_t_b.type = AT::Box_stuck;
			adj_t_b.box = nullptr;
		}
	}
}

//------------------------------------------------------------------------------

aoc24::XY aoc24_15::Box_grid::to_xy(int x, int y, const bool skip_check)
{
	if (!skip_check && is_oob(x, y)) {
		throw std::invalid_argument("Cannot convert x=" + std::to_string(x)
		                            + " and y=" + std::to_string(y)
		                            + " to a valid XY of map with size "
		                            + std::to_string(sz));
	}
	return {static_cast<size_t>(x), static_cast<size_t>(y)};
}

aoc24_15::Direction aoc24_15::Box_grid::dir_turn_left(const Direction dir) const
{
	return static_cast<Direction>((static_cast<int>(dir) + 3) % 4);
}

aoc24_15::Direction aoc24_15::Box_grid::dir_turn_right(const Direction dir) const
{
	return static_cast<Direction>((static_cast<int>(dir) + 1) % 4);
}

void aoc24_15::Box_grid::throw_if_no_map() const
{
	if (!map_ptr_init_) {
		throw std::runtime_error(
		    "Cannot access map: Maps have not been initialized");
	}
}

void aoc24_15::Box_grid::throw_if_occupied(size_t pos) const
{
	if (walls_map().test(pos) || movable_map().test(pos)
	    || stuck_map().test(pos)) {
		throw std::logic_error("Cannot add box: tile is already occupied");
	}
}

void aoc24_15::Robot::spawn(int pos_x, int pos_y)
{
	init_pos_ = {pos_x, pos_y};
	pos_ = init_pos_;
	has_spawned_ = true;
}

void aoc24_15::Robot::move(Direction dir)
{
	Vec2d offs = dir_to_offset(dir);
	pos_.x += offs.x;
	pos_.y += offs.y;
}
