#include "robo_grid.h"
#include <array>
#include <iostream>
#include <stdexcept>

aoc24_14::Robo_grid::Robo_grid(
    const std::vector<Robot_position_and_velocity>& robot_pos_and_v,
    size_t grid_size_x,
    size_t grid_size_y)
    : Int_grid(grid_size_x, grid_size_y)
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
				new_x = grid_->width();
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
				new_y = grid_->height();
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
		for (size_t i = 0; i < sz; ++i) {
			ostr << (i == 0 ? "" : " ") << row[i];
		}
		ostr << '\n';
	}
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
				x_or_y = w_or_h + x_or_y;
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
				x_or_y -= w_or_h;
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
