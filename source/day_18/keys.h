#ifndef KEYS_H
#define KEYS_H
#include <string>

namespace aoc24_18 {

namespace chars {
constexpr char path_char{'O'};
constexpr char empty_char{'.'};
constexpr char byte_char{'#'};
constexpr char start_char{'S'};
constexpr char end_char{'E'};
constexpr char generic_fork_char{'+'};
constexpr char fork_URDL_char{'+'};
constexpr char fork_URD_char{'}'};
constexpr char fork_RDL_char{'v'};
constexpr char fork_URL_char{'^'};
constexpr char fork_UDL_char{'{'};
constexpr char corner_UR_char{'L'};
constexpr char corner_RD_char{'r'};
constexpr char corner_DL_char{'q'};
constexpr char corner_UL_char{'J'};
constexpr char generic_u_turn_char{'U'};
constexpr char u_turn_U_char{'n'};
constexpr char u_turn_R_char{')'};
constexpr char u_turn_D_char{'u'};
constexpr char u_turn_L_char{'('};
constexpr char passage_UD_char{'|'};
constexpr char passage_RL_char{'-'};
constexpr char dead_end_char{'X'};
} // namespace chars

namespace keys {
extern const std::string dummy_key;
extern const std::string byte_key;
extern const std::string fork_URDL_key;
extern const std::string fork_URD_key;
extern const std::string fork_RDL_key;
extern const std::string fork_URL_key;
extern const std::string fork_UDL_key;
extern const std::string corner_UR_key;
extern const std::string corner_RD_key;
extern const std::string corner_DL_key;
extern const std::string corner_UL_key;
extern const std::string u_turn_U_key;
extern const std::string u_turn_R_key;
extern const std::string u_turn_D_key;
extern const std::string u_turn_L_key;
extern const std::string passage_UD_key;
extern const std::string passage_RL_key;
extern const std::string dead_end_key;
extern const std::string fork_corner_u_turn_key;
extern const std::string all_passages_key;
extern const std::string shortest_path_key;

} // namespace keys

} // namespace aoc24_18
#endif // KEYS_H
