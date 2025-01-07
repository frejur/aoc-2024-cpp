#include "robo_grid.h"
#include <array>
#include <iostream>
#include <stdexcept>

aoc24_14::Robo_grid::Robo_grid(
    const std::vector<Robot_position_and_velocity>& robot_pos_and_v,
    size_t grid_size_x,
    size_t grid_size_y)
    : Int_grid(grid_size_x, grid_size_y)
    , ticks_(0)
{
	for (const auto& rpv : robot_pos_and_v) {
		rob_.emplace_back(Robot{rpv.position.x, rpv.position.y});
		rob_.back().update_velocity(rpv.velocity.x, rpv.velocity.y);
		attach_robot(rob_.back());
	}
}

void aoc24_14::Robo_grid::fast_forward(int ticks)
{
	if (ticks <= 0) {
		throw std::invalid_argument("Ticks must be > 0");
	}

	reset();
	for (auto& r : rob_) {
		r.move(ticks);
		const Vec2d new_pos = r.position();
		set(new_pos.x, new_pos.y, int_at(new_pos.x, new_pos.y, true) + 1, true);
	}
	ticks_ += ticks;
}

void aoc24_14::Robo_grid::attach_robot(Robot& r)
{
	if (r.attached_) {
		throw std::logic_error("Cannot attach robot, it is already attached");
	}
	if (!valid_xy(r.pos_.x, r.pos_.y)) {
		throw std::invalid_argument(
		    "Cannot attach robot, its current position ("
		    + std::to_string(r.pos_.x) + ", " + std::to_string(r.pos_.y)
		    + ") is out of bounds");
	}
	r.grid_ = this;
	r.attached_ = true;
}

void aoc24_14::Robot::update_velocity(int x, int y)
{
	v_.x = x;
	v_.y = y;
}

void aoc24_14::Robot::move(int ticks)
{
	if (ticks <= 0) {
		throw std::invalid_argument("Ticks must be > 0");
	}
	if (!attached_) {
		throw std::logic_error(
		    "Robot must be attached to a grid before being moved");
	}
	int x_left = std::abs(v_.x * ticks);
	int y_left = std::abs(v_.y * ticks);
	while (x_left > 0 || y_left > 0) {
		bool do_warp = false;

		if (x_left > 0) {
			// Update X
			int offs = (v_.x > 0) ? (std::min)(x_left, v_.x)
			                      : (std::max(-x_left, v_.x));
			int new_x = pos_.x + offs;
			if (new_x < 0) {
				new_x = -1;
				do_warp = true;
			} else if (new_x >= grid_->width()) {
				new_x = static_cast<int>(grid_->width());
				do_warp = true;
			}
			x_left -= std::abs(new_x - pos_.x);
			pos_.x = new_x;
		}

		if (y_left > 0) {
			// Update Y
			int offs = (v_.y > 0) ? (std::min)(y_left, v_.y)
			                      : (std::max(-y_left, v_.y));
			int new_y = pos_.y + offs;
			if (new_y < 0) {
				new_y = -1;
				do_warp = true;
			} else if (new_y >= grid_->height()) {
				new_y = static_cast<int>(grid_->height());
				do_warp = true;
			}
			y_left -= std::abs(new_y - pos_.y);
			pos_.y = new_y;
		}

		// Warp
		if (do_warp) {
			warp();
		}
	}
}

void aoc24_14::Robo_grid::print(std::ostream& ostr) const
{
	for (const auto& row : g_) {
		for (size_t i = 0; i < width(); ++i) {
			ostr << (i == 0 ? "" : " ") << row[i];
		}
		ostr << '\n';
	}
}

