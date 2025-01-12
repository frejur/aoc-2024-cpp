#ifndef KEYS_H
#define KEYS_H
#include <string>

namespace aoc24_15 {

namespace chars {
constexpr char empty_ch{'.'};
constexpr char wall_ch{'#'};
constexpr char robot_ch{'@'};
constexpr char box_ch{'O'};
} // namespace chars

namespace keys {
extern const std::string wall_key;
extern const std::string stuck_key;
extern const std::string movable_key;
} // namespace keys

} // namespace aoc24_15
#endif // KEYS_H
