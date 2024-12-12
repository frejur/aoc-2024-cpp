#include "day_06.h"
std::string aoc24_06::map_key_from_direction(const Direction dir,
                                             const bool future)
{
	std::string key;
	switch (dir) {
	case Direction::North:
		key = Trail_grid::up_key();
		break;
	case Direction::East:
		key = Trail_grid::right_key();
		break;
	case Direction::South:
		key = Trail_grid::down_key();
		break;
	case Direction::West:
		key = Trail_grid::left_key();
		break;
	}
	return key + (future ? Trail_grid::future_key_suffix() : "");
}

bool aoc24_06::tile_is_empty(const Trail_grid& grid,
                             const aoc24::XY& tile_position)
{
	for (int i = 0; i < 4; ++i) {
		if (grid.bit_at(map_key_from_direction(static_cast<Direction>(i)),
		                tile_position.x,
		                tile_position.y)) {
			return false;
		}
	}
	return true;
}

//------------------------------------------------------------------------------

void aoc24_06::find_obstruction_or_same_dir(Move& move,
                                            const Guard& guard,
                                            const Trail_grid& grid)
{
	if (guard.is_from_the_future()) {
		// Pre-check
		if (move.type == Move_type::Unknown) {
			find_obstruction(move, guard, grid);
			return;
		}

		if (move.type == Move_type::Leave) {
			// No obstruction found during pre-check => Abort
			return;
		}

		if (move.type != Move_type::First_obstruction) {
			// Find potential obstruction if not already done during pre-check
			find_obstruction(move, guard, grid);
			if (move.type == Move_type::Leave) {
				// No obstruction found
				return;
			}
		}
	} else {
		find_obstruction(move, guard, grid);
	}

	// At this point, `move` contains the coordinates of an obstruction.
	// Search for existing tiles marked with the same direction as the guard
	// and see which one is the closest

	Pos2d start_pos = guard.current_position();
	Pos2d offs{dir_to_xy(guard.current_direction())};
	const aoc24::XY start_xy = pos_to_xy(grid, start_pos);
	const std::string& map_key
	    = map_key_from_direction(guard.current_direction(),
	                             guard.is_from_the_future());
	const aoc24::XY same_dir_xy
	    = grid.find_bit(map_key, 1, start_xy.x, start_xy.y, offs.x, offs.y);

	if (same_dir_xy == aoc24::XY::oob
	    || (steps_between(start_pos, move.end_position)
	        < steps_between(start_pos, xy_to_pos(same_dir_xy)))) {
		// No tile found, or obstacle is closer
		move.type = Move_type::Obstruction;
		return;
	}

	move.type = Move_type::Loop;
	move.end_position = xy_to_pos(same_dir_xy);
}

//------------------------------------------------------------------------------

void aoc24_06::mark_start_dir(Move& move, Guard& guard, Trail_grid& grid)
{
	grid.mark_tile(pos_to_xy(grid, guard.current_position()),
	               map_key_from_direction(guard.current_direction(),
	                                      guard.is_from_the_future()));
}

void aoc24_06::mark_start_dir_and_setup_maps(Move& move,
                                             Guard& guard,
                                             Trail_grid& grid)
{
	if (move.type == Move_type::Leave) {
		// No obstruction found during pre-check => Abort
		return;
	}

	// Reset all directional future maps
	for (int i = 0; i < 4; ++i) {
		grid.reset(map_key_from_direction(static_cast<Direction>(i), true));
	}

	// Update future map
	mark_start_dir(move, guard, grid);
}

//------------------------------------------------------------------------------

bool aoc24_06::same_dir_or_no_obstruction_ahead(const Move& move)
{
	return (move.type == Move_type::Loop || move.type == Move_type::Leave);
}

//------------------------------------------------------------------------------

void aoc24_06::mark_dir_trail(Move& move, Guard& guard, Trail_grid& grid)
{
	if (guard.is_from_the_future()
	    && (move.type == Move_type::Leave || move.type == Move_type::Loop)) {
		// Finishing a future move, no need to mark anything
		return;
	}
	Direction dir = (move.type == Move_type::Obstruction
	                 || move.type == Move_type::First_obstruction)
	                    ? guard.direction_to_the_left()
	                    : guard.current_direction();
	const std::string& key = map_key_from_direction(dir,
	                                                guard.is_from_the_future());
	grid.mark_tiles(pos_to_xy(grid, move.start_position),
	                pos_to_xy(grid, move.end_position),
	                key);
}

//------------------------------------------------------------------------------

void aoc24_06::move_to_limit_or_loop_start(Move& move,
                                           Guard& guard,
                                           Trail_grid& grid)
{
	if (move.type == Move_type::Leave) {
		move_to_end(move, guard, grid);
	} else {
		if (guard.current_position() != move.end_position) {
			guard.set_position(move.end_position);
		}
	}
}

//------------------------------------------------------------------------------

void aoc24_06::step_and_check_for_loops(Move& move,
                                        Guard& guard,
                                        Trail_grid& grid)
{
	int distance{steps_between(guard.current_position(), move.end_position)};
	// Step towards target, spawning future guards that check for loops
	for (size_t i = 0; i < distance; ++i) {
		aoc24::XY new_obstr_xy = pos_to_xy(grid, guard.simulated_position(1));
		if (tile_is_empty(grid, new_obstr_xy)) {
			// Temporarily mark new obstructure
			grid.mark_tile(new_obstr_xy, Trail_grid::obstruction_key());

			// Spawn future guard
			Pos2d fut_pos{guard.current_position()};
			Guard fut_guard(fut_pos);
			fut_guard.transport_to_future();
			fut_guard.set_direction(guard.current_direction());
			fut_guard.turn_right();

			// Move future guard until out of bounds or looping
			Move fut_move{fut_pos.x, fut_pos.y};
			move_and_mark_until_condition(fut_move,
			                              fut_guard,
			                              grid,
			                              find_obstruction_or_same_dir,
			                              mark_start_dir_and_setup_maps,
			                              find_obstruction_or_same_dir,
			                              same_dir_or_no_obstruction_ahead,
			                              move_to_obstruction_and_turn,
			                              move_to_limit_or_loop_start,
			                              mark_dir_trail);

			// Unmark new obstructure, save in New Obstructure map if in a loop
			grid.mark_tile(new_obstr_xy, Trail_grid::obstruction_key(), false);
			if (fut_move.type == Move_type::Loop) {
				grid.mark_tile(new_obstr_xy, Trail_grid::new_obstruction_key());
			}
		}
		guard.move_forward();
	}
	guard.turn_right();
}
