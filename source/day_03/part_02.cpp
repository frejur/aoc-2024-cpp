#include "day_03.h"
#include <cassert>
#include <sstream>
#include <vector>

namespace {
const std::string& enable_string()
{
	static const std::string s{"do()"};
	return s;
}
const std::string& disable_string()
{
	static const std::string s{"don't()"};
	return s;
}
constexpr char toggle_char{'d'};

static const bool check_lengths = []() {
	assert(enable_string().size() < disable_string().size());
	return true;
}();
} // namespace

std::istream& operator>>(std::istream& iss, aoc24_03::Toggle& tgl)
{
	std::streampos orig_pos{iss.tellg()};

	// Check for enable
	std::string buffer(enable_string().size(), '\0');
	if (iss.read(&buffer[0], enable_string().size())) {
		if (buffer == enable_string()) {
			tgl.enable = true;
			return iss;
		}
	}

	// Check for disable
	std::string rest(disable_string().size() - buffer.size(), '\0');
	if (iss.read(&rest[0], rest.size())) {
		if ((buffer + rest) == disable_string()) {
			tgl.enable = false;
			return iss;
		}
	}

	iss.clear();
	iss.seekg(orig_pos);
	iss.clear(std::ios_base::failbit);
	return iss;
}

void aoc24_03::disable_based_on_string(std::vector<Mul>& mul_v,
                                       const std::string& s)
{
	std::istringstream iss{s};

	bool enable = true;
	char c{};
	size_t dis_begin = 0;
	size_t dis_end = 0;
	size_t mul_idx = 0;
	for (size_t c_idx = 0; iss.get(c); ++c_idx) {
		if (c_idx == s.length() - 1 && !enable) {
			disable_based_on_pos(mul_v, mul_idx, dis_begin, c_idx);
			return;
		}
		if (c == toggle_char) {
			iss.putback(toggle_char);
			Toggle tgl;
			if (iss >> tgl) {
				if (tgl.enable && !enable) {
					dis_end = (c_idx == 0) ? 0 : c_idx - 1;
					mul_idx = disable_based_on_pos(mul_v,
					                               mul_idx,
					                               dis_begin,
					                               dis_end);
				}

				if (!tgl.enable && enable) {
					dis_begin = c_idx + disable_string().size();
				}

				c_idx += (tgl.enable) ? enable_string().size() - 1
				                      : disable_string().size() - 1;
				enable = tgl.enable;
			} else {
				iss.clear(std::ios_base::goodbit);
				iss.get(); // eat
			}
		}
	}
}

size_t aoc24_03::disable_based_on_pos(std::vector<Mul>& mul_v,
                                      const size_t mul_start,
                                      const size_t c_start,
                                      const size_t c_end)
{
	size_t i;
	for (i = mul_start; i < mul_v.size(); ++i) {
		if (mul_v[i].pos < c_start) {
			continue;
		}

		if (mul_v[i].pos <= c_end) {
			mul_v[i].enabled = false;
		} else {
			break;
		}
	}
	return mul_start + 1;
}
