#include "day_09.h"

void aoc24_09::Disk::backup()
{
	backup_->reset();
	*backup_ |= *block_;

	backup_instr_.clear();
	for (const File& f : file_) {
		for (int a : f.block_adr) {
			backup_instr_.push_back({f.id, a});
		}
	}
}

void aoc24_09::Disk::restore()
{
	block_->reset();
	*block_ |= *backup_;

	int prev_f_idx = -1;
	int adr_idx = 0;
	for (const auto& bp : backup_instr_) {
		if (prev_f_idx == bp.first) {
			++adr_idx;
		} else {
			prev_f_idx = bp.first;
			adr_idx = 0;
		}
		file_[bp.first].block_adr[adr_idx] = bp.second;
	}
}

void aoc24_09::Disk::defrag()
{
	for (int i = static_cast<int>(file_.size()) - 1; i >= 0; --i) {
		File& file = file_[i];
		size_t start_search = 0;
		while (true) {
			std::pair<size_t, size_t> span = first_free_span(start_search);
			size_t start = span.first;
			size_t end = span.second;
			if (start == nopos || start_adr(file) < start) {
				break;
			}

			if (!fits(file, start, end)) {
				start_search = end + 1;
				continue;
			}

			swap_w_free_span(file, start, end);
		}
	}
}
void aoc24_09::Disk::swap_w_free_span(File& file, size_t start, size_t end)
{
	int i = 0;
	for (int& a : file.block_adr) {
		a = start + i;
		block_->set(start + i);
		++i;
	}
}

size_t aoc24_09::Disk::start_adr(const File& file) const
{
	size_t min_pos{0};
	for (auto it = file.block_adr.begin(); it != file.block_adr.end(); ++it) {
		if (it == file.block_adr.begin()) {
			min_pos = static_cast<size_t>(*it);
		} else {
			min_pos = (std::min)(min_pos, static_cast<size_t>(*it));
		}
	}
	return min_pos;
}

std::pair<size_t, size_t> aoc24_09::Disk::first_free_span(size_t start) const
{
	size_t span_start = first_free(start);
	if (span_start == nopos) {
		return {nopos, nopos};
	}

	size_t span_end = span_start;
	while (span_end < block_->size() - 1 && !block_->test(span_end + 1)) {
		++span_end;
	}
	return {span_start, span_end};
}

bool aoc24_09::Disk::file_is_contigious(const File& f) const
{
	if (f.block_adr.size() == 1) {
		return true;
	}

	auto prev = f.block_adr.begin();
	for (auto it = std::next(prev); it != f.block_adr.end(); ++it) {
		if (*it != *prev + 1) {
			return false;
		}
		prev = it;
	}
	return true;
}

bool aoc24_09::Disk::fits(const File& f,
                          size_t span_start,
                          size_t span_end) const
{
	if (span_end == nopos || span_start == nopos) {
		return false;
	}
	if (span_start > span_end) {
		throw std::logic_error("Invalid span, start value exceeds end value");
	}
	return (f.block_adr.size() <= (span_end - span_start + 1));
}
