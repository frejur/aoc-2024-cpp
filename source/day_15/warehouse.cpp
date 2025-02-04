#include "warehouse.h"
#include "box.h"
#include "keys.h"
#include "warehouse_col.h"
#include <set>
#include <stack>

namespace {
bool has_been_processed(
    const aoc24_15::Box* box,
    const std::set<const aoc24_15::Box*>& processed_boxes)
{
	return (processed_boxes.find(box) != processed_boxes.end());
}
} // namespace
//------------------------------------------------------------------------------

aoc24_15::Warehouse::Warehouse(size_t width,
                               size_t height,
                               std::unique_ptr<aoc24::Dyn_bitset> dyn_bitset,
                               const aoc24::Char_grid& reference_grid)
    : Bit_grid(width, height, std::move(dyn_bitset))
    , map_ptr_init_(false)
    , num_moves_(0)
    , walls_(nullptr)
    , stuck_boxes_(nullptr)
    , collision(new Warehouse_collision(*this))
{
	init_maps();
	add_walls(reference_grid);
}

aoc24::Dyn_bitset& aoc24_15::Warehouse::walls_map() const
{
	throw_if_no_map();
	return *walls_;
}

aoc24::Dyn_bitset& aoc24_15::Warehouse::stuck_map() const
{
	throw_if_no_map();
	return *stuck_boxes_;
}

void aoc24_15::Warehouse::init_maps()
{
	if (map_ptr_init_) {
		return;
	}
	walls_ = &add_map(keys::wall_key);
	stuck_boxes_ = &add_map(keys::stuck_key);
	map_ptr_init_ = true;
}

void aoc24_15::Warehouse::add_walls(const aoc24::Char_grid& reference_grid)
{
	throw_if_no_map();
	size_t pos = 0;
	for (size_t row = 0; row < sz_y; ++row) {
		for (size_t col = 0; col < sz; ++col) {
			char c = reference_grid.char_at(col, row);
			switch (c) {
			case chars::empty_ch:
				break;
			case chars::wall_ch:
				walls_map().set(pos);
				break;
			default:
				throw std::invalid_argument(
				    "The reference grid contains unknown character '"
				    + std::string{c} + "'.");
			}
			++pos;
		}
	}
}

void aoc24_15::Warehouse::prune_stuck_boxes()
{
	bool keep_looking = true;
	while (keep_looking) {
		keep_looking = false; // Only keep looking if new matches are found
		for (auto it = boxes_.begin(); it != boxes_.end(); ++it) {
			Box& b = *it->get();
			if (b.prune_ || b.is_stuck()) {
				keep_looking = true;
				add_box_to_map(b, stuck_map());
				if (!b.prune_) {
					b.prune_ = true;
				}
			}
		}

		if (keep_looking) {
			// Prune
			for (auto it = boxes_.begin(); it != boxes_.end();) {
				const auto& b = *it->get();
				if (b.prune_) {
					stuck_box_positions_.emplace_back(b.position.x,
					                                  b.position.y);
					it = boxes_.erase(it);
				} else {
					++it;
				}
			}
		}
	}
}

void aoc24_15::Warehouse::add_box_to_map(
    const Box& box, aoc24::Dyn_bitset& map)
{
	const long long bitmap = box.bitmap();
	const size_t grid_max = sz * sz_y - 1;
	const size_t start_pos = static_cast<size_t>(box.position.y * sz
	                                             + box.position.x);
	const size_t start_col = start_pos / sz;
	size_t pos = start_pos;
	for (size_t row = 0; row < box.height; ++row) {
		if (pos > grid_max) {
			break;
		}
		for (size_t col = 0; col < box.width; ++col) {
			if ((pos / sz) > start_col) {
				continue;
			}
			bool bit_is_set = (bitmap
			                   >> static_cast<long long>(row * box.width + col))
			                  & 1;
			if (bit_is_set) {
				map.set(pos);
			}
			++pos;
		}
		pos += sz - box.width;
	}
}
void aoc24_15::Warehouse::add_box_to_print_map(
    const Box& box) const
{
	// Casting away constness is safe in this context, because the print map
	// doesn't change the essential state or invariants of the Warehouse object
	const_cast<Warehouse*>(this)->add_box_to_map(box, *print_);
}

std::vector<aoc24_15::Box*> aoc24_15::Warehouse::find_mutable_boxes_in_rect(
    int top_L_x, int top_L_y, int w, int h) const
{
	const std::vector<const Box*> boxes_c{
	    find_boxes_in_rect(top_L_x, top_L_y, w, h)};
	std::vector<Box*> boxes;
	boxes.reserve(boxes_c.size());
	for (const Box* b : boxes_c) {
		// Cast away constness
		boxes.emplace_back(const_cast<Box*>(b));
	}
	return boxes;
}

