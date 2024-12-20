#include "pluto_sim.h"
#include <climits>
#include <string>
#include <vector>

void aoc24_11::Plutonian_simulator::blink()
{
	for (size_t i = 0; i < stones_.size(); ++i) {
		Stone& s = stones_[i];
		long long this_count = 0;

		if (blinks_ == 0) {
			// Initial blink: exhaust all mutations and update pool of stones
			if (s.value() == 0) {
				new_stone(1, s);
				this_count = 1;
			} else {
				int numd = number_of_digits(s.value());
				if (numd % 2 == 0) {
					split_stone(s, numd);
					this_count = 2;
				} else {
					if (2024 > LLONG_MAX / s.value()) {
						throw std::overflow_error("Mult. overflow");
					}
					long long new_value = s.value() * 2024;
					new_stone(new_value, s);
					this_count = 1;
				}
			}
		} else {
			// Future blinks: Update and cache count
			this_count = get_future_count(s, blinks_ + 1);
		}

		s.append_count(this_count);
	}

	++blinks_;
}

//------------------------------------------------------------------------------

long long aoc24_11::Plutonian_simulator::get_future_count(
    const Stone& s, const int number_of_blinks) const
{
	if (number_of_blinks == 0) {
		return 1;
	}
	if (s.blinks_covered() >= number_of_blinks) {
		return s.count_at_blink(number_of_blinks - 1);
	}

	int future_blinks = number_of_blinks - 1;
	long long count = 0;
	if (s.count_at_blink(0) == 2) {
		// Future split
		const auto& addr_pair = s.future_stones();
		const Stone& future_left = *addr_pair.first;

		count += get_future_count(future_left, future_blinks);
		const Stone& future_right = *addr_pair.second;

		count += get_future_count(future_right, future_blinks);
	} else {
		// Single future stone
		const Stone& future_stone = *s.future_stone();

		count += get_future_count(future_stone, future_blinks);
	}
	return count;
}

//------------------------------------------------------------------------------

void aoc24_11::Stone::set_future_stones(std::initializer_list<Stone*> addresses)
{
	fut_stones_.assign(addresses);
}

aoc24_11::Stone* aoc24_11::Stone::future_stone() const
{
	if (fut_stones_.size() != 1) {
		throw std::logic_error(
		    "Requested a single future stone but the stone has "
		    + std::to_string(fut_stones_.size()));
	}
	return fut_stones_[0];
}

std::pair<aoc24_11::Stone*, aoc24_11::Stone*> aoc24_11::Stone::future_stones()
    const
{
	if (fut_stones_.size() != 2) {
		throw std::logic_error(
		    "Requested a pair of future stones but the stone has a total of "
		    + std::to_string(fut_stones_.size()) + " future stones");
	}
	return {fut_stones_[0], fut_stones_[1]};
}

//------------------------------------------------------------------------------

void aoc24_11::Plutonian_simulator::new_stone(long long new_value,
                                              Stone& parent_stone)
{
	auto it = stone_mem_.find(new_value);
	if (it != stone_mem_.end()) {
		parent_stone.set_future_stones({it->second});
	} else {
		stones_.emplace_back(Stone(new_value));
		auto a = stone_mem_.insert({new_value, &stones_.back()});
		parent_stone.set_future_stones({a.first->second});
	}
}

void aoc24_11::Plutonian_simulator::split_stone(Stone& old_stone,
                                                int number_of_digits)
{
	int f = static_cast<int>(std::pow(10, number_of_digits / 2));
	long long val = old_stone.value();
	long long l_val = val / f;
	long long r_val = val - (l_val * f);
	new_stone(l_val, old_stone);
	new_stone(r_val, old_stone);

	Stone* l_stone;
	Stone* r_stone;
	auto it = stone_mem_.find(l_val);
	if (it != stone_mem_.end()) {
		l_stone = it->second;
	} else {
		stones_.emplace_back(Stone(l_val));
		auto a = stone_mem_.insert({l_val, l_stone});
		l_stone = a.first->second;
	}
	it = stone_mem_.find(r_val);
	if (it != stone_mem_.end()) {
		r_stone = it->second;
	} else {
		auto a = stone_mem_.insert({r_val, &stones_.back()});
		r_stone = a.first->second;
	}
	old_stone.set_future_stones({l_stone, r_stone});
}

int aoc24_11::number_of_digits(long long value)
{
	if (value < 1) {
		throw std::invalid_argument(
		    "Number of digits: expected value > 0 but got "
		    + std::to_string(value));
	}
	int n = 0;
	while (value != 0) {
		value /= 10;
		++n;
	}
	return n;
}

//------------------------------------------------------------------------------

void aoc24_11::Plutonian_simulator::add_stone(Stone s)
{
	auto it = stone_mem_.find(s.value());
	if (it != stone_mem_.end()) {
		count_stones_.push_back(it->second);
	} else {
		stones_.push_back(s);
		auto a = stone_mem_.insert({s.value(), &stones_.back()});
		count_stones_.push_back(a.first->second);
	}
}

long long aoc24_11::Plutonian_simulator::number_of_stones() const
{
	long long count = 0;
	for (const Stone* s : count_stones_) {
		count += s->count_at_blink(blinks_ - 1);
	}
	return count;
}

//------------------------------------------------------------------------------

std::istream& aoc24_11::operator>>(std::istream& istr, Stone& stone)
{
	return istr >> stone.val_;
}

std::ostream& aoc24_11::operator<<(std::ostream& ostr, const Stone& stone)
{
	return ostr << stone.val_;
}

void aoc24_11::Plutonian_simulator::print(std::ostream& oss)
{
	oss << blinks_ << ": ";
	bool first = true;
	for (const Stone* s : count_stones_) {
		oss << (first ? "" : " ") << &s;
		first = false;
	}
	oss << '\n';
}
