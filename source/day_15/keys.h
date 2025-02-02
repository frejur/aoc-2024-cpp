#ifndef KEYS_H
#define KEYS_H
#include <string>

namespace aoc24_15 {

namespace chars {
constexpr char empty_ch{'.'};
constexpr char wall_ch{'#'};
constexpr char robot_ch{'@'};
constexpr char box_ch{'O'};
constexpr char box_stuck_ch{'X'};
constexpr char box_stuck_L_ch{'>'};
constexpr char box_stuck_R_ch{'<'};
constexpr char box_movable_L_ch{'['};
constexpr char box_movable_R_ch{']'};
} // namespace chars

namespace keys {
extern const std::string wall_key;
extern const std::string stuck_key;
extern const std::string movable_key;

// Part two
extern const std::string stuck_left_key;
extern const std::string stuck_right_key;
extern const std::string movable_left_key;
extern const std::string movable_right_key;
} // namespace keys

} // namespace aoc24_15
#endif // KEYS_H
