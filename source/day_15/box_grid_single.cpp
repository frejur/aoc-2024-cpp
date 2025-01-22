#include "box_grid_single.h"
#include "keys.h"

namespace {

} // namespace
//------------------------------------------------------------------------------

aoc24_15::Box_grid_single::Box_grid_single(
    const aoc24::Char_grid& reference_grid,
    std::unique_ptr<aoc24::Dyn_bitset> dyn_bitset)
    : Box_grid(std::move(dyn_bitset))
{
	Box_grid_single::init_maps();

	size_t pos = 0;
	int num_rob = 0;
	for (size_t row = 0; row < sz_y; ++row) {
		for (size_t col = 0; col < sz; ++col) {
			char c = reference_grid.char_at(col, row);
			switch (c) {
			case chars::empty_ch:
				break;
			case chars::wall_ch:
				walls_map().set(pos);
				break;
			case chars::box_ch:
				Box_grid_single::add_box(static_cast<int>(col),
				                         static_cast<int>(row));
				break;
			case chars::robot_ch:
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
	std::set<aoc24::Vec2d, aoc24::Vec2d_comparator> cache_updated;
	for (Box& b : box_) {
		if (b.position().x > 0 && !has_cached_l(b.position(), cache_updated)) {
			Box_grid_single::update_adj(b, aoc24::Direction::Left);
		}
		if (b.position().y > 0 && !has_cached_u(b.position(), cache_updated)) {
			Box_grid_single::update_adj(b, aoc24::Direction::Up);
		}
		Box_grid_single::update_adj(b, aoc24::Direction::Right);
		Box_grid_single::update_adj(b, aoc24::Direction::Down);
		cache_updated.insert(b.position());
	}

	Box_grid_single::prune_boxes();
}

void aoc24_15::Box_grid_single::move_robot(aoc24::Direction dir)
{
	++num_moves_;

	aoc24::Vec2d offs = dir_to_offset(dir);
	aoc24::Vec2d start_xy = rob_.position();
	aoc24::Vec2d adj_xy = {start_xy.x + offs.x, start_xy.y + offs.y};
	if (is_oob(adj_xy.x, adj_xy.y)) {
		return;
	}
	size_t adj_pos = adj_xy.y * size() + adj_xy.x;

	if (walls_map().test(adj_pos) || stuck_map().test(adj_pos)) {
		return;
	} else if (movable_map().test(adj_pos)) {
		aoc24::Direction dir_l = dir_turn_left(dir);
		aoc24::Direction dir_r = dir_turn_right(dir);

		auto box_ptr_v = movable_boxes_in_dir(start_xy.x, start_xy.y, dir);
		bool move_boxes = box_ptr_v.size() > 0;
		for (size_t i = 0; i < box_ptr_v.size(); ++i) {
			// Move boxes
			Box_single& b = *(static_cast<Box_single*>(box_ptr_v[i]));
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
			aoc24::Vec2d offs_l = dir_to_offset(dir_l);
			aoc24::Vec2d offs_r = dir_to_offset(dir_r);
			auto& start_box_l = find_box(adj_xy.x + offs_l.x,
			                             adj_xy.y + offs_l.y);
			if (!start_box_l.is_dummy()) {
				auto& adj_to_clear = static_cast<Adjacent_tile_single&>(
				    adj_from_dir(start_box_l, dir_r));
				adj_to_clear.type = Adjacent_tile_single::Type::Empty;
				adj_to_clear.box = nullptr;
			}
			auto& start_box_r = find_box(adj_xy.x + offs_r.x,
			                             adj_xy.y + offs_r.y);
			if (!start_box_r.is_dummy()) {
				auto& adj_to_clear = static_cast<Adjacent_tile_single&>(
				    adj_from_dir(start_box_r, dir_l));
				adj_to_clear.type = Adjacent_tile_single::Type::Empty;
				adj_to_clear.box = nullptr;
			}

			rob_.move(dir);

			// Update maps
			movable_map().reset(adj_pos);
			int num_boxes = static_cast<int>(box_ptr_v.size());
			aoc24::Vec2d end_xy = {adj_xy.x + offs.x * num_boxes,
			                       adj_xy.y + offs.y * num_boxes};
			movable_map().set(end_xy.y * size() + end_xy.x);

			// Prune if the box at the end may have gotten stuck
			const auto& adj_end = static_cast<Adjacent_tile_single&>(
			    adj_from_dir(*box_ptr_v.back(), dir));
			if (adj_end.type == Adjacent_tile_single::Type::Box_stuck
			    || adj_end.type == Adjacent_tile_single::Type::Wall) {
				prune_boxes();
			}
		}
	} else {
		rob_.move(dir);
	}
}

//------------------------------------------------------------------------------

void aoc24_15::Box_grid_single::init_maps()
{
	if (map_ptr_init_) {
		return;
	}
	walls_ = &add_map(keys::wall_key);
	movable_ = &add_map(keys::movable_key);
	stuck_ = &add_map(keys::stuck_key);
	map_ptr_init_ = true;
}

aoc24::Dyn_bitset& aoc24_15::Box_grid_single::walls_map() const
{
	throw_if_no_map();
	return *walls_;
}

aoc24::Dyn_bitset& aoc24_15::Box_grid_single::movable_map() const
{
	throw_if_no_map();
	return *movable_;
}

aoc24::Dyn_bitset& aoc24_15::Box_grid_single::stuck_map() const
{
	throw_if_no_map();
	return *stuck_;
}

//------------------------------------------------------------------------------

void aoc24_15::Box_grid_single::print_map(std::ostream& ostr)
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

//------------------------------------------------------------------------------

bool aoc24_15::Box_grid_single::boxes_match_maps()
{
	for (auto& b : box_) {
		for (int i = 0; i < 4; ++i) {
			aoc24::Vec2d offs = dir_to_offset(static_cast<aoc24::Direction>(i));
			aoc24::Vec2d adj_xy = {b.position().x + offs.x,
			                       b.position().y + offs.y};
			size_t adj_pos = adj_xy.y * size() + adj_xy.x;
			const auto adj_t = static_cast<Adjacent_tile_single&>(
			    adj_from_dir(b, static_cast<aoc24::Direction>(i)));
			if (adj_t.type == Adjacent_tile_single::Type::Empty) {
				if (!walls_map().test(adj_pos) && !movable_map().test(adj_pos)
				    && !stuck_map().test(adj_pos)) {
					continue;
				}
				std::cout << "Expected empty at position (" << adj_xy.x << ", "
				          << adj_xy.y << "). Dir "
				          << static_cast<aoc24::Direction>(i) << '\n';
			} else if (adj_t.type == Adjacent_tile_single::Type::Wall) {
				if (walls_map().test(adj_pos) && !movable_map().test(adj_pos)
				    && !stuck_map().test(adj_pos)) {
					continue;
				}
				std::cout << "Expected wall at position (" << adj_xy.x << ", "
				          << adj_xy.y << "). Dir "
				          << static_cast<aoc24::Direction>(i) << '\n';
			} else if (adj_t.type == Adjacent_tile_single::Type::Box_movable) {
				if (adj_t.box->position().x != adj_xy.x
				    || adj_t.box->position().y != adj_xy.y) {
					std::cout << "Expected box with position (" << adj_xy.x
					          << ", " << adj_xy.y << ") but got position ("
					          << adj_t.box->position().x << ", "
					          << adj_t.box->position().y << "). Dir "
					          << static_cast<aoc24::Direction>(i) << '\n';
					return false;
				}
				if (!walls_map().test(adj_pos) && movable_map().test(adj_pos)
				    && !stuck_map().test(adj_pos)) {
					continue;
				}
				std::cout << "Expected box at position (" << adj_xy.x << ", "
				          << adj_xy.y << "). Dir "
				          << static_cast<aoc24::Direction>(i) << '\n';
			} else if (adj_t.type == Adjacent_tile_single::Type::Box_stuck) {
				if (!walls_map().test(adj_pos) && !movable_map().test(adj_pos)
				    && stuck_map().test(adj_pos)) {
					continue;
				};
				std::cout << "Expected STUCK box at position (" << adj_xy.x
				          << ", " << adj_xy.y << "). Dir "
				          << static_cast<aoc24::Direction>(i) << '\n';
			}
			return false;
		}
	}
	return true;
}

//------------------------------------------------------------------------------

long long aoc24_15::Box_grid_single::sum_of_all_box_coordinates() const
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

//------------------------------------------------------------------------------

void aoc24_15::Box_grid_single::add_box(int pos_x, int pos_y)
{
	size_t pos = pos_y * sz + pos_x;
	if (!valid_xy(pos_x, pos_y)) {
		throw std::invalid_argument(
		    "Cannot add box, its current position (" + std::to_string(pos_x)
		    + ", " + std::to_string(pos_y) + ") is out of bounds");
	}
	Box_grid_single::throw_if_occupied(pos);

	movable_map().set(pos);
	box_.emplace_back(Box_single(pos_x, pos_y));
}

aoc24_15::Box& aoc24_15::Box_grid_single::find_box(int pos_x,
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

//------------------------------------------------------------------------------

aoc24_15::Adjacent_tile& aoc24_15::Box_grid_single::adj_from_dir(
    Box& box, aoc24::Direction dir, bool invert)
{
	if (invert) {
		dir = static_cast<aoc24::Direction>((static_cast<int>(dir) + 2) % 4);
	}

	auto& box_s = static_cast<Box_single&>(box);
	Adjacent_tile_single* t = nullptr;
	switch (dir) {
	case aoc24::Direction::Up:
		t = &(box_s.adj_U);
		break;
	case aoc24::Direction::Right:
		t = &(box_s.adj_R);
		break;
	case aoc24::Direction::Down:
		t = &(box_s.adj_D);
		break;
	case aoc24::Direction::Left:
		t = &(box_s.adj_L);
		break;
	default:
		throw std::invalid_argument(
		    "Cannot convert aoc24::Direction to adjacent: Unknown "
		    "aoc24::Directional value");
	}
	return *t;
}

void aoc24_15::Box_grid_single::update_adj(Box& box, aoc24::Direction dir)
{
	aoc24::Vec2d offs = dir_to_offset(dir);
	auto& box_s = static_cast<Box_single&>(box);
	aoc24::Vec2d adj_xy = {box_s.pos_.x + offs.x, box_s.pos_.y + offs.y};
	if (is_oob(adj_xy.x, adj_xy.y)) {
		return;
	}

	auto& tile = static_cast<Adjacent_tile_single&>(adj_from_dir(box, dir));
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
		tile.type = Adjacent_tile_single::Type::Box_stuck;
		tile.box = nullptr;
	} else if (walls_map().test(adj_pos)) {
		tile.type = Adjacent_tile_single::Type::Wall;
		tile.box = nullptr;
	} else {
		tile.type = Adjacent_tile_single::Type::Empty;
		tile.box = nullptr;
	}
}

void aoc24_15::Box_grid_single::link_adj_boxes(Box& box_a,
                                               Box& box_b,
                                               aoc24::Direction from_a_to_b)
{
	auto& adj_tile_a = static_cast<Adjacent_tile_single&>(
	    adj_from_dir(box_a, from_a_to_b));
	auto& adj_tile_b = static_cast<Adjacent_tile_single&>(
	    adj_from_dir(box_b, from_a_to_b, true));
	adj_tile_a.box = &box_b;
	adj_tile_b.box = &box_a;
	adj_tile_a.type = Adjacent_tile_single::Type::Box_movable;
	adj_tile_b.type = Adjacent_tile_single::Type::Box_movable;
}

//------------------------------------------------------------------------------

void aoc24_15::Box_grid_single::prune_boxes()
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

void aoc24_15::Box_grid_single::mark_as_stuck(Box& b)
{
	using AT = Adjacent_tile_single::Type;
	auto& box_s = static_cast<Box_single&>(b);
	box_s.marked_for_pruning_ = true;
	stuck_map().set(box_s.pos_.y * size() + box_s.pos_.x);
	movable_map().reset(box_s.pos_.y * size() + box_s.pos_.x);
	for (int i = 0; i < 4; ++i) {
		aoc24::Direction adj_dir = static_cast<aoc24::Direction>(i);
		const auto& adj_t_a = static_cast<Adjacent_tile_single&>(
		    adj_from_dir(box_s, adj_dir));
		if (adj_t_a.type == AT::Box_movable) {
			auto& adj_t_b = static_cast<Adjacent_tile_single&>(
			    adj_from_dir(*adj_t_a.box, adj_dir, true));
			adj_t_b.type = AT::Box_stuck;
			adj_t_b.box = nullptr;
		}
	}
}

bool aoc24_15::Box_grid_single::adj_is_on_stuck_map(const Box& b,
                                                    aoc24::Direction dir) const
{
	aoc24::Vec2d offs = dir_to_offset(dir);
	auto& box_s = static_cast<const Box_single&>(b);
	aoc24::Vec2d adj_xy = {box_s.pos_.x + offs.x, box_s.pos_.y + offs.y};
	if (is_oob(adj_xy.x, adj_xy.y)) {
		return false;
	}
	return stuck_map().test(adj_xy.y * size() + adj_xy.x);
}

aoc24_15::Is_stuck aoc24_15::Box_grid_single::is_stuck(const Box& b) const
{
	using AT = Adjacent_tile_single::Type;
	auto& box_s = static_cast<const Box_single&>(b);
	Is_stuck s;
	s.up = (box_s.adj_U.type == AT::Wall || box_s.adj_U.type == AT::Box_stuck
	        || adj_is_on_stuck_map(box_s, aoc24::Direction::Up));
	s.right = (box_s.adj_R.type == AT::Wall || box_s.adj_R.type == AT::Box_stuck
	           || adj_is_on_stuck_map(box_s, aoc24::Direction::Right));
	s.down = (box_s.adj_D.type == AT::Wall || box_s.adj_D.type == AT::Box_stuck
	          || adj_is_on_stuck_map(box_s, aoc24::Direction::Down));
	s.left = (box_s.adj_L.type == AT::Wall || box_s.adj_L.type == AT::Box_stuck
	          || adj_is_on_stuck_map(box_s, aoc24::Direction::Left));
	return s;
}

//------------------------------------------------------------------------------

std::vector<aoc24_15::Box*> aoc24_15::Box_grid_single::movable_boxes_in_dir(
    int pos_x, int pos_y, aoc24::Direction dir)
{
	aoc24::Vec2d offs = dir_to_offset(dir);
	aoc24::Vec2d adj_pos = {pos_x + offs.x, pos_y + offs.y};
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

	auto found_adj = static_cast<Adjacent_tile_single&>(
	    adj_from_dir(start_b, dir)); // Copy
	while (found_adj.type == Adjacent_tile_single::Type::Box_movable) {
		box_ptr.push_back(found_adj.box);
		found_adj = static_cast<Adjacent_tile_single&>(
		    adj_from_dir(*found_adj.box, dir));
	}

	if (found_adj.type != Adjacent_tile_single::Type::Empty) {
		return {};
	}

	return box_ptr;
}

//------------------------------------------------------------------------------

void aoc24_15::Box_grid_single::throw_if_occupied(size_t pos) const
{
	if (walls_map().test(pos) || movable_map().test(pos)
	    || stuck_map().test(pos)) {
		throw std::logic_error("Cannot add box: tile is already occupied");
	}
}
