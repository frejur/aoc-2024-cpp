#include "day_09.h"
#include <climits>

namespace {
constexpr int max_id{10000};
}
//------------------------------------------------------------------------------

aoc24_09::Block_instructions::Block_instructions(bool is_in_use,
                                                 int number_of_blocks,
                                                 int file_id)
    : in_use(is_in_use)
    , blocks(valid_num_blocks(in_use, number_of_blocks))
    , id(valid_id(in_use, file_id))
{}

int aoc24_09::Block_instructions::valid_num_blocks(const bool is_in_use,
                                                   const int n)
{
	const int min = (is_in_use) ? 1 : 0;
	if (n < min || n > 9) {
		throw std::invalid_argument(
		    "Invalid number of blocks, expected a value "
		    "between "
		    + std::to_string(min) + " and 9 but got " + std::to_string(n));
	}
	return n;
}

int aoc24_09::Block_instructions::valid_id(const bool is_in_use, const int id)
{
	if (!is_in_use && id != nofile) {
		throw std::invalid_argument("A free block cannot be intitialized with a"
		                            "file id");
	}
	if (is_in_use && (id < 0 || id > max_id)) {
		throw std::invalid_argument("Invalid file id, expected a value "
		                            "between 0 and "
		                            + std::to_string(max_id) + " but got "
		                            + std::to_string(id));
	}
	return id;
}

//------------------------------------------------------------------------------

aoc24_09::Disk::Disk(std::unique_ptr<aoc24::Dyn_bitset> dyn_bitset,
                     const std::vector<Block_instructions>& instructions)
    : block_(std::move(dyn_bitset))
    , dummy_file(File::noid)
{
	size_t pos = 0;
	for (const auto& i : instructions) {
		if (i.in_use) {
			file_.emplace_back(i.id);
		}
		for (size_t b = 0; b < i.blocks; ++b) {
			if (i.in_use) {
				block_->set(pos);
				file_.back().block_adr.emplace_back(pos);
			}
			++pos;
		}
	}
}

void aoc24_09::Disk::compact()
{
	while (true) {
		std::pair<File&, size_t> last_fpair = last_in_use();
		File& file = last_fpair.first;
		size_t last = (file.id != File::noid)
		                  ? file.block_adr[last_fpair.second]
		                  : nopos;
		size_t first = first_free();

		if (last == nopos || first == nopos || last <= first) {
			break;
		}

		swap_w_free_block(file, last_fpair.second, first);
	}
}

void aoc24_09::Disk::print(std::ostream& oss)
{
	oss << block_->to_string() << '\n';
}

long long aoc24_09::Disk::checksum() const
{
	long long c = 0;
	for (const File& f : file_) {
		for (int pos : f.block_adr) {
			if (f.id > 0 && pos > LLONG_MAX / f.id) {
				throw std::overflow_error("Mult. overflow");
			}

			long long s = f.id * pos;

			if (c > LLONG_MAX - s) {
				throw std::overflow_error("Addition overflow");
			}

			c += s;
		}
	}
	return c;
}

size_t aoc24_09::Disk::first_free(size_t start) const
{
	if (block_->count() == block_->size()) {
		return nopos;
	}
	for (int i = start; i < block_->size(); ++i) {
		if (!block_->test(i)) {
			return i;
		}
	}
	return nopos;
}

std::pair<aoc24_09::File&, size_t> aoc24_09::Disk::last_in_use()
{
	if (block_->count() == 0) {
		return {dummy_file, nopos};
	}
	int pos = static_cast<int>(block_->size() - 1);
	for (; pos >= -1; --pos) {
		if (block_->test(pos)) {
			break;
		}
	}

	if (pos > -1) {
		for (int i = static_cast<int>(file_.size() - 1); i >= 0; --i) {
			for (int j = static_cast<int>(file_[i].block_adr.size() - 1);
			     j >= 0;
			     --j) {
				if (file_[i].block_adr[j] == pos) {
					return {file_[i], static_cast<size_t>(j)};
				}
			}
		}
		throw std::logic_error("A block in use was found at address "
		                       + std::to_string(pos)
		                       + ", but it's not connected to a file");
	}
	return {dummy_file, nopos};
}

void aoc24_09::Disk::swap_w_free_block(File& file,
                                       size_t file_block_address_index,
                                       size_t empty_block_pos)
{
	size_t temp = file.block_adr[file_block_address_index];
	file.block_adr[file_block_address_index] = static_cast<int>(empty_block_pos);
	block_->reset(temp);
	block_->set(empty_block_pos);
}
