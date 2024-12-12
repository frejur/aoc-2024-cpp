#include "puzzle.h"
#include <fstream>
#include <iomanip>
#include <sstream>

namespace {
constexpr int max_part{5};
constexpr int max_col{63};
constexpr int invalid_int{-99999};
int valid_day(const int day);
int valid_part(const int part);
std::string tst_path(const int day);
std::string inp_path(const int day);
std::string pad(const int num);
} // namespace

//------------------------------------------------------------------------------

aoc24::Puzzle::Puzzle(int day, const std::string& name, int argc, char* argv[])
    : use_test(false)
    , has_secret(false)
    , d(valid_day(day))
    , nm(name)
    , p_inp(inp_path(d))
    , p_tst(tst_path(d))
{
	if (argc == 1) {
		return;
	}

	if (argc > 2) {
		throw std::invalid_argument("Invalid number of arguments");
	}

	const std::string arg = argv[1];

	// Check for test mode
	if (arg == "-t") {
		use_test = true;
		has_secret = true;
		p_sec = "test/secret.txt";
		return;
	}

	// Check for SECRET value if present
	std::string secret;
	const std::string key = "-SECRET=";

	if (arg.find(key) == 0) {
		secret = arg.substr(key.size());
		strip_q(secret);
		has_secret = true;
		p_sec = secret;
	} else {
		throw std::invalid_argument("Invalid argument");
	}
}

//------------------------------------------------------------------------------

std::string aoc24::Puzzle::input_file_path() const
{
	if (use_test) {
		return p_tst;
	}
	return p_inp;
}

void aoc24::Puzzle::file_answer(int part,
                                const std::string& name,
                                long long value)
{
	const int val_part{valid_part(part)};
	if (ans_v.size() + 1 > max_part) {
		throw std::logic_error("Puzzle already has the max. number of answers: "
		                       + std::to_string(max_part));
	}
	if (val_part != ans_v.size() + 1) {
		throw std::logic_error("Cannot add part " + std::to_string(part)
		                       + " before part "
		                       + std::to_string(ans_v.size() + 1));
	}
	Answer::Source source = use_test ? Answer::Source::Test
	                                 : Answer::Source::Final;
	long long secret{get_int(part)};
	if (secret == invalid_int) {
		source = Answer::Source::Missing;
	}
	ans_v.emplace_back(Answer{d, part, source, name, value, secret});
}

void aoc24::Puzzle::file_answer(int part,
                                const std::string& name,
                                const std::string& value)
{
	const int val_part{valid_part(part)};
	if (ans_v.size() + 1 > max_part) {
		throw std::logic_error("Puzzle already has the max. number of answers: "
		                       + std::to_string(max_part));
	}
	if (val_part != ans_v.size() + 1) {
		throw std::logic_error("Cannot add part " + std::to_string(part)
		                       + " before part "
		                       + std::to_string(ans_v.size() + 1));
	}
	Answer::Source source = use_test ? Answer::Source::Test
	                                 : Answer::Source::Final;
	std::string secret{get(part)};
	if (secret.empty()) {
		source = Answer::Source::Missing;
	}
	ans_v.emplace_back(Answer{d, part, source, name, value, secret});
}

aoc24::Answer aoc24::Puzzle::answer(const int part)
{
	if (ans_v.empty()) {
		throw std::logic_error("There are no answers on file");
	}
	const int val_part{valid_part(part)};
	if (val_part > ans_v.size() - 1) {
		throw std::logic_error("No answer has been filed for part "
		                       + std::to_string(val_part));
	}
	return ans_v[val_part - 1];
}

//------------------------------------------------------------------------------

void aoc24::Puzzle::print_answers(std::ostream& os)
{
	if (ans_v.empty()) {
		throw std::logic_error("There are no answers on file");
	}

	std::ostringstream ostr;
	ostr << "Advent of Code 2024, day " << d << ':';
	ostr << std::setw(max_col - ostr.str().size() - nm.size() - 2) << std::right
	     << '"' << nm << '"' << '\n';
	os << ostr.str();
	os << std::setfill('=') << std::setw(max_col - 1) << "" << std::setfill(' ')
	   << '\n';

	int count = 0;
	for (const Answer& a : ans_v) {
		os << (count++ ? "\n\n" : "") << a;
	}
	os.flush();
}
//------------------------------------------------------------------------------

