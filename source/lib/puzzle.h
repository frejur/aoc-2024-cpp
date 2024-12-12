#ifndef PUZZLE_H
#define PUZZLE_H
#include <iostream>
#include <string>
#include <vector>

namespace aoc24 {

class Answer; // Fwd. dec.
//------------------------------------------------------------------------------

class Puzzle
{
public:
	Puzzle(int day, const std::string& name, int argc, char* argv[]);
	std::string input_file_path() const;
	bool is_testing() const { return use_test; }
	void file_answer(int part, const std::string& name, long long value);
	void file_answer(int part,
	                 const std::string& name,
	                 const std::string& value);
	Answer answer(const int part);
	void print_answers(std::ostream& os = std::cout);

private:
	bool use_test;
	bool has_secret;
	int d;
	std::string nm;
	std::string p_inp;
	std::string p_tst;
	std::string p_sec;
	std::vector<Answer> ans_v;

	std::string get(const int part);
	long long get_int(const int part);
};
//------------------------------------------------------------------------------

class Answer
{
public:
	enum class Type { Integer, String };
	enum class Source { Test, Final, Missing };
	Answer(int day,
	       int part,
	       Source source,
	       const std::string& name,
	       long long tentative,
	       long long definite);
	Answer(int day,
	       int part,
	       Source source,
	       const std::string& name,
	       const std::string& tentative,
	       const std::string& definite);
	bool is_correct() const;

	friend std::ostream& operator<<(std::ostream& os, const Answer& a);

private:
	const Type tp;
	const Source src;
	const int d;
	const int pt;
	const long long tent_int;
	const long long defn_int;
	const std::string nm;
	const std::string tent_str;
	const std::string defn_str;
};

//------------------------------------------------------------------------------

void strip_q(std::string& s);

} // namespace aoc24

#endif // PUZZLE_H
