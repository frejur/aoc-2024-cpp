#include "day_06.h"
#include <stdexcept>

int aoc24_06::steps_between(const Pos2d& a, const Pos2d& b)
{
	int s = 0;
	if (a.y == b.y) {
		s = std::abs(static_cast<int>(a.x) - static_cast<int>(b.x));
	} else if (a.x == b.x) {
		s = std::abs(static_cast<int>(a.y) - static_cast<int>(b.y));
	} else {
		throw std::invalid_argument("Only horizontal / vertical paths allowed");
	}
	return s;
}

aoc24_06::Guard aoc24_06::spawn_guard(const aoc24::Char_grid& grid)
{
	aoc24::XY pos{grid.find_char('^', 0, 0)};
	if (pos == aoc24::XY::oob) {
		throw std::runtime_error("Could not find guard starting position");
	}
	return Guard({static_cast<int>(pos.x), static_cast<int>(pos.y)});
}

aoc24::XY aoc24_06::pos_to_xy(const aoc24::Grid& grid,
                              const Pos2d& pos,
                              const bool skip_check)
{
	if (!skip_check && grid.is_oob(pos.x, pos.y)) {
		throw std::invalid_argument("Cannot convert x=" + std::to_string(pos.x)
		                            + " and y=" + std::to_string(pos.y)
		                            + " to a valid XY of map with size "
		                            + std::to_string(grid.size()));
	}
	return {static_cast<size_t>(pos.x), static_cast<size_t>(pos.y)};
}

aoc24_06::Pos2d aoc24_06::xy_to_pos(const aoc24::XY& pos, const bool skip_check)
{
	return {static_cast<int>(pos.x), static_cast<int>(pos.y)};
}

//------------------------------------------------------------------------------

void aoc24_06::move_and_mark_until_condition(Move& move,
                                             Guard& guard,
                                             Trail_grid& grid,
                                             Read_G_Gr_fn pre_check_fn,
                                             Edit_G_Gr_fn initial_action_fn,
                                             Read_G_Gr_fn find_tile_fn,
                                             Test_Move_fn stop_condition_fn,
                                             Edit_G_Gr_fn move_fn,
                                             Edit_G_Gr_fn stop_fn,
                                             Edit_G_Gr_fn mark_fn)
{
	if (stop_condition_fn == skip_function) {
		throw std::invalid_argument(
		    "The stop condition function cannot be skipped");
	}
	if (move.type != Move_type::Unknown) {
		throw std::invalid_argument(
		    "Expected an initial Move object of type Unknown(0) but got "
		    + std::to_string(static_cast<int>(move.type)));
	}
	if (move.start_position != move.end_position) {
		throw std::invalid_argument(
		    "Expected the start and end positions of the intial Move object to "
		    "be identical but got ("
		    + std::to_string(move.start_position.x) + ", "
		    + std::to_string(move.start_position.y) + ") vs ("
		    + std::to_string(move.end_position.x) + ", "
		    + std::to_string(move.end_position.y) + ")");
	}

	if (pre_check_fn != skip_function) {
		pre_check_fn(move, guard, grid);
	}

	if (initial_action_fn != skip_function) {
		initial_action_fn(move, guard, grid);
	}

	bool stop = false;
	while (!stop) {
		if (find_tile_fn != skip_function) {
			find_tile_fn(move, guard, grid);
		}

		if (!stop_condition_fn(move)) {
			if (move_fn != skip_function) {
				move_fn(move, guard, grid);
			}
		} else {
			if (stop_fn != skip_function) {
				stop_fn(move, guard, grid);
			}
			stop = true;
		}

		if (mark_fn != skip_function) {
			mark_fn(move, guard, grid);
		}

		if (!stop) {
			move.start_position = move.end_position;
		}
	}
}

//------------------------------------------------------------------------------

void aoc24_06::move_to_end(Move& move, Guard& guard, Trail_grid& grid)
{
	const Pos2d& pos{guard.current_position()};
	if (grid.is_oob(pos.x, pos.y)) {
		return;
	}
	guard.set_position(move.end_position);
}

void aoc24_06::move_to_obstruction_and_turn(Move& move,
                                            Guard& guard,
                                            Trail_grid& grid)
{
	if (guard.current_position() == move.end_position) {
		// Already at obstruction
		guard.turn_right();
	} else {
		// Obstruction, move to tile before it, then turn right
		guard.set_position(move.end_position);
		guard.turn_right();
	}
}

//------------------------------------------------------------------------------

void aoc24_06::mark_start(Move& move, Guard& guard, Trail_grid& grid)
{
	// Move remains unchanged
	grid.mark_tile(pos_to_xy(grid, guard.current_position()));
}

//------------------------------------------------------------------------------

void aoc24_06::find_obstruction(Move& move,
                                const Guard& guard,
                                const Trail_grid& grid)
{
	if (move.type == Move_type::First_obstruction) {
		// Already taken care of by pre-check function
		move.type = Move_type::Obstruction;
		return;
	}
	Pos2d start_pos = guard.current_position();
	Pos2d offs{dir_to_xy(guard.current_direction())};
	const aoc24::XY start_xy = pos_to_xy(grid, start_pos);
	const aoc24::XY obstruction_xy = grid.find_bit(Trail_grid::obstruction_key(),
	                                               1,
	                                               start_xy.x,
	                                               start_xy.y,
	                                               offs.x,
	                                               offs.y);

	if (obstruction_xy == aoc24::XY::oob) {
		set_oob_move(move, offs.x, offs.y, grid.size());
	} else {
		move.type = (move.type == Move_type::Unknown)
		                ? Move_type::First_obstruction
		                : Move_type::Obstruction;
		move.end_position = {static_cast<int>(obstruction_xy.x - offs.x),
		                     static_cast<int>(obstruction_xy.y - offs.y)};
	}
}
void aoc24_06::set_oob_move(Move& move,
                            const int dir_offs_x,
                            const int dir_offs_y,
                            const int size)
{
	move.type = Move_type::Leave;
	move.end_position.x = (dir_offs_x != 0)
	                          ? ((dir_offs_x == -1) ? 0 : size - 1)
	                          : move.end_position.x;
	move.end_position.y = (dir_offs_y != 0)
	                          ? ((dir_offs_y == -1) ? 0 : size - 1)
	                          : move.end_position.y;
}

//------------------------------------------------------------------------------

bool aoc24_06::no_obstruction_ahead(const Move& move)
{
	return move.type == Move_type::Leave;
}

//------------------------------------------------------------------------------

void aoc24_06::mark_trail(Move& move, Guard& guard, Trail_grid& grid)
{
	grid.mark_tiles(pos_to_xy(grid, move.start_position),
	                pos_to_xy(grid, move.end_position));
}