aoc24_14::Vec2d aoc24_14::Robo_grid::find_symmetrical_pattern() const
{
	// Determine the minimum WIDTH of the symmetrical pattern to search for
	const int num_robots = static_cast<int>(rob_.size());
	const int pat_min_w = num_robots / 20;
	if (pat_min_w < 3) {
		throw std::runtime_error("Invalid pattern width");
	}

	// Find all symmetrical patterns on the X-axis (Palindromes, essentially)
	bool has_pat_rows = false;
	size_t start_row = 0;
	std::vector<std::vector<Palindrome>> pat_rows;
	for (size_t i = 0; i < g_.size(); ++i) {
		const std::vector<int>& row = g_[i];
		std::vector<Palindrome> pat = max_w_palindromes(row, pat_min_w);
		if (pat.size() > 0 && !has_pat_rows) {
			has_pat_rows = true;
			start_row = i;
		}
		pat_rows.emplace_back(std::move(pat));
	}

	if (!has_pat_rows) {
		return Vec2d::dummy();
	}

	// Determine the minimum HEIGHT of the symmetrical pattern to search for
	const int pat_min_h = num_robots / 20;
	if (pat_min_h < 1) {
		throw std::runtime_error("Invalid pattern height");
	}

	// Find 'stacked' palindromes
	bool has_min_h_pat = false;
	Vec2d match_pos = Vec2d::dummy();
	size_t max_row = g_.size() - pat_min_h;
	for (size_t row_idx = start_row; (!has_min_h_pat && row_idx < max_row);
	     ++row_idx) {
		std::vector<Palindrome>& row = pat_rows[row_idx];
		if (row.empty()) {
			continue;
		}
		for (size_t p_idx = 0; (!has_min_h_pat && p_idx < row.size()); ++p_idx) {
			Palindrome& p = row[p_idx];
			if (p.is_dummy()) {
				continue;
			}

			size_t c = p.position;
			std::vector<int> radii = {p.radius};
			p = Palindrome::dummy();

			for (size_t pat_r_idx = 1;
			     (!has_min_h_pat && pat_r_idx < pat_min_h);
			     ++pat_r_idx) {
				bool has_stacked_rows = false;
				std::vector<Palindrome>& pat_r = pat_rows[row_idx + pat_r_idx];
				for (Palindrome& stacked_p : pat_r) {
					if (stacked_p.position == c) {
						radii.push_back(stacked_p.radius);
						has_stacked_rows = true;
						stacked_p = Palindrome::dummy();

						// Update position and flag success if on the last iter.
						if (pat_r_idx == pat_min_h - 1) {
							match_pos.x = static_cast<int>(c);
							match_pos.y = static_cast<int>(row_idx);
							has_min_h_pat = true;
						}
					}
				}

				// Abort if no consequetive stacked row
				if (!has_stacked_rows) {
					break;
				}
			}
		}
	}

	return match_pos;

	// How I initially found the easter egg:
	/* Check for symmetry on the y-axis
	constexpr float sym_threshold{0.3};
	int count_sym = 0;
	for (size_t col = 0; col < sz; ++col) {
		for (size_t row = 0; row < sz / 2; ++row) {
			if ((g_[row][col] > 0 && g_[sz - row - 1][col] > 0)
			    || (g_[row][col] == 0 && g_[sz - row - 1][col] == 0)) {
				count_sym += (g_[row][col] + g_[sz - row - 1][col]);
			}
		}
	}
	return (static_cast<float>(count_sym) / rob_.size()) >= sym_threshold;
	*/
}

int aoc24_14::Robo_grid::safety_factor() const
{
	std::array<int, 4> count_quadr{0, 0, 0, 0};
	for (size_t row = 0; row < sz_y; ++row) {
		if (row == sz_y / 2) {
			continue;
		}
		for (size_t col = 0; col < sz; ++col) {
			if (col == sz / 2) {
				continue;
			}
			int val = int_at(col, row);
			if (val == 0) {
				continue;
			}
			int q = (col < sz / 2) ? 0 : 1;
			q += (row < sz_y / 2) ? 0 : 2;
			count_quadr[q] += val;
		}
	}
	return count_quadr[0] * count_quadr[1] * count_quadr[2] * count_quadr[3];
}

void aoc24_14::Robo_grid::reset_time()
{
	reset();
	for (auto& r : rob_) {
		r.reset();
	}
	ticks_ = 0;
}

