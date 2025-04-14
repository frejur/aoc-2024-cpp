#ifndef IO_H
#define IO_H
#include <string>
#include <vector>

namespace aoc24 {
struct Vec2d; // Forward decl.
}

namespace aoc24_18 {

std::vector<aoc24::Vec2d> read_coordinates_from_file(
    const std::string& filepath, const size_t number_of_entries);

}

#endif // IO_H