std::vector<aoc24_15::Box*> aoc24_15::Warehouse::movable_adj_boxes_in_dir(
    const Box& start_box, aoc24::Direction dir) const
{
	// NOTE: Assumes the start box is NOT a Dummy box

	std::set<const Box*> processed_boxes;
	std::stack<Box*> box_stack;

	// The start box needs to be included, and mutable
	Box* start_box_ptr = &(const_cast<Box&>(start_box));
	box_stack.push(start_box_ptr);
	std::vector<Box*> mv_boxes;

	bool is_blocked = false;
	while (!is_blocked && !box_stack.empty()) {
		Box* current_box = box_stack.top();
		box_stack.pop();

		if (has_been_processed(current_box, processed_boxes)) {
			continue;
		}

		std::vector<Edge_tile> tiles = current_box->edge_tiles(dir);
		for (size_t i = 0; !is_blocked && (i < tiles.size()); ++i) {
			const auto& t = tiles[i];
			if (!valid_xy(static_cast<size_t>(t.position.x),
			              static_cast<size_t>(t.position.y))) {
				is_blocked = true;
			} else {
				size_t pos = static_cast<size_t>(t.position.y * sz
				                                 + t.position.x);
				if (collision->wall_or_stuck_box_at(pos)) {
					is_blocked = true;
				}
			}
		}

		if (!is_blocked) {
			if (current_box != nullptr && !current_box->is_dummy) {
				std::vector<Box*> l_box{current_box->linked_boxes(dir)};
				for (Box* b : l_box) {
					if (!has_been_processed(b, processed_boxes)) {
						box_stack.push(b);
					}
				}
			}

			processed_boxes.insert(const_cast<const Box*>(current_box));
			mv_boxes.emplace_back(current_box);
		}
	}

	if (is_blocked) {
		return {};
	}

	return mv_boxes;
}

void aoc24_15::Warehouse::move_boxes(
    std::vector<Box*> boxes, aoc24::Direction dir)
{
	aoc24::Vec2d move_v = aoc24::dir_to_offset(dir);
	for (Box* box : boxes) {
		auto linked = box->linked_boxes();
		for (Box* lb : linked) {
			lb->unlink_box(box);
		}
		box->position.x += move_v.x;
		box->position.y += move_v.y;
	}

	for (Box* box : boxes) {
		std::vector<Edge_tile_instruction> instr{box->update_adj()};
		for (const auto& i : instr) {
			i.box->update_edge_tile(i.tile, box);
		}
	}
}

std::vector<const aoc24_15::Box*> aoc24_15::Warehouse::find_boxes_in_rect(
    int top_L_x, int top_L_y, int w, int h) const
{
	std::vector<const Box*> boxes;
	for (size_t i = 0; i < boxes_.size(); ++i) {
		const auto& box = *boxes_[i].get();
		if (box.intersects_with_rect(top_L_x, top_L_y, w, h)) {
			boxes.emplace_back(boxes_[i].get());
		}
	}
	return boxes;
}

void aoc24_15::Warehouse::spawn_robot(int pos_x, int pos_y)
{
	using aoc24::xytoa;
	if (!valid_xy(pos_x, pos_y)) {
		throw std::invalid_argument("Spawn position " + xytoa(pos_x, pos_y)
		                            + " is OOB");
	}
	const size_t pos = static_cast<size_t>(pos_y * sz + pos_x);
	if (walls_map().test(pos) || stuck_map().test(pos)) {
		throw std::logic_error(
		    "Cannot spawn at " + xytoa(pos_x, pos_y)
		    + ", that tile already contains a wall / stuck box");
	}
	if (collision->pt_has_box(pos_x, pos_y)) {
		throw std::logic_error("Cannot spawn at " + xytoa(pos_x, pos_y)
		                       + ", that tile already contains a box");
	}
	robot_.spawn(pos_x, pos_y);
}

void aoc24_15::Warehouse::move_robot(aoc24::Direction move_dir)
{
	if (!robot_.has_spawned()) {
		throw std::logic_error("Robot cannot move before it has been spawned");
	}

	++num_moves_; // Register move regardless of outcome

	using aoc24::Direction;
	using aoc24::Vec2d;
	Vec2d robot_start_xy{robot_.position()};
	Vec2d move_v{aoc24::dir_to_offset(move_dir)};
	Vec2d robot_end_xy{robot_start_xy + move_v};
	size_t robot_end_pos{
	    static_cast<size_t>(robot_end_xy.y * sz + robot_end_xy.x)};

	if (!valid_xy(robot_end_xy.x, robot_end_xy.y)
	    || collision->wall_or_stuck_box_at(robot_end_pos)) {
		/*
		    ABORT - Target tile is out of bounds or a stationary object */
		return;
	}

	auto start_box = find_box_at(robot_end_xy.x, robot_end_xy.y);

	if (start_box->is_dummy) {
		/*
		    MOVE and EXIT - Target tile is empty */
		robot_.move(move_dir);
		return;
	}

	// Check for movable boxes
	std::vector<Box*> mv_boxes{movable_adj_boxes_in_dir(*start_box, move_dir)};

	if (mv_boxes.empty()) {
		/*
		    ABORT - At least one of the movable boxes are blocked by an
		            out of bounds tile or a stationary object */
		return;
	}

	// Move robot and boxes
	robot_.move(move_dir);
	move_boxes(mv_boxes, move_dir);

	// Check for any stuck boxes
	for (size_t i = 0; i < mv_boxes.size(); ++i) {
		Box& box = *mv_boxes[i];
		if (box.is_stuck()) {
			box.prune_ = true;
			prune_stuck_boxes();
			break;
		}
	}
}

