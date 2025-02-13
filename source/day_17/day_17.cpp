#include "../lib/puzzle.h"

int main(
    int argc, char* argv[])
try {
	aoc24::Puzzle pz{17, "Chronospatial Computer", argc, argv};
	return 0;
} catch (const std::exception& e) {
	std::cerr << "Error: " << e.what() << '\n';
	return 1;
} catch (...) {
	std::cerr << "Unknown error" << '\n';
	return 2;
}
