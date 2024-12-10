#include "day_06.h"
#include <sstream>
#include <stdexcept>
namespace {
constexpr char obstacle{'#'};
const std::string& obstacle_key()
{
	static const std::string k{"Obstacles"};
	return k;
}
const std::string& trail_key()
{
	static const std::string k{"Trail"};
	return k;
}
} // namespace

aoc24_06::Pos2d aoc24_06::dir_to_xy(Direction dir)
{
	using D = Direction;
	int offs_x{}, offs_y{};
	offs_x = dir == D::East ? 1 : (dir == D::West ? -1 : 0);
	offs_y = dir == D::South ? 1 : (dir == D::North ? -1 : 0);
	return {offs_x, offs_y};
}

aoc24_06::Guard::Guard(Pos2d position)
    : pos(position)
    , dir(Direction::North)
{}

aoc24_06::Pos2d aoc24_06::Guard::current_position() const
{
	return pos;
}

aoc24_06::Direction aoc24_06::Guard::current_direction() const
{
	return dir;
}
void aoc24_06::Guard::move_forward(int steps)
{
	if (steps < 1) {
		throw std::invalid_argument("Parameter steps must > 0");
	}
	Pos2d offs{dir_to_xy(dir)};
	pos.x += offs.x * steps;
	pos.y += offs.y * steps;
}
void aoc24_06::Guard::turn_right()
{
	dir = static_cast<Direction>((static_cast<int>(dir) + 1) % 4);
}

//------------------------------------------------------------------------------

aoc24_06::Trail_grid::Trail_grid(const aoc24::Char_grid& reference_grid,
                                 std::unique_ptr<aoc24::Dyn_bitset> dyn_bitset)
    : Bit_grid(reference_grid.size(), obstacle_key(), std::move(dyn_bitset))
{
	aoc24::XY xy{0, 0};
	while (xy = reference_grid.find_char(obstacle, xy.x, xy.y),
	       xy != aoc24::XY::oob) {
		set(xy);
		reference_grid.next(xy);
	}
	add_map(trail_key());
}

void aoc24_06::Trail_grid::mark_tile(aoc24::XY pos)
{
	set(trail_key(), pos);
}

void aoc24_06::Trail_grid::mark_trail(aoc24::XY a, aoc24::XY b)
{
	if (a.y == b.y) {
		// Horizontal
		int start = static_cast<int>((std::min)(a.x, b.x));
		int end = static_cast<int>((std::max)(a.x, b.x));
		mask_row(a.y, start, sz - end - 1);
	} else if (a.x == b.x) {
		// Vertical
		int start = static_cast<int>((std::min)(a.y, b.y));
		int end = static_cast<int>((std::max)(a.y, b.y));
		mask_col(a.x, start, sz - end - 1);
	} else {
		throw std::invalid_argument("Only horizontal / vertical paths allowed");
	}
	apply_mask(trail_key());
}

void aoc24_06::Trail_grid::print_trail(std::ostream& ostr)
{
	print(trail_key());
}

int aoc24_06::Trail_grid::number_of_marked_tiles() const
{
	return count(trail_key());
}

aoc24_06::Guard aoc24_06::spawn_guard(const aoc24::Char_grid& grid)
{
	aoc24::XY pos{grid.find_char('^', 0, 0)};
	if (pos == aoc24::XY::oob) {
		throw std::runtime_error("Could not find guard starting position");
	}
	return Guard({static_cast<int>(pos.x), static_cast<int>(pos.y)});
}

int aoc24_06::move_to_limit(Guard& guard, const Trail_grid& grid)
{
	Pos2d pos{guard.current_position()};
	int size{static_cast<int>(grid.size())};

	if (pos.x < 0 || pos.y < 0
	    || grid.is_oob(
	        {static_cast<size_t>(pos.x), static_cast<size_t>(pos.y)})) {
		return 0;
	}

	int steps = 0;
	switch (guard.current_direction()) {
	case Direction::North:
		steps = guard.current_position().y;
		break;
	case Direction::East:
		steps = size - guard.current_position().x - 1;
		break;
	case Direction::South:
		steps = size - guard.current_position().y - 1;
		break;
	case Direction::West:
		steps = guard.current_position().x;
		break;
	}

	guard.move_forward(steps);

	return steps;
}

void aoc24_06::move_and_mark_until_oob(Guard& guard, Trail_grid& grid)
{
	Pos2d start_pos{guard.current_position()};
	grid.mark_tile(
	    {static_cast<size_t>(start_pos.x), static_cast<size_t>(start_pos.y)});

	bool is_in = true;
	while (is_in) {
		auto c_szt = [](int i) { return static_cast<size_t>(i); };

		start_pos = guard.current_position();
		Pos2d offs{dir_to_xy(guard.current_direction())};
		const aoc24::XY start_xy = {c_szt(start_pos.x), c_szt(start_pos.y)};
		const aoc24::XY obstacle_xy = grid.find_bit(obstacle_key(),
		                                            1,
		                                            start_xy.x,
		                                            start_xy.y,
		                                            offs.x,
		                                            offs.y);
		aoc24::XY final_guard_xy = {0, 0};

		if (obstacle_xy == aoc24::XY::oob) {
			// No obstacle in the way, move to boundary limit
			move_to_limit(guard, grid);

			Pos2d final{guard.current_position()};
			final_guard_xy = {c_szt(final.x), c_szt(final.y)};
			guard.move_forward(); // Move one step past bounds
			is_in = false;
		} else {
			// Obstacle, move to tile before it, then turn right
			guard.move_forward(steps_between(start_xy, obstacle_xy) - 1);
			Pos2d final{guard.current_position()};
			final_guard_xy = {c_szt(final.x), c_szt(final.y)};
			guard.turn_right();
		}

		grid.mark_trail(start_xy, final_guard_xy);
	}
}

int aoc24_06::steps_between(const aoc24::XY& a, const aoc24::XY& b)
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