void aoc24_15::Warehouse::add_box(Box_factory& f, int pos_x, int pos_y)
{
	boxes_.emplace_back(f.create_box(*this, pos_x, pos_y));
	Box& box = *boxes_.back().get();
	std::vector<Edge_tile_instruction> instr{box.update_adj()};
	for (const auto& i : instr) {
		i.box->update_edge_tile(i.tile, &box);
	}
	if (box.is_stuck()) {
		box.prune_ = true;
		prune_stuck_boxes();
	}
}

void aoc24_15::Warehouse::print_map(std::ostream& ostr) const
{
	// Mark boxes
	print_->reset();
	for (const auto& b : boxes_) {
		const auto& box = *b.get();
		add_box_to_print_map(box);
	}

	size_t i = 0;
	for (size_t y = 0; y < sz_y; ++y) {
		for (size_t x = 0; x < sz; ++x) {
			char c = ' '; // Empty by default
			if (print_->test(i)) {
				c = chars::box_ch;
			} else if (walls_map().test(i)) {
				c = chars::wall_ch;
			} else if (stuck_map().test(i)) {
				c = chars::box_stuck_ch;
			} else if (x == robot_.position().x && y == robot_.position().y) {
				c = chars::robot_ch;
			}
			ostr << (x == 0 ? "" : " ") << c;
			++i;
		}
		ostr << '\n';
	}
}

aoc24_15::Box_integrity aoc24_15::Warehouse::box_integrity() const
{
	// Mark boxes
	print_->reset();
	for (size_t i = 0; i < boxes_.size(); ++i) {
		const Box& box = *boxes_[i].get();
		add_box_to_print_map(box);
	}

	// Check links
	for (size_t i = 0; i < boxes_.size(); ++i) {
		const Box& box = *boxes_[i].get();
		std::vector<Edge_tile> tiles = box.edge_tiles();
		for (const auto& t : tiles) {
			const Box* linked_box_ptr = box.linked_box(t);
			if (linked_box_ptr == nullptr) {
				return {box, t.position, "Box or Dummy box", "nullptr"};
			}
			const Box& linked_box = *linked_box_ptr;
			bool tile_is_oob = !valid_xy(t.position.x, t.position.y);
			size_t tile_pos = t.position.y * sz + t.position.x;
			if (linked_box.is_dummy) {
				if (!tile_is_oob && print_->test(tile_pos)) {
					return {box, t.position, "Box", "Dummy box"};
				}
			} else {
				if (tile_is_oob || !print_->test(tile_pos)
				    || collision->wall_or_stuck_box_at(tile_pos)) {
					return {box, t.position, "Dummy box", "Box"};
				}
			}
		}
	}
	return {};
}

long long aoc24_15::Warehouse::sum_of_all_box_coordinates() const
{
	constexpr int top_dist_f{100};

	long long sum = 0;
	for (size_t i = 0; i < boxes_.size(); ++i) {
		const Box& box = *boxes_[i].get();
		sum += top_dist_f * box.position.y + box.position.x;
	}
	for (const aoc24::Vec2d& stuck : stuck_box_positions_) {
		sum += top_dist_f * stuck.y + stuck.x;
	}
	return sum;
}

void aoc24_15::Warehouse::throw_if_no_map() const
{
	if (!map_ptr_init_) {
		throw std::runtime_error(
		    "Cannot access map: Maps have not been initialized");
	}
}

const aoc24_15::Box* aoc24_15::Warehouse::find_box_at(
    int top_L_x, int top_L_y) const
{
	std::vector<const Box*> boxes;
	for (size_t i = 0; i < boxes_.size(); ++i) {
		const auto& box = *boxes_[i].get();
		if (box.contains_pt(top_L_x, top_L_y)) {
			return &box;
		}
	}
	return &Box::Dummy;
}

aoc24_15::Box* aoc24_15::Warehouse::find_mutable_box_at(
    int top_L_x, int top_L_y) const
{
	auto match = find_box_at(top_L_x, top_L_y);
	if (!match->is_dummy) {
		// Cast away constness
		return const_cast<Box*>(match);
	}
	return &Box::Dummy;
}

aoc24_15::Box_integrity::Box_integrity(
    const Box& box,
    const aoc24::Vec2d& tile_pos,
    const std::string& expected,
    const std::string& found)
    : has_errors(true)
    , faulty_box_initial_position(box.initial_position)
    , faulty_box_position(box.position)
    , faulty_edge_tile_position(tile_pos)
    , expected_what(expected)
    , found_what(found)
{}
