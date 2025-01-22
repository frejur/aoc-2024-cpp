#include "box_grid_wide.h"
#include "keys.h"
#include <stack>

namespace {
size_t width_from_size(size_t sz)
{
	if (sz < 3) {
		throw std::invalid_argument(
		    "Invalid dynamic bitset passed to Box_grid_wide");
	}
	return static_cast<size_t>(sqrt(sz / 2));
}

bool has_cached_pos(const aoc24::Vec2d& pos,
                    const std::set<aoc24::Vec2d, aoc24::Vec2d_comparator> cache)
{
	return (cache.find({pos}) != cache.end());
};
} // namespace

//------------------------------------------------------------------------------

aoc24_15::Box_grid_wide::Box_grid_wide(
    const aoc24::Char_grid& reference_grid,
    std::unique_ptr<aoc24::Dyn_bitset> dyn_bitset)
    : Box_grid(width_from_size(dyn_bitset->size()) * 2,
               width_from_size(dyn_bitset->size()),
               std::move(dyn_bitset))
{
	Box_grid_wide::init_maps();

	size_t pos = 0;
	int num_rob = 0;
	size_t char_grid_w = sz_y;
	for (size_t row = 0; row < sz_y; ++row) {
		for (size_t col = 0; col < char_grid_w; ++col) {
			char c = reference_grid.char_at(col, row);
			switch (c) {
			case chars::empty_ch:
				break;
			case chars::wall_ch:
				walls_map().set(pos);
				walls_map().set(pos + 1);
				break;
			case chars::box_ch:
				Box_grid_wide::add_box(static_cast<int>(col * 2),
				                       static_cast<int>(row));
				break;
			case chars::robot_ch:
				if (num_rob++ > 0) {
					throw std::invalid_argument(
					    "The reference grid contains more than one robot "
					    "character ('"
					    + std::string{c} + "')");
				}
				rob_.spawn(static_cast<int>(col * 2), static_cast<int>(row));
				break;
			default:
				throw std::invalid_argument(
				    "The reference grid contains unknown character '"
				    + std::string{c} + "'.");
			}
			pos += 2; // Everything is twice as wide...
		}
	}

	// Update adjacent
	std::set<aoc24::Vec2d, aoc24::Vec2d_comparator> cache_updated;
	for (Box& b : box_) {
		if (b.position().x > 0 && !has_cached_l(b.position(), cache_updated)) {
			Box_grid_wide::update_adj(b, aoc24::Direction::Left);
		}
		if (b.position().y > 0 && !has_cached_u(b.position(), cache_updated)) {
			Box_grid_wide::update_adj(b, aoc24::Direction::Up);
		}
		Box_grid_wide::update_adj(b, aoc24::Direction::Right);
		Box_grid_wide::update_adj(b, aoc24::Direction::Down);
		cache_updated.insert(b.position());
	}

	Box_grid_wide::prune_boxes();
}

//------------------------------------------------------------------------------

