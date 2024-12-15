#ifndef DAY_09_H
#define DAY_09_H
#include "../lib/dynbs.h"
#include <iostream>
#include <string>
#include <vector>

namespace aoc24_09 {

struct Block_instructions; // Fwd. decl.

std::vector<Block_instructions> read_block_instructions_from_text_file(
    const std::string& p, const size_t number_of_instr);
//------------------------------------------------------------------------------
// Part 1 (definitions in `part_01.cpp`)

struct Block_instructions
{
	Block_instructions(bool is_in_use,
	                   int number_of_blocks,
	                   int file_id = nofile);
	bool in_use;
	int blocks;
	int id;
	static constexpr char nofile{-1};
	static int valid_num_blocks(const bool is_in_use, const int n);
	static int valid_id(const bool is_in_use, const int id);
};

struct File
{
	File(int idd)
	    : id(idd)
	{}
	int id;
	std::vector<int> block_adr;
	static constexpr int noid = -1;
};

class Disk
{
public:
	Disk(std::unique_ptr<aoc24::Dyn_bitset> dyn_bitset,
	     const std::vector<Block_instructions>& instructions);
	void compact();
	static constexpr size_t nopos = -1;
	void print(std::ostream& oss = std::cout);
	long long checksum() const;

private:
	std::unique_ptr<aoc24::Dyn_bitset> block_;
	std::vector<File> file_;

	File dummy_file;

	size_t first_free(size_t start = 0) const;
	std::pair<File&, size_t> last_in_use();
	void swap_w_free_block(File& file,
	                       size_t file_block_address_index,
	                       size_t empty_block_pos);
};

//------------------------------------------------------------------------------
// Part 2 (definitions in `part_02.cpp`)

} // namespace aoc24_09

#endif // DAY_09_H
