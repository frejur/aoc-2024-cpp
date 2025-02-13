#ifndef DYNBS_H
#define DYNBS_H
#include <bitset>
#include <memory>

namespace aoc24 {
class Dyn_bitset
{
public:
	// Operators
	virtual bool operator==(const Dyn_bitset& rhs) const = 0;
	virtual bool operator!=(const Dyn_bitset& rhs) const = 0;
	virtual bool operator[](size_t pos) const = 0;
	virtual Dyn_bitset& operator&=(const Dyn_bitset& other) = 0;
	virtual Dyn_bitset& operator|=(const Dyn_bitset& other) = 0;
	virtual Dyn_bitset& operator^=(const Dyn_bitset& other) = 0;
	virtual Dyn_bitset& operator<<=(size_t pos) = 0;
	virtual Dyn_bitset& operator>>=(size_t pos) = 0;

	// Custom arithmetic
	virtual void subtract(Dyn_bitset& mask) = 0;

	// Getters
	virtual bool test(size_t pos) const = 0;
	virtual size_t count() const = 0;
	virtual size_t size() const = 0;
	virtual std::string to_string(char zero = '0', char one = '1') const = 0;

	// Setters
	virtual Dyn_bitset& set() = 0;
	virtual Dyn_bitset& set(size_t pos, bool value = true) = 0;
	virtual Dyn_bitset& reset() = 0;
	virtual Dyn_bitset& reset(size_t pos) = 0;

	// Create mask
	virtual std::unique_ptr<Dyn_bitset> new_copy() const = 0;
	virtual std::unique_ptr<Dyn_bitset> new_reset_copy() const = 0;

	virtual void* raw() = 0;
	virtual ~Dyn_bitset() = default;
};

template<size_t Size>
class Dyn_bitset_implementation : public Dyn_bitset
{
public:
	Dyn_bitset_implementation(){};
	~Dyn_bitset_implementation() = default;

	// Comparison operators
	bool operator==(const Dyn_bitset& rhs) const override
	{
		const auto& other = dynamic_cast<const Dyn_bitset_implementation&>(rhs);
		return b_ == other.b_;
	}

	bool operator!=(const Dyn_bitset& rhs) const override
	{
		return !(*this == rhs);
	}

	// Access bit at position
	bool operator[](size_t pos) const override
	{
		return b_[pos];
	}

	// Bitwise operators
	Dyn_bitset& operator&=(const Dyn_bitset& other) override
	{
		const auto& o = dynamic_cast<const Dyn_bitset_implementation&>(other);
		b_ &= o.b_;
		return *this;
	}

	Dyn_bitset& operator|=(const Dyn_bitset& other) override
	{
		const auto& o = dynamic_cast<const Dyn_bitset_implementation&>(other);
		b_ |= o.b_;
		return *this;
	}

	Dyn_bitset& operator^=(const Dyn_bitset& other) override
	{
		const auto& o = dynamic_cast<const Dyn_bitset_implementation&>(other);
		b_ ^= o.b_;
		return *this;
	}

	Dyn_bitset& operator<<=(size_t pos) override
	{
		b_ <<= pos;
		return *this;
	}

	Dyn_bitset& operator>>=(size_t pos) override
	{
		b_ >>= pos;
		return *this;
	}

	void subtract(Dyn_bitset& mask) override
	{
		b_ &= ~*(static_cast<std::bitset<Size>*>(mask.raw()));
	};

	// Getters
	bool test(size_t pos) const override { return b_.test(pos); }

	size_t count() const override { return b_.count(); }

	size_t size() const override { return b_.size(); }

	std::string to_string(char zero = '0', char one = '1') const override
	{
		return b_.to_string(zero, one);
	}

	// Setters
	Dyn_bitset& set() override
	{
		b_.set();
		return *this;
	}

	Dyn_bitset& set(size_t pos, bool value = true) override
	{
		b_.set(pos, value);
		return *this;
	}

	Dyn_bitset& reset() override
	{
		b_.reset();
		return *this;
	}

	Dyn_bitset& reset(size_t pos) override
	{
		b_.reset(pos);
		return *this;
	}

	std::unique_ptr<Dyn_bitset> new_copy() const override
	{
		return std::unique_ptr<Dyn_bitset>(new Dyn_bitset_implementation<Size>);
	}

	std::unique_ptr<Dyn_bitset> new_reset_copy() const override
	{
		auto ptr = new_copy();
		ptr->reset();
		return ptr;
	}

	void* raw() override { return &b_; }

private:
	std::bitset<Size> b_;
};

class Bitset_factory
{
public:
	template<size_t Size>
	static std::unique_ptr<Dyn_bitset> create()
	{
		return std::unique_ptr<Dyn_bitset>(new Dyn_bitset_implementation<Size>);
	}
};

} // namespace aoc24
#endif // DYNBS_H
