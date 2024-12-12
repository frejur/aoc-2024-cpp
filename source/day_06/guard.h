#ifndef GUARD_H
#define GUARD_H

namespace aoc24_06 {

enum class Direction {
	North = 0,
	East,
	South,
	West,
};
struct Pos2d
{
	int x;
	int y;
	bool operator==(const Pos2d& other)
	{
		return (x == other.x && y == other.y);
	};
	bool operator!=(const Pos2d& other) { return !(*this == other); };
};

Pos2d dir_to_xy(Direction dir);

class Guard
{
public:
	Guard(Pos2d position);
	Pos2d current_position() const;
	Pos2d simulated_position(int steps = 1) const;
	Direction current_direction() const;

	void set_position(Pos2d position) { pos = position; };
	void set_direction(Direction direction) { dir = direction; };
	void move_forward(int steps = 1);
	void turn_right();
	void transport_to_future() { is_future = true; }
	void transport_to_present() { is_future = false; }

	Direction direction_to_the_left() const;
	bool is_from_the_future() const { return is_future; }

private:
	bool is_future;
	Pos2d pos;
	Direction dir;
};

} // namespace aoc24_06

#endif // GUARD_H
