#ifndef EXTRACT_H
#define EXTRACT_H
#include <string>
#include <vector>

namespace aoc24 {

std::vector<std::string> read_input_into_grid(const std::string& file_path,
                                              size_t string_size);
std::vector<std::string> read_input_into_grid(const std::string& file_path,
                                              size_t width,
                                              size_t height);
} // namespace aoc24

#endif // EXTRACT_H
