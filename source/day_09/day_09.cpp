#include "day_09.h"
#include "../lib/puzzle.h"
#include <algorithm>
#include <cassert>
#include <fstream>
#include <sstream>

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
try {
	using namespace aoc24_09;

	aoc24::Puzzle pz{9, "Disk fragmenter", argc, argv};

	constexpr size_t sz_inp{19999};
	constexpr size_t sz_test{19};
	const size_t sz = (pz.is_testing() ? sz_test : sz_inp);
	const std::vector<Block_instructions> instr{
	    read_block_instructions_from_text_file(pz.input_file_path(), sz)};

	// At this point, there should be instructions for creating the following
	// number of blocks:
	constexpr size_t num_bl_inp{94485};
	constexpr size_t num_bl_test{42};
	auto blocks = (pz.is_testing())
	                  ? aoc24::Bitset_factory::create<num_bl_test>()
	                  : aoc24::Bitset_factory::create<num_bl_inp>();

	Disk disk{std::move(blocks), instr};
	disk.backup();
	disk.compact();

	long long p1_checksum{disk.checksum()};
	pz.file_answer(1, "Compact checksum", p1_checksum);

	disk.restore();
	disk.defrag();

	long long p2_checksum{disk.checksum()};
	pz.file_answer(2, "Defrag checksum", p2_checksum);

	pz.print_answers();

	return 0;
} catch (const std::exception& e) {
	std::cerr << "Error: " << e.what() << '\n';
	return 1;
} catch (...) {
	std::cerr << "Unknown error" << '\n';
	return 2;
}

//------------------------------------------------------------------------------

std::vector<aoc24_09::Block_instructions>
aoc24_09::read_block_instructions_from_text_file(const std::string& p,
                                                 const size_t number_of_instr)
{
	std::ifstream ifs{p};
	if (!ifs) {
		throw std::ios_base::failure("Could not open sample input file: " + p);
	}

	std::vector<Block_instructions> instr;
	instr.reserve(number_of_instr);

	bool is_file = true;
	char c{};
	int id = 0;
	while (ifs >> c) {
		if (is_file) {
			instr.emplace_back(Block_instructions{true, c - '0', id});
			++id;
		} else {
			instr.emplace_back(Block_instructions{false, c - '0'});
		}
		is_file = !is_file;
	}

	assert(instr.size() == number_of_instr);

	return instr;
}