void aoc24_15::Box_grid_wide::move_robot(aoc24::Direction dir)
{
	using ATT = Adjacent_subtile::Type;
	++num_moves_;

	aoc24::Vec2d offs = dir_to_offset(dir);
	aoc24::Vec2d start_xy = rob_.position();
	aoc24::Vec2d adj_xy = {start_xy.x + offs.x, start_xy.y + offs.y};
	if (is_oob(adj_xy.x, adj_xy.y)) {
		return;
	}
	size_t adj_pos = adj_xy.y * sz + adj_xy.x;

	if (walls_map().test(adj_pos)
	    || stuck_map(aoc24::Direction::Left).test(adj_pos)
	    || stuck_map(aoc24::Direction::Right).test(adj_pos)) {
		return;
	} else if (movable_map(aoc24::Direction::Left).test(adj_pos)
	           || movable_map(aoc24::Direction::Right).test(adj_pos)) {
		aoc24::Direction dir_l = dir_turn_left(dir);
		aoc24::Direction dir_r = dir_turn_right(dir);

		auto box_ptr_v = movable_boxes_in_dir(start_xy.x, start_xy.y, dir);
		std::vector<Box*> stationary_adj_L;
		std::vector<Box*> stationary_adj_R;
		bool move_boxes = box_ptr_v.size() > 0;
		for (size_t i = 0; i < box_ptr_v.size(); ++i) {
			Box_wide& b = *(static_cast<Box_wide*>(box_ptr_v[i]));

			// Keep track of stationary adjacent boxes (That don't move)
			if (dir_is_UD(dir)) {
				if (b.adj_L.left_tile->type == ATT::Box_movable_right) {
					bool not_moving_L = true;
					for (const auto* b_move_L : box_ptr_v) {
						if (b.adj_L.left_tile->box == b_move_L) {
							not_moving_L = false;
							break;
						}
					}
					if (not_moving_L) {
						stationary_adj_L.push_back(b.adj_L.left_tile->box);
					}
				}
				if (b.adj_R.right_tile->type == ATT::Box_movable_left) {
					bool not_moving_R = true;
					for (const auto* b_move_R : box_ptr_v) {
						if (b.adj_R.right_tile->box == b_move_R) {
							not_moving_R = false;
							break;
						}
					}
					if (not_moving_R) {
						stationary_adj_R.push_back(b.adj_R.right_tile->box);
					}
				}
			}

			// Move boxes
			b.pos_.x += offs.x;
			b.pos_.y += offs.y;
		}

		if (move_boxes) {
			// Clear link of boxes adj. to the now empty start tile(s)
			aoc24::Vec2d offs_L = dir_to_offset(dir_l);
			aoc24::Vec2d offs_R = dir_to_offset(dir_r);
			if (dir_is_UD(dir)) {
				offs_L.x *= 2;
				offs_R.x *= 2;
			}

			aoc24::Vec2d xy_L = {adj_xy.x + offs_L.x, adj_xy.y + offs_L.y};
			size_t pos_L = static_cast<size_t>(xy_L.y * sz + xy_L.x);
			if (movable_map(aoc24::Direction::Right).test(pos_L)) {
				--xy_L.x;
			}

			auto& start_box_L = find_box(xy_L.x, xy_L.y);
			if (!start_box_L.is_dummy()) {
				auto& adj_to_clear = static_cast<Adjacent_tile_wide&>(
				    adj_from_dir(start_box_L, dir_r));
				if (!dir_is_UD(dir)) {
					adj_to_clear.left_tile->type = ATT::Empty;
					adj_to_clear.left_tile->box = nullptr;
					adj_to_clear.right_tile->type = ATT::Empty;
					adj_to_clear.right_tile->box = nullptr;
				} else if (offs_L.x < 0) {
					adj_to_clear.right_tile->type = ATT::Empty;
					adj_to_clear.right_tile->box = nullptr;
				} else {
					adj_to_clear.left_tile->type = ATT::Empty;
					adj_to_clear.left_tile->box = nullptr;
				}
			}
			aoc24::Vec2d xy_R = {adj_xy.x + offs_R.x, adj_xy.y + offs_R.y};
			size_t pos_R = static_cast<size_t>(xy_R.y * sz + xy_R.x);
			if (movable_map(aoc24::Direction::Right).test(pos_R)) {
				--xy_R.x;
			}

			auto& start_box_R = find_box(xy_R.x, xy_R.y);
			if (!start_box_R.is_dummy()) {
				auto& adj_to_clear = static_cast<Adjacent_tile_wide&>(
				    adj_from_dir(start_box_R, dir_l));
				if (!dir_is_UD(dir)) {
					adj_to_clear.left_tile->type = ATT::Empty;
					adj_to_clear.left_tile->box = nullptr;
					adj_to_clear.right_tile->type = ATT::Empty;
					adj_to_clear.right_tile->box = nullptr;
				} else if (offs_R.x < 0) {
					adj_to_clear.right_tile->type = ATT::Empty;
					adj_to_clear.right_tile->box = nullptr;
				} else {
					adj_to_clear.left_tile->type = ATT::Empty;
					adj_to_clear.left_tile->box = nullptr;
				}
			}

			rob_.move(dir);

			auto adj_start_type = movable_map(aoc24::Direction::Left)
			                              .test(adj_pos)
			                          ? ATT::Box_movable_left
			                          : ATT::Box_movable_right;

			// Update maps
			int num_boxes = static_cast<int>(box_ptr_v.size());
			aoc24::Vec2d end_xy = {adj_xy.x + offs.x * num_boxes,
			                       adj_xy.y + offs.y * num_boxes};

			if (dir_is_UD(dir)) {
				for (int j = num_boxes - 1; j >= 0; --j) {
					aoc24::Vec2d current_xy
					    = static_cast<Box_wide*>(box_ptr_v[j])->position();

					aoc24::Vec2d back_xy = {current_xy.x - offs.x,
					                        current_xy.y - offs.y};
					size_t current_pos = current_xy.y * sz + current_xy.x;
					size_t back_pos = back_xy.y * sz + back_xy.x;

					movable_map(aoc24::Direction::Left).reset(back_pos);
					movable_map(aoc24::Direction::Left).set(current_pos);
					movable_map(aoc24::Direction::Right).set(current_pos + 1);
					movable_map(aoc24::Direction::Right).reset(back_pos + 1);
				}
			} else {
				size_t start_pos = box_ptr_v.front()->position().y * sz
				                   + box_ptr_v.front()->position().x;
				if (dir_is_L(dir)) {
					for (int i = 0; i < num_boxes; ++i) {
						size_t p_L = start_pos + 1 - i * 2;
						size_t p_R = p_L + 1;
						movable_map(aoc24::Direction::Left).reset(p_L);
						movable_map(aoc24::Direction::Left).set(p_L - 1);
						movable_map(aoc24::Direction::Right).reset(p_R);
						movable_map(aoc24::Direction::Right).set(p_L);
					}
				} else {
					for (int i = 0; i < num_boxes; ++i) {
						size_t p_L = start_pos - 1 + i * 2;
						size_t p_R = p_L + 1;
						movable_map(aoc24::Direction::Left).reset(p_L);
						movable_map(aoc24::Direction::Left).set(p_R);
						movable_map(aoc24::Direction::Right).reset(p_R);
						movable_map(aoc24::Direction::Right).set(p_R + 1);
					}
				}
			}

			for (size_t i = 0; i < box_ptr_v.size(); ++i) {
				// Update adjacent of stationary
				if (dir_is_UD(dir)) {
					for (auto* stat_L : stationary_adj_L) {
						update_adj(*stat_L, aoc24::Direction::Right);
					}
					for (auto* stat_R : stationary_adj_R) {
						update_adj(*stat_R, aoc24::Direction::Left);
					}
				}

				// Update adjacent of moved boxes
				Box_wide& b = *(static_cast<Box_wide*>(box_ptr_v[i]));

				bool adj_inv_is_also_moved = dir_is_UD(dir) ? false : true;
				bool adj_R_is_also_moved = false;
				bool adj_L_is_also_moved = false;
				if (dir_is_UD(dir)) {
					const auto& adj_inv = (dir == aoc24::Direction::Up)
					                          ? b.adj_D
					                          : b.adj_U;
					bool look_for_L = ((adj_inv.left_tile->type
					                    == ATT::Box_movable_left)
					                   || (adj_inv.left_tile->type
					                       == ATT::Box_movable_right));
					bool look_for_R = ((adj_inv.right_tile->type
					                    == ATT::Box_movable_left)
					                   || (adj_inv.right_tile->type
					                       == ATT::Box_movable_right));
					for (size_t j = 0;
					     (look_for_L || look_for_R) && j < box_ptr_v.size();
					     ++j) {
						const Box* moved_b = box_ptr_v[j];
						int count_matches = 0;
						if (look_for_L
						    && &moved_b == &(adj_inv.left_tile->box)) {
							look_for_L = false;
							++count_matches;
						}
						if (look_for_R
						    && &moved_b == &(adj_inv.right_tile->box)) {
							look_for_R = false;
							++count_matches;
						}
						if (count_matches == 2) {
							adj_inv_is_also_moved = true;
							break;
						}
					}
					if (b.adj_L.right_tile->type == ATT::Box_movable_right) {
						for (const Box* moved_b : box_ptr_v) {
							if (&moved_b == &(b.adj_L.right_tile->box)) {
								adj_L_is_also_moved = true;
								break;
							}
						}
					}
					if (b.adj_R.left_tile->type == ATT::Box_movable_left) {
						for (const Box* moved_b : box_ptr_v) {
							if (&moved_b == &b.adj_R.left_tile->box) {
								adj_R_is_also_moved = true;
								break;
							}
						}
					}
				}
				if (!adj_inv_is_also_moved) {
					update_adj(b,
					           (dir == aoc24::Direction::Up
					                ? aoc24::Direction::Down
					                : aoc24::Direction::Up));
				}
				if (!adj_L_is_also_moved) {
					update_adj(b, dir_l);
				}
				if (!adj_R_is_also_moved) {
					update_adj(b, dir_r);
				}

				update_adj(b, dir);
			}

			//And check for boxes that may have gotten stuck
			bool box_may_be_stuck = false;

			for (int k = num_boxes - 1; k >= 0; --k) {
				const auto& adj_fwd_t = static_cast<Adjacent_tile_wide&>(
				    adj_from_dir(*box_ptr_v[k], dir));
				if (!box_may_be_stuck
				    && (adj_fwd_t.left_tile->type == ATT::Box_stuck_left
				        || adj_fwd_t.left_tile->type == ATT::Box_stuck_right
				        || adj_fwd_t.left_tile->type == ATT::Wall
				        || adj_fwd_t.right_tile->type == ATT::Box_stuck_left
				        || adj_fwd_t.right_tile->type == ATT::Box_stuck_right
				        || adj_fwd_t.right_tile->type == ATT::Wall)) {
					box_may_be_stuck = true;
				}
			}

			// Prune if any of the boxes moved may have gotten stuck
			if (box_may_be_stuck) {
				prune_boxes();
			}
		}
	} else {
		// Move robot onto empty tile
		rob_.move(dir);
	}
}