std::ostream& aoc24::operator<<(std::ostream& os, const aoc24::Answer& a)
{
	// Name(s)
	if (a.src != Answer::Source::Missing) {
		os << std::setw(max_col / 2) << std::left;
	}
	os << (a.nm + ":");

	if (a.src == Answer::Source::Test) {
		os << "Test answer:";
	} else if (a.src == Answer::Source::Final) {
		os << "Solution:";
	}
	os << '\n';

	// Value(s)
	if (a.src != Answer::Source::Missing) {
		os << std::setw(max_col / 2) << std::left;
	}
	if (a.tp == Answer::Type::Integer) {
		os << a.tent_int;
		if (a.src != Answer::Source::Missing) {
			os << a.defn_int;
		}
	} else {
		os << a.tent_str;
		if (a.src != Answer::Source::Missing) {
			os << a.defn_str;
		}
	}
	return os << '\n';
}
//------------------------------------------------------------------------------

std::string aoc24::Puzzle::get(const int part)
{
	if (!has_secret) {
		return "";
	}
	const std::string& file_path{p_sec};

	std::ifstream ifs{file_path};
	if (!ifs) {
		throw std::ios_base::failure("Could not open file: " + file_path);
	}

	const std::string id{"day_" + pad(d) + "_part_" + pad(part)};

	bool search_for_close = false;
	int num_ln = 0;
	constexpr int max_ln{128};
	std::string ln;
	std::string secret;
	const std::string open = "[[" + id + "]]";
	const std::string close = "[[/" + id + "]]";
	while (std::getline(ifs, ln)) {
		if (!search_for_close) {
			search_for_close = (ln == open);
		} else {
			if (ln != close) {
				if (++num_ln > 1) {
					secret += '\n';
				}
				secret += ln;
				if (num_ln > max_ln) {
					std::runtime_error("Answer contained too many lines: "
					                   + std::to_string(num_ln));
				}
			} else {
				search_for_close = false;
				break;
			}
		}
	}
	if (search_for_close) {
		std::runtime_error("Could not find closing tag: " + close);
	}
	return secret;
}

long long aoc24::Puzzle::get_int(const int part)
{
	std::string s{get(part)};
	if (s.empty()) {
		return invalid_int;
	}

	std::istringstream is{s};

	long long secret = 0;
	char c{};
	is >> std::noskipws;

	if (!(is >> secret) || is >> c) {
		throw std::logic_error("The answer found for day " + std::to_string(d)
		                       + " part " + std::to_string(part)
		                       + " is not a valid integer value");
	}

	return secret;
}

//------------------------------------------------------------------------------
aoc24::Answer::Answer(int day,
                      int part,
                      Source source,
                      const std::string& name,
                      long long tentative,
                      long long definite)
    : tp(Type::Integer)
    , src(source)
    , d(day)
    , pt(part)
    , tent_int(tentative)
    , defn_int(definite)
    , nm(name)
{}

aoc24::Answer::Answer(int day,
                      int part,
                      Source source,
                      const std::string& name,
                      const std::string& tentative,
                      const std::string& definite)
    : tp(Type::Integer)
    , src(source)
    , d(day)
    , pt(part)
    , tent_int(0)
    , defn_int(0)
    , nm(name)
    , tent_str(tentative)
    , defn_str(definite)
{}
//------------------------------------------------------------------------------

void aoc24::strip_q(std::string& s)
{
	if (s.size() >= 2 && s.front() == '"' && s.back() == '"') {
		s = s.substr(1, s.size() - 2);
	}
}
//------------------------------------------------------------------------------

namespace {
int valid_day(const int day)
{
	if (1 > day || day > 24) {
		throw std::invalid_argument("Day must be a value between 1 and 24");
	}
	return day;
}
int valid_part(const int part)
{
	if (1 > part || part > max_part) {
		throw std::invalid_argument("Part must be in the range: 1 to "
		                            + std::to_string(max_part));
	}
	return part;
}
std::string tst_path(const int day)
{
	return "test/202412" + pad(day) + "_test.txt";
}
std::string inp_path(const int day)
{
	return "input/202412" + pad(day) + "_input.txt";
}
std::string pad(const int num)
{
	std::ostringstream os;
	os << std::setw(2) << std::setfill('0') << num;
	return os.str();
}
} // namespace
