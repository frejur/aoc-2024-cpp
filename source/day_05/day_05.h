#ifndef DAY_05_H
#define DAY_05_H
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace aoc24_05 {

//------------------------------------------------------------------------------
// Part 1 (definitions in `part_01.cpp`)

namespace Page_order {
class Ruleset; // Fwd. decl.

class Update
{
public:
	Update()
	    : s_(Status::Not_validated)
	{}
	enum class Status { Not_validated, Correct, Incorrect };
	Status status() const { return s_; }
	bool is_correct() const { return s_ == Status::Correct; }
	void validate(const Ruleset& rules);
	int middle() const;
	void sort(const Ruleset& rules);

	friend std::istream& operator>>(std::istream& is, Update& upd);

private:
	Status s_;
	std::vector<int> pages;
};

struct Rule
{
	int left_page;
	int right_page;
};

std::istream& operator>>(std::istream& is, Rule& r);

class Ruleset
{
public:
	void reserve(size_t sz);
	void add(int left, int right);
	bool x_must_preceed_y(int x, int y) const;

private:
	std::unordered_multimap<int, int> must_come_after_map;
};

} // namespace Page_order

void read_input_into_rules_and_updates(const std::string& file_path,
                                       Page_order::Ruleset& rules,
                                       std::vector<Page_order::Update>& updates);

void validate(std::vector<Page_order::Update>& updates,
              const Page_order::Ruleset& rules);

long int sum_of_correct_middle_page_numbers(
    std::vector<Page_order::Update>& updates);

//------------------------------------------------------------------------------
// Part 2 (definitions in `part_02.cpp`)

void sort(std::vector<Page_order::Update>& updates,
          const Page_order::Ruleset& rules);

} // namespace aoc24_05

#endif // DAY_05_H
