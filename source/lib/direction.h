#ifndef DIRECTION_H
#define DIRECTION_H
#include <iostream>

namespace aoc24 {

struct Vec2d;
enum class Direction { Up = 0, Right, Down, Left };

std::ostream& operator<<(std::ostream& ostr, const Direction& dir);
std::istream& operator>>(std::istream& istr, Direction& dir);

Vec2d dir_to_offset(Direction dir, bool invert = false);
Direction dir_inverted(const Direction dir);
Direction dir_turn_left(const Direction dir);
Direction dir_turn_right(const Direction dir);

} // namespace aoc24

#endif // DIRECTION_H
