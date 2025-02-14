#include "io.h"
#include <fstream>
#include <limits>

void aoc24_17::get_register_values_and_instructions_from_file(
    const std::string& file_path,
    std::array<long, 3>& registers,
    std::vector<std::pair<int, int>>& instructions)
{
	std::ifstream ifs{file_path};
	if (!ifs) {
		throw std::logic_error("Could not open file '" + file_path + '\'');
	}

	long invalid_reg_val{std::numeric_limits<long>::min()};
	std::array<char, 3> valid_reg_ids{'A', 'B', 'C'};
	for (int i = 0; i < 3; ++i) {
		Register reg;
		reg.value = invalid_reg_val;
		ifs >> reg;
		if (reg.value == invalid_reg_val) {
			throw std::logic_error("Could not parse register value");
		}
		if (reg.id != valid_reg_ids[i]) {
			throw std::logic_error("Invalid register ID");
		}

		long& r_val = registers[i];
		r_val = reg.value;
	}

	Program prog;
	ifs >> prog;

	if (prog.instructions.empty()) {
		throw std::logic_error("Could not parse program instructions");
	}

	instructions = std::move(prog.instructions);
}

std::istream& aoc24_17::operator>>(std::istream& istr, Register& reg)
{
	std::string reg_txt;
	char reg_id{};
	char colon{};
	long reg_val;

	istr >> reg_txt >> reg_id >> colon >> reg_val;

	if (reg_txt != "Register"
	    || !(reg_id == 'A' || reg_id == 'B' || reg_id == 'C') || colon != ':') {
		istr.clear(std::ios_base::failbit);
		return istr;
	}

	reg.id = reg_id;
	reg.value = reg_val;

	return istr;
}

std::istream& aoc24_17::operator>>(std::istream& istr, Program& prog)
{
	std::string prog_txt;
	std::string instr;

	istr >> prog_txt >> instr;
	if (prog_txt != "Program:" || instr.empty() || (instr.size() + 1) % 4 != 0) {
		istr.clear(std::ios_base::failbit);
		return istr;
	}

	std::vector<std::pair<int, int>> instr_v;
	instr_v.reserve((instr.size() + 1) / 4);
	for (int i = 0; i < instr.size() - 2; i += 4) {
		if (!isdigit(instr[i]) || instr[i + 1] != ',' || !isdigit(instr[i + 2])
		    || (i + 3 < instr.size() && instr[i + 3] != ',')) {
			istr.clear(std::ios_base::failbit);
			return istr;
		}
		instr_v.emplace_back(instr[i] - '0', instr[i + 2] - '0');
	}

	prog.instructions = std::move(instr_v);
	return istr;
}
