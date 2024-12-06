#include "day_05.h"
#include <algorithm>

void aoc24_05::Page_order::Update::validate(const Ruleset& rules)
{
	for (size_t i = 0; i < pages.size() - 1; ++i) {
		int page = pages[i];
		for (size_t j = i + 1; j < pages.size(); ++j) {
			if (rules.x_must_preceed_y(pages[j], page)) {
				s_ = Status::Incorrect;
				return;
			}
		}
	}
	s_ = Status::Correct;
}

int aoc24_05::Page_order::Update::middle() const
{
	if (pages.empty()) {
		return 0;
	}
	return pages[pages.size() / 2];
}

void aoc24_05::Page_order::Update::sort(const Ruleset& rules)
{
	std::sort(pages.begin(), pages.end(), [&rules](int a, int b) {
		return rules.x_must_preceed_y(a, b);
	});
}

//------------------------------------------------------------------------------

std::istream& aoc24_05::Page_order::operator>>(std::istream& is,
                                               aoc24_05::Page_order::Update& upd)
{
	std::vector<int> pages;

	bool reset_skipws = (is.flags() & std::ios::skipws);

	char c;
	if (!(is >> std::skipws >> c)) {
		return is;
	} else {
		is.putback(c);
	}

	is >> std::noskipws;

	int p = 0;
	char comma{};
	while (is >> p) {
		pages.push_back(p);

		if (is >> comma) {
			if (comma == ',') {
				continue;
			}
			if (isspace(comma)) {
				is.putback(comma);
			}
		} else if (is.eof()) {
			is.clear(std::ios_base::goodbit);
		}
		break;
	}

	if (is) {
		upd.pages = std::move(pages);
	}

	if (reset_skipws) {
		is >> std::skipws;
	}
	return is;
}

//------------------------------------------------------------------------------

std::istream& aoc24_05::Page_order::operator>>(std::istream& is,
                                               aoc24_05::Page_order::Rule& r)
{
	char pipe{};
	if (!(is >> r.left_page)) {
		return is;
	}
	if (!(is >> pipe) || pipe != '|') {
		if (is) {
			is.putback(pipe);
			is.clear(std::ios_base::failbit);
		}
		return is;
	}

	return is >> r.right_page;
}

//------------------------------------------------------------------------------

void aoc24_05::Page_order::Ruleset::reserve(size_t sz)
{
	must_come_after_map.reserve(sz);
}

void aoc24_05::Page_order::Ruleset::add(int l, int r)
{
	must_come_after_map.emplace(r, l); // Group by right value
}

bool aoc24_05::Page_order::Ruleset::x_must_preceed_y(int x, int y) const
{
	auto range = must_come_after_map.equal_range(y);
	for (auto i = range.first; i != range.second; ++i) {
		if (i->second == x) {
			return true;
		}
	}
	return false;
}

//------------------------------------------------------------------------------

void aoc24_05::validate(std::vector<Page_order::Update>& updates,
                        const Page_order::Ruleset& rules)
{
	for (auto& upd : updates) {
		upd.validate(rules);
	}
}

long aoc24_05::sum_of_correct_middle_page_numbers(
    std::vector<Page_order::Update>& updates)
{
	long int sum = 0;
	for (auto& upd : updates) {
		if (upd.is_correct()) {
			sum += upd.middle();
		}
	}
	return sum;
}

//------------------------------------------------------------------------------
// Part 2

void aoc24_05::sort(std::vector<Page_order::Update>& updates,
                    const Page_order::Ruleset& rules)
{
	for (auto& upd : updates) {
		upd.sort(rules);
	}
}