aoc24_14::Row_result aoc24_14::Robo_grid::count_symmetrical_rows(
    const Vec2d& pos,
    const Direction dir,
    const int max_rows,
    const int num_gaps_allowed) const
{
	bool done = false;
	int cur_row = pos.y;
	int gaps = 0;
	int max_radius = 0;
	int count_rows = 0;

	const int min_max_row = (dir == Direction::Down)
	                            ? static_cast<int>(height()) - 1
	                            : 0;
	const int last_row = (dir == Direction::Down)
	                         ? (std::min)(pos.y + max_rows, min_max_row)
	                         : (std::max)(pos.y - max_rows, min_max_row);
	while (!done) {
		const auto& row_vals = g_[cur_row];
		int r = 0;
		int last_filled_r = 0;
		int lhs = pos.x - (r + 1);
		int rhs = pos.x + (r + 1);
		while (lhs >= 0 && rhs < width()
		       && ((row_vals[lhs] == 0 && row_vals[rhs] == 0)
		           || (row_vals[lhs] > 0 && row_vals[rhs] > 0))) {
			--lhs;
			++rhs;
			++r;
			if (row_vals[lhs] > 0) {
				last_filled_r = r;
			}
		}
		max_radius = (std::max)(max_radius, last_filled_r);

		if (last_filled_r == 0) {
			if (gaps == num_gaps_allowed) {
				count_rows = count_rows - gaps + 1; // Include last 'gap'
				done = true;
			} else {
				++count_rows;
				++gaps;
			}
		} else {
			gaps = 0;
			++count_rows;
		}

		if (!done) {
			if (cur_row == last_row) {
				done = true;
			} else {
				cur_row += (dir == Direction::Down) ? 1 : -1;
			}
		}
	}
	return {count_rows, max_radius * 2 + 1};
}
void aoc24_14::Robo_grid::print_pattern_at_position(const Vec2d& pos,
                                                    std::ostream& ostr) const
{
	if (!valid_xy(pos.x, pos.y)) {
		throw std::invalid_argument("Invalid pattern position");
	}
	constexpr float asp_r = 1.0f / 3 * 2;
	const int max_h = static_cast<int>(rob_.size() * asp_r);

	constexpr int num_row_gaps_allowed{0};

	// Scan upwards
	Row_result rr_up = count_symmetrical_rows(pos,
	                                          Direction::Up,
	                                          max_h,
	                                          num_row_gaps_allowed);
	int start_row = pos.y - rr_up.number_of_rows + 1; // Includes current row

	// Scan downwards
	const int remaining_h = max_h - (1 + pos.y - start_row);
	Row_result rr_down = count_symmetrical_rows({pos.x, pos.y + 1},
	                                            Direction::Down,
	                                            remaining_h,
	                                            num_row_gaps_allowed);
	int end_row = pos.y + rr_down.number_of_rows;

	const int w = (std::max)(rr_up.number_of_rows, rr_down.number_of_rows);
	const int start_x = pos.x - w / 2;
	const int end_x = pos.x + w / 2;
	std::vector<std::vector<int>> pat;
	for (size_t row = start_row; row <= end_row; ++row) {
		std::vector<int> pat_row;
		for (size_t col = start_x; col <= end_x; ++col) {
			pat_row.push_back(g_[row][col]);
		}
		pat.emplace_back(std::move(pat_row));
	}

	std::cout << "(Bounds: x = " << start_x << ", y = " << start_row
	          << ", width = " << w << ", height = " << end_row - start_row
	          << ')' << "\n\n";
	for (const auto& row : pat) {
		for (size_t i = 0; i < row.size(); ++i) {
			ostr << (i == 0 ? "" : " ") << row[i];
		}
		ostr << '\n';
	}
}

void aoc24_14::Robot::warp()
{
	bool invalid_x = false, invalid_y = false;
	int warp_count = 0;
	for (size_t i = 0; i < 2; ++i) {
		auto& x_or_y = (i == 0) ? pos_.x : pos_.y;
		const size_t w_or_h = (i == 0) ? grid_->width() : grid_->height();
		if (x_or_y < 0) {
			// Negative wrap-around
			if (x_or_y <= (w_or_h - 1)) {
				if (i == 0) {
					invalid_x = true;
				} else {
					invalid_y = true;
				}
			} else {
				x_or_y = static_cast<int>(w_or_h) + x_or_y;
			}
			++warp_count;
		} else if (x_or_y >= w_or_h) {
			// Positive wrap-around
			if (x_or_y > (w_or_h - 1) * 2) {
				if (i == 0) {
					invalid_x = true;
				} else {
					invalid_y = true;
				}
			} else {
				x_or_y -= static_cast<int>(w_or_h);
			}
			++warp_count;
		}
	}

	if (warp_count == 0) {
		throw std::logic_error("Cannot warp, the current position does not "
		                       "exceed the boundaries of the grid");
	}
	if (invalid_x || invalid_y) {
		throw std::logic_error("Cannot warp, the relative value of "
		                       + std::string(invalid_x ? "x" : "y")
		                       + " exceeds the grid size");
	}
}

std::istream& aoc24_14::operator>>(std::istream& iss,
                                   Robot_position_and_velocity& rpv)
{
	char c{}, eq{}, comma{};
	int x = 0, y = 0;
	for (size_t i = 0; i < 2; ++i) {
		if (!(iss >> c >> eq >> x >> comma >> y)) {
			return iss;
		}
		if ((i == 0 && c != 'p') || (i == 1 && c != 'v') || eq != '='
		    || comma != ',') {
			iss.clear(std::ios_base::failbit);
			return iss;
		}
		if (i == 0) {
			rpv.position.x = x;
			rpv.position.y = y;
		} else {
			rpv.velocity.x = x;
			rpv.velocity.y = y;
		}
	}
	return iss;
}
