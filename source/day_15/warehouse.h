#ifndef WAREHOUSE_H
#define WAREHOUSE_H
#include "../lib/bit_grid.h"
#include "robot.h"
#include <memory>
#include <vector>

namespace aoc24_15 {

class Box;

class Warehouse : aoc24::Bit_grid
{
private:
	int num_moves_;
	std::unique_ptr<Robot> rob_;
	std::vector<std::shared_ptr<Box>> box_;

public:
	Warehouse(size_t width,
	          size_t height,
	          std::unique_ptr<aoc24::Dyn_bitset> dyn_bitset);
};

} // namespace aoc24_15

#endif // WAREHOUSE_H