//------------------------------------------------------------------------------

void aoc24_15::Box_grid_wide::init_maps()
{
	if (map_ptr_init_) {
		return;
	}
	walls_ = &add_map(keys::wall_key);
	movable_L_ = &add_map(keys::movable_left_key);
	movable_R_ = &add_map(keys::movable_right_key);
	stuck_L_ = &add_map(keys::stuck_left_key);
	stuck_R_ = &add_map(keys::stuck_right_key);
	map_ptr_init_ = true;
}

bool aoc24_15::Box_grid_wide::is_on_box_map(size_t pos) const
{
	return (movable_map(aoc24::Direction::Left).test(pos)
	        || movable_map(aoc24::Direction::Right).test(pos)
	        || stuck_map(aoc24::Direction::Left).test(pos)
	        || stuck_map(aoc24::Direction::Right).test(pos));
}

aoc24::Dyn_bitset& aoc24_15::Box_grid_wide::walls_map() const
{
	throw_if_no_map();
	return *walls_;
}

aoc24::Dyn_bitset& aoc24_15::Box_grid_wide::movable_map(aoc24::Direction dir) const
{
	throw_if_no_map();
	if (dir == aoc24::Direction::Left) {
		return *movable_L_;
	} else if (dir == aoc24::Direction::Right) {
		return *movable_R_;
	}
	throw std::logic_error("Invalid direction");
	return *(*map_.begin()).second;
}

aoc24::Dyn_bitset& aoc24_15::Box_grid_wide::stuck_map(aoc24::Direction dir) const
{
	throw_if_no_map();
	if (dir == aoc24::Direction::Left) {
		return *stuck_L_;
	} else if (dir == aoc24::Direction::Right) {
		return *stuck_R_;
	}
	throw std::logic_error("Invalid direction");
	return *(*map_.begin()).second;
}

//------------------------------------------------------------------------------

void aoc24_15::Box_grid_wide::print_map(std::ostream& ostr)
{
	size_t i = 0;
	for (size_t y = 0; y < sz_y; ++y) {
		for (size_t x = 0; x < sz; ++x) {
			bool has_rob = rob_.position().x == x && rob_.position().y == y;
			bool has_wal = walls_map().test(i);
			bool has_mvl = movable_map(aoc24::Direction::Left).test(i);
			bool has_mvr = movable_map(aoc24::Direction::Right).test(i);
			bool has_stl = stuck_map(aoc24::Direction::Left).test(i);
			bool has_str = stuck_map(aoc24::Direction::Right).test(i);
			char c{};
			if (has_rob + has_wal + has_mvl + has_mvr + has_stl + has_str > 1) {
				c = (has_rob) ? '$' : '!';
			} else {
				if (has_rob) {
					c = chars::robot_ch;
				} else if (has_wal) {
					c = chars::wall_ch;
				} else if (has_mvl) {
					c = chars::box_movable_L_ch;
				} else if (has_mvr) {
					c = chars::box_movable_R_ch;
				} else if (has_stl) {
					c = chars::box_stuck_L_ch;
				} else if (has_str) {
					c = chars::box_stuck_R_ch;
				} else {
					c = ' ';
				}
			}
			ostr << c;
			++i;
		}
		ostr << '\n';
	}
}

//------------------------------------------------------------------------------

