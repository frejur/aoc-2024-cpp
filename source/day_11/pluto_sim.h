#ifndef PLUTO_SIM_H
#define PLUTO_SIM_H
#include <deque>
#include <iostream>
#include <list>
#include <unordered_map>
#include <vector>

namespace aoc24_11 {

class Stone
{
public:
	Stone(long long val)
	    : val_(val)
	{}

	long long value() const { return val_; }
	long long count_at_blink(int i) const { return count_.at(i); }
	int blinks_covered() const { return static_cast<int>(count_.size()); }

	void append_count(long long n) { count_.push_back(n); }
	void set_future_stones(std::initializer_list<Stone*> addresses);

	Stone* future_stone() const;
	std::pair<Stone*, Stone*> future_stones() const;

	friend std::istream& operator>>(std::istream& istr, Stone& stone);
	friend std::ostream& operator<<(std::ostream& istr, const Stone& stone);

private:
	long long val_;
	std::vector<long long> count_;
	std::vector<Stone*> fut_stones_;
};

//------------------------------------------------------------------------------

class Plutonian_simulator
{
public:
	Plutonian_simulator()
	    : blinks_(0)
	{}

	void add_stone(Stone s);

	void blink();

	void print(std::ostream& oss = std::cout);
	long long number_of_stones() const;
	int number_of_blinks() const { return blinks_; }

private:
	int blinks_;
	std::deque<Stone> stones_;
	std::deque<Stone*> count_stones_;
	std::unordered_map<long long, Stone*> stone_mem_;

	void new_stone(long long new_value, Stone& parent_stone);
	void split_stone(Stone& old_stone, int number_of_digits);
	long long get_future_count(const Stone& s, const int number_of_blinks) const;
};

int number_of_digits(long long value);

} // namespace aoc24_11

#endif // PLUTO_SIM_H
