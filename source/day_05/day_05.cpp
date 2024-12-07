#include "day_05.h"
#include "../lib/puzzle.h"
#include <fstream>
#include <sstream>

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
try {
	using namespace aoc24_05;
	namespace PO = Page_order;

	aoc24::Puzzle pz{5, "Print Queue", argc, argv};

	constexpr size_t sz_inp_rules{140};
	constexpr size_t sz_test_rules{10};
	constexpr size_t sz_inp_updates{140};
	constexpr size_t sz_test_updates{10};
	const size_t sz_rules = (pz.is_testing() ? sz_test_rules : sz_inp_rules);
	const size_t sz_updates = (pz.is_testing() ? sz_test_updates
	                                           : sz_inp_updates);
	PO::Ruleset rules{};
	rules.reserve(sz_rules);

	std::vector<PO::Update> updates;
	updates.reserve(sz_updates);

	read_input_into_rules_and_updates(pz.input_file_path(), rules, updates);

	// Part 1
	validate(updates, rules);
	long int p1_sum_mid{sum_of_correct_middle_page_numbers(updates)};

	// Part 2
	sort(updates, rules);
	long int total_sum_mid{sum_of_correct_middle_page_numbers(updates)};
	long int p2_resorted_sum_mid{total_sum_mid - p1_sum_mid};

	pz.file_answer(1, "Sum of correct middle pages", p1_sum_mid);
	pz.file_answer(2, "Sum of re-sorted middle pages", p2_resorted_sum_mid);
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

void aoc24_05::read_input_into_rules_and_updates(
    const std::string& p,
    Page_order::Ruleset& rules,
    std::vector<Page_order::Update>& updates)
{
	std::ifstream ifs{p};
	if (!ifs) {
		throw std::ios_base::failure("Could not open sample input file: " + p);
	}

	std::string ln;
	std::stringstream ss;

	// Read 'Rules' until "\n\n"
	while (std::getline(ifs, ln) && !ln.empty()) {
		ss << ln << '\n';
	}

	Page_order::Rule r;
	while (ss >> r) {
		rules.add(r.left_page, r.right_page);
	}

	// Reset stream, keep reading 'Updates'
	ss.clear(std::ios_base::goodbit);
	ss.str("");
	while (std::getline(ifs, ln)) {
		ss << ln << '\n';
	}

	Page_order::Update upd;
	while (ss >> upd) {
		updates.push_back(upd);
	}
}
