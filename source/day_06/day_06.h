#ifndef DAY_06_H
#define DAY_06_H
#include "guard.h"
#include "trail_grid.h"

namespace aoc24_06 {

//------------------------------------------------------------------------------
// Part 1 (definitions in `part_01.cpp`)

Guard spawn_guard(const aoc24::Char_grid& grid);
int steps_between(const Pos2d& a, const Pos2d& b);
aoc24::XY pos_to_xy(const aoc24::Grid& grid,
                    const Pos2d& pos,
                    const bool skip_check = false);
Pos2d xy_to_pos(const aoc24::XY& pos, const bool skip_check = false);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

enum class Move_type { Unknown, First_obstruction, Obstruction, Leave, Loop };
struct Move
{
	Move(int x, int y)
	    : type(Move_type::Unknown)
	    , start_position({x, y})
	    , end_position({x, y}){};
	Move_type type;
	Pos2d start_position;
	Pos2d end_position;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

using Read_G_Gr_fn = void (*)(Move&, const Guard&, const Trail_grid&);
using Edit_G_Gr_fn = void (*)(Move&, Guard&, Trail_grid&);
using Test_Move_fn = bool (*)(const Move&);
constexpr auto skip_function{nullptr};

void move_and_mark_until_condition(Move& move,
                                   Guard& guard,
                                   Trail_grid& grid,
                                   Read_G_Gr_fn pre_check_fn,
                                   Edit_G_Gr_fn initial_action_fn,
                                   Read_G_Gr_fn find_tile_fn,
                                   Test_Move_fn stop_condition_fn,
                                   Edit_G_Gr_fn move_fn,
                                   Edit_G_Gr_fn stop_fn,
                                   Edit_G_Gr_fn mark_fn);

void set_oob_move(Move& move,
                  const int dir_offs_x,
                  const int dir_offs_y,
                  const int size);
void find_obstruction(Move& move, const Guard&, const Trail_grid&);
bool no_obstruction_ahead(const Move& move);
void mark_start(Move& move, Guard& guard, Trail_grid& grid);
void move_to_obstruction_and_turn(Move& move, Guard& guard, Trail_grid& grid);
void move_to_end(Move& move, Guard& guard, Trail_grid& grid);
void mark_trail(Move& m, Guard& g, Trail_grid& gr);

//------------------------------------------------------------------------------
// Part 2 (definitions in `part_02.cpp`)

std::string map_key_from_direction(const Direction dir,
                                   const bool future = false);
bool tile_is_empty(const Trail_grid& grid, const aoc24::XY& tile_position);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void find_obstruction_or_same_dir(Move& move, const Guard&, const Trail_grid&);
void mark_start_dir(Move& move, Guard& guard, Trail_grid& grid);
void mark_start_dir_and_setup_maps(Move& move, Guard& guard, Trail_grid& grid);
bool same_dir_or_no_obstruction_ahead(const Move& move);
void mark_dir_trail(Move& m, Guard& g, Trail_grid& gr);
void move_to_limit_or_loop_start(Move& move, Guard& guard, Trail_grid& grid);
void step_and_check_for_loops(Move& move, Guard& guard, Trail_grid& grid);

} // namespace aoc24_06

#endif // DAY_06_H