long long aoc24_15::Box_grid_wide::sum_of_all_box_coordinates() const
{
	constexpr int top_dist_f{100};

	// Use print map to temporarily combine box maps
	print_->reset();
	(*print_) |= movable_map(aoc24::Direction::Left);
	(*print_) |= stuck_map(aoc24::Direction::Left);

	long long sum = 0;
	size_t pos = 0;
	for (int row = 0; row < static_cast<int>(sz_y); ++row) {
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

void aoc24_15::Box_grid_wide::add_box(int pos_x, int pos_y)
{
	size_t pos = pos_y * sz + pos_x;
	if (!valid_xy(pos_x, pos_y)) {
		throw std::invalid_argument(
		    "Cannot add box, its current position (" + std::to_string(pos_x)
		    + ", " + std::to_string(pos_y) + ") is out of bounds");
	}
	Box_grid_wide::throw_if_occupied(pos);

	movable_map(aoc24::Direction::Left).set(pos);
	movable_map(aoc24::Direction::Right).set(pos + 1);
	box_.emplace_back(Box_wide(pos_x, pos_y));
}

aoc24_15::Box& aoc24_15::Box_grid_wide::find_box(int pos_x,
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

aoc24_15::Adjacent_tile& aoc24_15::Box_grid_wide::adj_from_dir(
    Box& box, aoc24::Direction dir, bool invert)
{
	if (invert) {
		dir = static_cast<aoc24::Direction>((static_cast<int>(dir) + 2) % 4);
	}

	auto& box_s = static_cast<Box_wide&>(box);
	Adjacent_tile_wide* t = nullptr;
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

void aoc24_15::Box_grid_wide::update_adj(Box& box, aoc24::Direction dir)
{
	aoc24::Vec2d offs = dir_to_offset(dir);
	offs.x *= (dir == aoc24::Direction::Right) ? 2
	                                           : 1; // Boxes are two tiles wide
	auto& box_s = static_cast<Box_wide&>(box);
	aoc24::Vec2d adj_xy = {box_s.pos_.x + offs.x, box_s.pos_.y + offs.y};
	if (is_oob(adj_xy.x, adj_xy.y)) {
		return;
	}

	using ATT = Adjacent_subtile::Type;
	auto& tile = static_cast<Adjacent_tile_wide&>(
	    Box_grid_wide::adj_from_dir(box, dir));
	std::vector<aoc24::Vec2d> adj_xy_v = {adj_xy};
	if (dir_is_UD(dir)) {
		// Up / Down means there are two adjacent tiles
		adj_xy_v.emplace_back(
		    aoc24::Vec2d{adj_xy_v.front().x + 1, adj_xy_v.front().y});
	}

	for (int i = 0; i < static_cast<int>(adj_xy_v.size()); ++i) {
		Adjacent_subtile* subtile_ptr = nullptr;
		if (i == 0) {
			subtile_ptr = dir == aoc24::Direction::Right ? tile.right_tile.get()
			                                             : tile.left_tile.get();
		} else {
			subtile_ptr = tile.right_tile.get();
		}
		auto& subtile = *subtile_ptr;

		const aoc24::Vec2d& xy = adj_xy_v[i];
		const size_t pos = adj_xy_v[i].y * sz + adj_xy_v[i].x;
		if (is_on_box_map(pos)) {
			bool is_mov_L = movable_map(aoc24::Direction::Left).test(pos);
			if (is_mov_L || movable_map(aoc24::Direction::Right).test(pos)) {
				int offs_x = 0;
				if ((dir_is_UD(dir) && !is_mov_L) || dir_is_L(dir)) {
					offs_x = -1;
				}

				Box& adj_box = find_box(xy.x + offs_x, xy.y, true);
				if (adj_box.is_dummy()) {
					print_map();
					throw std::logic_error(
					    "Cannot link boxes: No box found with coordinates ("
					    + std::to_string(xy.x + offs_x) + ", "
					    + std::to_string(xy.y) + ")");
				}
				subtile.type = is_mov_L ? ATT::Pending_left
				                        : ATT::Pending_right;
				subtile.box = nullptr;
				link_adj_boxes(box, adj_box, dir);
			} else if (stuck_map(aoc24::Direction::Left).test(pos)) {
				subtile.type = ATT::Box_stuck_left;
				subtile.box = nullptr;
			} else {
				subtile.type = ATT::Box_stuck_right;
				subtile.box = nullptr;
			}
		} else if (walls_map().test(pos)) {
			subtile.type = ATT::Wall;
			subtile.box = nullptr;
		} else {
			if (dir_is_UD(dir)
			    && (subtile.type == ATT::Box_movable_left
			        || subtile.type == ATT::Box_movable_right)) {
				auto& linked_b = static_cast<Box_wide&>(*subtile.box);
				auto& linked_adj = static_cast<Adjacent_tile_wide&>(
				    adj_from_dir(*subtile.box, dir, true));
				if (box.position().x == linked_b.position().x
				    || std::abs(box.position().y - linked_b.position().y) == 2) {
					if (subtile.type == ATT::Box_movable_left) {
						linked_adj.left_tile->type = ATT::Empty;
						linked_adj.left_tile->box = nullptr;
					}
					if (subtile.type == ATT::Box_movable_right) {
						linked_adj.right_tile->type = ATT::Empty;
						linked_adj.right_tile->box = nullptr;
					}
				}
			}
			subtile.type = ATT::Empty;
			subtile.box = nullptr;
		}
	}
}

void aoc24_15::Box_grid_wide::link_adj_boxes(Box& box_a,
                                             Box& box_b,
                                             aoc24::Direction from_a_to_b)
{
	using ATT = Adjacent_subtile::Type;
	auto& adj_tile_a = static_cast<Adjacent_tile_wide&>(
	    Box_grid_wide::adj_from_dir(box_a, from_a_to_b));
	auto& adj_tile_b = static_cast<Adjacent_tile_wide&>(
	    Box_grid_wide::adj_from_dir(box_b, from_a_to_b, true));

	bool is_pending_L = (adj_tile_a.left_tile->type == ATT::Pending_left
	                     || adj_tile_a.left_tile->type == ATT::Pending_right);
	bool is_pending_R = (adj_tile_a.right_tile->type == ATT::Pending_left
	                     || adj_tile_a.right_tile->type == ATT::Pending_right);
	if (is_pending_L && is_pending_R) {
		throw std::logic_error(
		    "Both adjacent subtiles can not be Pending at the same point "
		    "in time");
	}
	if (!is_pending_L && !is_pending_R) {
		throw std::logic_error(
		    "Expected either the right or left subtile to be pending");
	}
	auto& subtile_a = (is_pending_L) ? adj_tile_a.left_tile
	                                 : adj_tile_a.right_tile;
	const ATT pending_type = (is_pending_L) ? adj_tile_a.left_tile->type
	                                        : adj_tile_a.right_tile->type;
	bool pending_L = (pending_type == ATT::Pending_left);
	const ATT final_type_a = pending_L ? ATT::Box_movable_left
	                                   : ATT::Box_movable_right;

	ATT final_type_b = ATT::Unknown;
	Adjacent_subtile* subtile_b_ptr = nullptr;
	if (dir_is_UD(from_a_to_b)) {
		if (box_a.position().x == box_b.position().x) {
			subtile_b_ptr = is_pending_L ? adj_tile_b.left_tile.get()
			                             : adj_tile_b.right_tile.get();
			final_type_b = pending_L ? ATT::Box_movable_left
			                         : ATT::Box_movable_right;
		} else {
			subtile_b_ptr = is_pending_L ? adj_tile_b.right_tile.get()
			                             : adj_tile_b.left_tile.get();
			final_type_b = pending_L ? ATT::Box_movable_right
			                         : ATT::Box_movable_left;
			Adjacent_subtile& subtile_c = is_pending_L
			                                  ? *adj_tile_b.left_tile.get()
			                                  : *adj_tile_b.right_tile.get();
			size_t subtile_c_pos = is_pending_L ? static_cast<size_t>(
			                           box_a.position().y * sz
			                           + box_b.position().x)
			                                    : static_cast<size_t>(
			                                        box_a.position().y * sz
			                                        + box_b.position().x + 1);
			if (!is_on_box_map(subtile_c_pos)
			    && !walls_map().test(subtile_c_pos)) {
				Box* temp_box = subtile_c.box;
				ATT temp_type = subtile_c.type;
				subtile_c.type = ATT::Empty;
				subtile_c.box = nullptr;
				bool linked_subtile_L = (temp_type == ATT::Box_movable_left);
				if (linked_subtile_L || temp_type == ATT::Box_movable_right) {
					auto& adj_tile_c_linked = static_cast<Adjacent_tile_wide&>(
					    adj_from_dir(*temp_box, from_a_to_b));
					auto& subtile_c_linked
					    = linked_subtile_L
					          ? *adj_tile_c_linked.left_tile.get()
					          : *adj_tile_c_linked.right_tile.get();
					subtile_c_linked.type = ATT::Empty;
					subtile_c_linked.box = nullptr;
				}
			}
		}
	} else {
		subtile_b_ptr = (dir_is_R(from_a_to_b)) ? adj_tile_b.left_tile.get()
		                                        : adj_tile_b.right_tile.get();
		final_type_b = (dir_is_R(from_a_to_b)) ? ATT::Box_movable_right
		                                       : ATT::Box_movable_left;
	}

	auto& subtile_b = subtile_b_ptr;
	subtile_a->box = &box_b;
	subtile_b->box = &box_a;
	subtile_a->type = final_type_a;
	subtile_b->type = final_type_b;
}

//------------------------------------------------------------------------------

void aoc24_15::Box_grid_wide::prune_boxes()
{
	bool keep_looking = true;
	while (keep_looking) {
		keep_looking = false; // Only keep looking if new matches are found
		for (auto it = box_.begin(); it != box_.end(); ++it) {
			Box& b = *it;
			const auto& b_stuck = Box_grid_wide::is_stuck(b);
			if (b_stuck.has_corner_stuck()) {
				keep_looking = true;
				Box_grid_wide::mark_as_stuck(b);
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

void aoc24_15::Box_grid_wide::mark_as_stuck(Box& b)
{
	using ATT = Adjacent_subtile::Type;
	auto& box_s = static_cast<Box_wide&>(b);
	box_s.marked_for_pruning_ = true;
	stuck_map(aoc24::Direction::Left).set(box_s.pos_.y * size() + box_s.pos_.x);
	stuck_map(aoc24::Direction::Right)
	    .set(box_s.pos_.y * size() + box_s.pos_.x + 1);
	movable_map(aoc24::Direction::Left)
	    .reset(box_s.pos_.y * size() + box_s.pos_.x);
	movable_map(aoc24::Direction::Right)
	    .reset(box_s.pos_.y * size() + box_s.pos_.x + 1);
	for (int i = 0; i < 4; ++i) {
		aoc24::Direction adj_dir = static_cast<aoc24::Direction>(i);
		const auto& adj_t_a = static_cast<Adjacent_tile_wide&>(
		    Box_grid_wide::adj_from_dir(box_s, adj_dir));
		for (int j = 0; j < (dir_is_UD(adj_dir) ? 2 : 1); ++j) {
			const bool use_R = (j == 1 || adj_dir == aoc24::Direction::Right);
			const auto& subtile_a = use_R ? adj_t_a.right_tile.get()
			                              : adj_t_a.left_tile.get();
			if (subtile_a->type == ATT::Box_movable_left
			    || subtile_a->type == ATT::Box_movable_right) {
				auto& adj_t_b = static_cast<Adjacent_tile_wide&>(
				    Box_grid_wide::adj_from_dir(*subtile_a->box, adj_dir, true));
				if (subtile_a->type == ATT::Box_movable_left) {
					adj_t_b.left_tile->type = use_R ? ATT::Box_stuck_right
					                                : ATT::Box_stuck_left;
					adj_t_b.left_tile->box = nullptr;
				} else if (subtile_a->type == ATT::Box_movable_right) {
					adj_t_b.right_tile->type = use_R ? ATT::Box_stuck_right
					                                 : ATT::Box_stuck_left;
					adj_t_b.right_tile->box = nullptr;
				}
			}
		}
	}
}

bool aoc24_15::Box_grid_wide::adj_is_on_stuck_map(const Box& b,
                                                  aoc24::Direction dir) const
{
	aoc24::Vec2d offs = dir_to_offset(dir);
	auto& box_s = static_cast<const Box_wide&>(b);
	aoc24::Vec2d adj_xy = {box_s.pos_.x + offs.x, box_s.pos_.y + offs.y};
	if (is_oob(adj_xy.x, adj_xy.y)) {
		return false;
	}
	if (dir_is_UD(dir)) {
		return (
		    stuck_map(aoc24::Direction::Left).test(adj_xy.y * size() + adj_xy.x)
		    || stuck_map(aoc24::Direction::Left)
		           .test(adj_xy.y * size() + adj_xy.x + 1)
		    || stuck_map(aoc24::Direction::Right)
		           .test(adj_xy.y * size() + adj_xy.x)
		    || stuck_map(aoc24::Direction::Right)
		           .test(adj_xy.y * size() + adj_xy.x + 1));
	}
	return (stuck_map(aoc24::Direction::Left).test(adj_xy.y * size() + adj_xy.x)
	        || stuck_map(aoc24::Direction::Right)
	               .test(adj_xy.y * size() + adj_xy.x));
}

aoc24_15::Is_stuck aoc24_15::Box_grid_wide::is_stuck(const Box& b) const
{
	using ATT = Adjacent_subtile::Type;
	auto& box_s = static_cast<const Box_wide&>(b);
	Is_stuck s;
	s.up = (box_s.adj_U.left_tile->type == ATT::Wall
	        || box_s.adj_U.left_tile->type == ATT::Box_stuck_left
	        || box_s.adj_U.left_tile->type == ATT::Box_stuck_right
	        || box_s.adj_U.right_tile->type == ATT::Wall
	        || box_s.adj_U.right_tile->type == ATT::Box_stuck_left
	        || box_s.adj_U.right_tile->type == ATT::Box_stuck_right
	        || Box_grid_wide::adj_is_on_stuck_map(box_s, aoc24::Direction::Up));
	s.right = (box_s.adj_R.right_tile->type == ATT::Wall
	           || box_s.adj_R.right_tile->type == ATT::Box_stuck_left
	           || Box_grid_wide::adj_is_on_stuck_map(box_s,
	                                                 aoc24::Direction::Right));
	s.down = (box_s.adj_D.left_tile->type == ATT::Wall
	          || box_s.adj_D.left_tile->type == ATT::Box_stuck_left
	          || box_s.adj_D.left_tile->type == ATT::Box_stuck_right
	          || box_s.adj_D.right_tile->type == ATT::Wall
	          || box_s.adj_D.right_tile->type == ATT::Box_stuck_left
	          || box_s.adj_D.right_tile->type == ATT::Box_stuck_right
	          || Box_grid_wide::adj_is_on_stuck_map(box_s,
	                                                aoc24::Direction::Up));
	s.left = (box_s.adj_L.left_tile->type == ATT::Wall
	          || box_s.adj_R.left_tile->type == ATT::Box_stuck_right
	          || Box_grid_wide::adj_is_on_stuck_map(box_s,
	                                                aoc24::Direction::Left));
	return s;
}

//------------------------------------------------------------------------------

std::vector<aoc24_15::Box*> aoc24_15::Box_grid_wide::movable_boxes_in_dir(
    int pos_x, int pos_y, aoc24::Direction dir)
{
	aoc24::Vec2d offs = dir_to_offset(dir);
	if (dir == aoc24::Direction::Left || dir == aoc24::Direction::Right) {
		offs.x *= 2;
	}
	aoc24::Vec2d adj_xy = {pos_x + offs.x, pos_y + offs.y};
	if (is_oob(adj_xy.x, adj_xy.y)) {
		return {};
	}

	if (movable_map(aoc24::Direction::Right).test(adj_xy.y * sz + adj_xy.x)) {
		--adj_xy.x;
	}
	std::stack<Adjacent_subtile> adj_subtiles;
	std::set<aoc24::Vec2d, aoc24::Vec2d_comparator> cached_boxes;

	Box& start_b = find_box(adj_xy.x, adj_xy.y, true);
	if (start_b.is_dummy()) {
		throw std::logic_error(
		    "Cannot scan for movable boxes: No box found with coordinates ("
		    + std::to_string(adj_xy.x) + ", " + std::to_string(adj_xy.y) + ")");
	}

	std::stack<Box*> box_stack;
	box_stack.push(&start_b);
	std::vector<Box*> final_boxes;

	using ATT = Adjacent_subtile::Type;
	bool found_wall_or_stuck = false;
	while (!found_wall_or_stuck && !box_stack.empty()) {
		Box* current_box_ptr = box_stack.top();
		box_stack.pop();

		for (int i = 0; i < (dir_is_UD(dir) ? 2 : 1); ++i) {
			auto& adj_tile = static_cast<Adjacent_tile_wide&>(
			    adj_from_dir(*current_box_ptr, dir));
			const auto& adj_subtile = (i == 1 || dir_is_R(dir)
			                               ? adj_tile.right_tile
			                               : adj_tile.left_tile);
			if (adj_subtile->type == ATT::Wall
			    || adj_subtile->type == ATT::Box_stuck_left
			    || adj_subtile->type == ATT::Box_stuck_right) {
				found_wall_or_stuck = true;
				break;
			} else if ((adj_subtile->type == ATT::Box_movable_left
			            || adj_subtile->type == ATT::Box_movable_right)
			           && (!has_cached_pos(adj_subtile->box->position(),
			                               cached_boxes))) {
				box_stack.push(adj_subtile->box);
				cached_boxes.insert(adj_subtile->box->position());
			}
		}

		if (!found_wall_or_stuck) {
			final_boxes.emplace_back(current_box_ptr);
		}
	}

	if (found_wall_or_stuck) {
		return {};
	}
	return final_boxes;
}

//------------------------------------------------------------------------------

void aoc24_15::Box_grid_wide::throw_if_occupied(size_t pos) const
{
	if (walls_map().test(pos) || walls_map().test(pos + 1)
	    || movable_map(aoc24::Direction::Left).test(pos)
	    || movable_map(aoc24::Direction::Left).test(pos + 1)
	    || movable_map(aoc24::Direction::Right).test(pos)
	    || movable_map(aoc24::Direction::Right).test(pos + 1)
	    || stuck_map(aoc24::Direction::Left).test(pos)
	    || stuck_map(aoc24::Direction::Left).test(pos + 1)
	    || stuck_map(aoc24::Direction::Right).test(pos)
	    || stuck_map(aoc24::Direction::Right).test(pos + 1)) {
		throw std::logic_error("Cannot add box: tile is already occupied");
	}
}

//------------------------------------------------------------------------------

bool aoc24_15::Box_grid_wide::boxes_match_maps()
{
	for (auto& b : box_) {
		for (int i = 0; i < 4; ++i) {
			aoc24::Direction dir = static_cast<aoc24::Direction>(i);
			aoc24::Vec2d offs = dir_to_offset(dir);
			if (dir_is_R(dir)) {
				offs.x *= 2; // Box is two units wide
			}
			aoc24::Vec2d adj_xy_L = {b.position().x + offs.x,
			                         b.position().y + offs.y};
			aoc24::Vec2d adj_xy_R = {adj_xy_L.x + 1, adj_xy_L.y};

			const auto& adj_t = static_cast<Adjacent_tile_wide&>(
			    adj_from_dir(b, static_cast<aoc24::Direction>(i)));

			auto validate = validate_if_adj_unknown(adj_t,
			                                        adj_xy_L,
			                                        adj_xy_R,
			                                        dir,
			                                        false,
			                                        false);
			if (!validate.is_valid) {
				return false;
			}

			validate = validate_if_adj_empty(adj_t,
			                                 adj_xy_L,
			                                 adj_xy_R,
			                                 dir,
			                                 validate.found_left,
			                                 validate.found_right);
			if (!validate.is_valid) {
				return false;
			}

			validate = validate_if_adj_wall(adj_t,
			                                adj_xy_L,
			                                adj_xy_R,
			                                dir,
			                                validate.found_left,
			                                validate.found_right);
			if (!validate.is_valid) {
				return false;
			}

			validate = validate_if_adj_box(adj_t,
			                               adj_xy_L,
			                               adj_xy_R,
			                               dir,
			                               validate.found_left,
			                               validate.found_right);
			if (!validate.is_valid) {
				return false;
			}
		}
	}
	return true;
}

//------------------------------------------------------------------------------

bool aoc24_15::Box_grid_wide::dir_is_UD(const aoc24::Direction dir) const
{
	return (dir == aoc24::Direction::Up || dir == aoc24::Direction::Down);
}

bool aoc24_15::Box_grid_wide::dir_is_L(const aoc24::Direction dir) const
{
	return dir == aoc24::Direction::Left;
}

bool aoc24_15::Box_grid_wide::dir_is_R(const aoc24::Direction dir) const
{
	return dir == aoc24::Direction::Right;
}

bool aoc24_15::Box_grid_wide::dir_is_UDL(const aoc24::Direction dir) const
{
	return (dir_is_UD(dir) || dir_is_L(dir));
}

bool aoc24_15::Box_grid_wide::dir_is_UDR(const aoc24::Direction dir) const
{
	return (dir_is_UD(dir) || dir_is_R(dir));
}

bool aoc24_15::Box_grid_wide::adj_type_is_box(Adjacent_subtile::Type t) const
{
	return (t == Adjacent_subtile::Type::Box_movable_left
	        || t == Adjacent_subtile::Type::Box_movable_right
	        || t == Adjacent_subtile::Type::Box_stuck_left
	        || t == Adjacent_subtile::Type::Box_stuck_right);
}

const aoc24::Dyn_bitset& aoc24_15::Box_grid_wide::box_map(
    Adjacent_subtile::Type t) const
{
	const aoc24::Dyn_bitset* map = nullptr;
	using ATT = Adjacent_subtile::Type;
	if (t == ATT::Box_movable_left) {
		map = &movable_map(aoc24::Direction::Left);
	} else if (t == ATT::Box_movable_right) {
		map = &movable_map(aoc24::Direction::Right);
	} else if (t == ATT::Box_stuck_left) {
		map = &stuck_map(aoc24::Direction::Left);
	} else if (t == ATT::Box_stuck_right) {
		map = &stuck_map(aoc24::Direction::Right);
	} else {
		throw std::invalid_argument("Invalid box map type");
	}
	return *map;
}

const std::string aoc24_15::Box_grid_wide::type_to_str(
    Adjacent_subtile::Type t) const
{
	switch (t) {
	case Adjacent_subtile::Type::Empty:
		return "Empty";
	case Adjacent_subtile::Type::Wall:
		return "Wall";
	case Adjacent_subtile::Type::Box_stuck_left:
		return "Left side of Stuck box";
	case Adjacent_subtile::Type::Box_stuck_right:
		return "Right side of Stuck box";
	case Adjacent_subtile::Type::Box_movable_left:
		return "Left side of Movable box";
	case Adjacent_subtile::Type::Box_movable_right:
		return "Right side of Movable box";
	case Adjacent_subtile::Type::Pending_left:
		return "Pending-Left";
	case Adjacent_subtile::Type::Pending_right:
		return "Pending-Right";
	default:
		throw std::invalid_argument(
		    "Unknown type cannot be converted to string");
		break;
	}
	return "";
}

int aoc24_15::Box_grid_wide::presence_in_maps(const size_t pos) const
{
	return (walls_map().test(pos) + movable_map(aoc24::Direction::Left).test(pos)
	        + stuck_map(aoc24::Direction::Left).test(pos)
	        + movable_map(aoc24::Direction::Right).test(pos)
	        + stuck_map(aoc24::Direction::Right).test(pos));
}

bool aoc24_15::Box_grid_wide::on_no_map(const aoc24::Vec2d& xy,
                                        const aoc24::Direction dir) const
{
	const size_t pos = xy.y * sz + xy.x;
	if (presence_in_maps(pos) != 0) {
		std::cout << "Expected empty at position (" << xy.x << ", " << xy.y
		          << "). Dir " << dir << '\n';
		return false;
	}
	return true;
}

bool aoc24_15::Box_grid_wide::only_on_a_single_map(
    const aoc24::Vec2d& xy,
    const aoc24::Direction dir,
    const aoc24::Dyn_bitset& only_map,
    const std::string& expected_type) const
{
	const size_t pos = xy.y * sz + xy.x;
	const int num_maps = presence_in_maps(pos);
	if (num_maps > 1 || !only_map.test(pos)) {
		std::cerr << "Expected " << expected_type << " at position (" << xy.x
		          << ", " << xy.y << "). Dir " << dir << '\n';
		return false;
	}
	return true;
}
bool aoc24_15::Box_grid_wide::adj_subtile_is_invalid_side(
    const aoc24::Vec2d& pos,
    const Adjacent_tile_wide& tile,
    const aoc24::Direction dir,
    const bool stuck) const
{
	bool is_invalid = false;
	if (dir == aoc24::Direction::Left) {
		Adjacent_subtile::Type invalid_t
		    = stuck ? Adjacent_subtile::Type::Box_stuck_left
		            : Adjacent_subtile::Type::Box_movable_left;
		if (tile.right_tile->type == invalid_t) {
			is_invalid = true;
		}
	} else if (dir == aoc24::Direction::Right) {
		Adjacent_subtile::Type invalid_t
		    = stuck ? Adjacent_subtile::Type::Box_stuck_right
		            : Adjacent_subtile::Type::Box_movable_right;
		if (tile.left_tile->type == invalid_t) {
			is_invalid = true;
		}
	} else {
		throw std::invalid_argument("aoc24::Direction to determine invalid "
		                            "side may only be Left / Right");
	}
	if (is_invalid) {
		std::cerr << "Expected "
		          << (dir == aoc24::Direction::Left ? "left" : "right")
		          << " part of " << (stuck ? "STUCK " : "")
		          << "box at position (" << pos.x << ", " << pos.y << "). Dir "
		          << dir << '\n';
		return true;
	}
	return false;
}

bool aoc24_15::Box_grid_wide::unused_subtile_is_unknown(
    const aoc24::Vec2d& pos,
    const Adjacent_tile_wide& tile,
    const aoc24::Direction dir) const
{
	if (dir == aoc24::Direction::Left) {
		if (tile.right_tile->type != Adjacent_subtile::Type::Unknown) {
			std::cerr << "Expected unknown right subtile at position (" << pos.x
			          << ", " << pos.y << "). Dir " << dir << '\n';
			return false;
		}
	} else if (dir == aoc24::Direction::Right) {
		if (tile.left_tile->type != Adjacent_subtile::Type::Unknown) {
			std::cerr << "Expected unknown left subtile at position (" << pos.x
			          << ", " << pos.y << "). Dir " << dir << '\n';
			return false;
		}
	} else {
		throw std::invalid_argument(
		    "aoc24::Direction to determine valid unused "
		    "subtile may only be Left / Right");
	}
	return true;
}

//------------------------------------------------------------------------------
aoc24_15::Tile_validation aoc24_15::Box_grid_wide::validate_if_adj_unknown(
    const Adjacent_tile_wide& adj_t,
    const aoc24::Vec2d& adj_xy_L,
    const aoc24::Vec2d& adj_xy_R,
    const aoc24::Direction dir,
    const bool found_left,
    const bool found_right)
{
	if (dir_is_UD(dir)) {
		if (adj_t.left_tile->type == Adjacent_subtile::Type::Unknown) {
			std::cerr << "Unknown subtile at position (" << adj_xy_L.x << ", "
			          << adj_xy_L.y
			          << "), but the direction from the center tile is " << dir
			          << '\n';
			return {false, true, false};
		}

		if (adj_t.right_tile->type == Adjacent_subtile::Type::Unknown) {
			std::cerr << "Unknown subtile at position (" << adj_xy_R.x << ", "
			          << adj_xy_R.y
			          << "), but the direction from the center tile is " << dir
			          << '\n';
			return {false, false, true};
		}
	}

	if (!found_left && dir_is_R(dir)
	    && adj_t.left_tile->type != Adjacent_subtile::Type::Unknown) {
		std::cerr << "Adjacent left subtile at position (" << adj_xy_L.x << ", "
		          << adj_xy_L.y
		          << ") is a type other than Unknown, but the direction from "
		             "the center tile is "
		          << dir << '\n';
		return {false, true, false};
	}
	const aoc24::Vec2d mod_adj_xy_R = dir_is_R(dir) ? adj_xy_L : adj_xy_R;
	if (!found_right && dir_is_L(dir)
	    && adj_t.right_tile->type != Adjacent_subtile::Type::Unknown) {
		std::cerr << "Adjacent right subtile at position (" << mod_adj_xy_R.x
		          << ", " << mod_adj_xy_R.y
		          << ") is a type other than Unknown, but the direction from "
		             "the center tile is "
		          << dir << '\n';
		return {false, false, true};
	}
	return {true, false, false};
}

aoc24_15::Tile_validation aoc24_15::Box_grid_wide::validate_if_adj_empty(
    const Adjacent_tile_wide& adj_t,
    const aoc24::Vec2d& adj_xy_L,
    const aoc24::Vec2d& adj_xy_R,
    const aoc24::Direction dir,
    const bool found_left,
    const bool found_right)
{
	bool new_found_left = found_left;
	bool new_found_right = found_right;
	if (!found_left && dir_is_UDR(dir)
	    && adj_t.left_tile->type == Adjacent_subtile::Type::Empty) {
		new_found_left = true;
		if (!on_no_map(adj_xy_L, dir)) {
			return {false, true, false};
		}
	}
	const aoc24::Vec2d mod_adj_xy_R = dir_is_R(dir) ? adj_xy_L : adj_xy_R;
	if (!found_right && dir_is_UDL(dir)
	    && adj_t.right_tile->type == Adjacent_subtile::Type::Empty) {
		new_found_right = true;
		if (!on_no_map(mod_adj_xy_R, dir)) {
			return {false, false, true};
		}
	}
	return {true, new_found_left, new_found_right};
}

aoc24_15::Tile_validation aoc24_15::Box_grid_wide::validate_if_adj_wall(
    const Adjacent_tile_wide& adj_t,
    const aoc24::Vec2d& adj_xy_L,
    const aoc24::Vec2d& adj_xy_R,
    const aoc24::Direction dir,
    const bool found_left,
    const bool found_right)
{
	bool new_found_left = found_left;
	bool new_found_right = found_right;

	if (dir_is_UDL(dir) && !found_left
	    && adj_t.left_tile->type == Adjacent_subtile::Type::Wall) {
		new_found_left = true;
		if (!only_on_a_single_map(adj_xy_L,
		                          dir,
		                          walls_map(),
		                          type_to_str(adj_t.left_tile->type))) {
			return {false, true, false};
		}
	}
	const aoc24::Vec2d mod_adj_xy_R = dir_is_R(dir) ? adj_xy_L : adj_xy_R;
	if (dir_is_UDR(dir) && !found_right
	    && adj_t.right_tile->type == Adjacent_subtile::Type::Wall) {
		new_found_right = true;
		if (!only_on_a_single_map(mod_adj_xy_R,
		                          dir,
		                          walls_map(),
		                          type_to_str(adj_t.right_tile->type))) {
			return {false, false, true};
		}
	}
	return {true, new_found_left, new_found_right};
}

aoc24_15::Tile_validation aoc24_15::Box_grid_wide::validate_if_adj_box(
    const Adjacent_tile_wide& adj_t,
    const aoc24::Vec2d& adj_xy_L,
    const aoc24::Vec2d& adj_xy_R,
    const aoc24::Direction dir,
    const bool found_left,
    const bool found_right)
{
	const bool left_is_box = adj_type_is_box(adj_t.left_tile->type);
	const bool right_is_box = adj_type_is_box(adj_t.right_tile->type);
	if (dir_is_UD(dir) && !left_is_box && !right_is_box) {
		return {true, found_left, found_right};
	}

	bool new_found_left = found_left;
	bool new_found_right = found_right;
	if (dir_is_UDL(dir) && !found_left && left_is_box) {
		new_found_left = true;
		if (dir_is_L(dir)
		    && (adj_subtile_is_invalid_side(adj_xy_L, adj_t, dir)
		        || !unused_subtile_is_unknown(adj_xy_L, adj_t, dir))) {
			return {false, true, false};
		}

		if (!only_on_a_single_map(adj_xy_L,
		                          dir,
		                          box_map(adj_t.left_tile->type),
		                          type_to_str(adj_t.left_tile->type))) {
			return {false, true, false};
		}
	}

	const aoc24::Vec2d mod_adj_xy_R = dir_is_R(dir) ? adj_xy_L : adj_xy_R;
	if (dir_is_UDR(dir) && !found_right && right_is_box) {
		new_found_right = true;
		if (dir_is_R(dir)
		    && (adj_subtile_is_invalid_side(mod_adj_xy_R, adj_t, dir)
		        || !unused_subtile_is_unknown(mod_adj_xy_R, adj_t, dir))) {
			return {false, false, true};
		}
		if (!only_on_a_single_map(mod_adj_xy_R,
		                          dir,
		                          box_map(adj_t.right_tile->type),
		                          type_to_str(adj_t.right_tile->type))) {
			return {false, false, true};
		}
	}
	return {true, new_found_left, new_found_right};
}
